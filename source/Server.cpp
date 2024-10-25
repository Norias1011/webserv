#include "Server.hpp"

//#define IP 1270001  //  to link to parsing
//#define PORT 8001 // to link to parsing
#define MAX_CO 10
#define BUFFER 1024

Server::Server() : _config(), _done(false), _working(false), _break(false), _epollFd(-1)
{
}

Server::Server(const Config &config) : _config(config), _done(false), _working(false), _break(false), _epollFd(-1)
{
}

Server::Server(Server const &copy)
{
	*this = copy;
}

Server::~Server()
{
	for (std::map<int, Client*>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		delete it->second;
	}
	this->_clients.clear();
	for (std::map<int, struct sockaddr_in>::iterator it = this->_sockets.begin(); it != this->_sockets.end(); it++)
	{
		close(it->first);
	}
	this->_sockets.clear();
	if (_epollFd != -1)
		close(_epollFd);
}

Server &Server::operator=(Server const &rhs)
{
	if (this != &rhs)
	{
		_sockets = rhs._sockets;
		_clients = rhs._clients;
		_config = rhs._config;
		_serv_list = rhs._serv_list;
		new_server = rhs.new_server;
		_done = rhs._done;
		_working = rhs._working;
		_break = rhs._break;
	}
	return *this;
}


// Creation du server socket, bind, listen to incoming connection
void Server::createSocket()
{
	_serv_list = _config.getConfigServer();
	_epollFd = epoll_create1(EPOLL_CLOEXEC);
	if (_epollFd == -1)
		throw Server::ErrorException("epoll_create1 fail");
	for (std::map<std::string, std::vector<ConfigServer> >::iterator it = _serv_list.begin(); it != _serv_list.end(); it++)
	{
		std::cout << "[DEBUG] - before first it" << std::endl;
		new_server = it->second;
		//for (std::map<int, struct sockaddr_in>::iterator it2 = _sockets.begin(); it2 != _sockets.end(); it2++)
		for (size_t i = 0; i < new_server.size(); i++)
		{
			std::cout << "[DEBUG] - success creating the socket" << std::endl;
			int new_socket = socket(AF_INET, SOCK_STREAM, 0);
			if (new_socket == -1)
				throw Server::ErrorException("Unable to create socket");
			std::cout << "[DEBUG] - success creating the socket" << std::endl;

			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port =  htons(new_server[i].getPort());
			std::cout << "[DEBUG] - success creating the socket" << new_server[i].getPort() << std::endl;
			addr.sin_addr.s_addr = INADDR_ANY;
			bzero(&(addr.sin_zero),8);
			_sockets[new_socket] = addr;
			this->addSocket(_epollFd, new_socket, EPOLLIN);
			std::cout << "[DEBUG] - success creating the epoll instance" << std::endl;
		}
	}
	BindandListen();
	_done = true;
}
//AREVOIR LE REINTERPRET cast
void Server::BindandListen()
{
	for (std::map<int, struct sockaddr_in>::iterator it = _sockets.begin(); it != _sockets.end(); it++)
	{
		if (bind(it->first, reinterpret_cast<const struct sockaddr*>(&it->second), sizeof(struct sockaddr)) == -1) 
			std::cerr << "Error: Unable to bind socket" << std::endl;
		std::cout << "[DEBUG] - success binding the socket to" << it->second.sin_port << std::endl;
		if (listen(it->first, MAX_CO) == -1)
			std::cerr << "Error: Unable to listen" << std::endl;
		std::cout << "[DEBUG] - success listenning to port"  << std::endl;
	}
}

// the epoll instance is created and the server socket is added to it to monitor the events.
// especially EPOLLIN means that the socket is ready to read.
// when new connection attemtpt is made by the client, the server socket will be ready to read.
// and the epoll wait will return an event indicating the server socket is ready to read.
// -1 corresponding to timeout  - to see if we add one.

void Server::runServer()
{
	if (!_done)
		throw Server::ErrorException("Server not ready");
	_working = true;
	epoll_event events[MAX_CO];
	time_t before_loop_time = time(0);
	while (this->_working)
	{
		std::cout << "Epoll wait for loop" << std::endl;
		int num_fds = epoll_wait(_epollFd, events, MAX_CO, -1); 
		std::cout << "num fds:" << num_fds << std::endl;
		if (num_fds < 0)
			throw Server::ErrorException("epoll_wait fail");
		for (int i = 0; i < num_fds; i++)
			handleEvent(&events[i]);
		time_t now = time(0);
		if (now - before_loop_time > 10)
		{
			std::map<int, Client*>::iterator it = _clients.begin();
			while (it != this->_clients.end())
			{
				it->second->getRequest()->timeoutChecker();
				if (now - it->second->getLastRequestTime() > 10)
				{
					std::cout << "Client timeout: " << NumberToString(it->first) << std::endl;
					epoll_event ev;
					ev.data.fd = it->first;
					epoll_ctl(_epollFd, EPOLL_CTL_DEL, it->first, &ev);
					delete it->second;
					_clients.erase(it++);
				}
				else
					it++;
			}
			before_loop_time = now;
		}
	}
}

// this will handle the event on the server socket
// check if the event in on the server socket
// then if yes will accept the connection
// check if the event is on the client socket
// will handle the request 

void Server::handleEvent(epoll_event *event)
{
	try
	{
		if (event->events & (EPOLLHUP | EPOLLERR | !(EPOLLIN)))
			throw Client::DecoExc();
		if (event->events & EPOLLIN)
		{
			std::cout << "[DEBUG] - event data fd:" << event->data.fd << "fd: " << std::endl;
			if (this->_clients.find(event->data.fd) == this->_clients.end())
				this->handleConnection(event->data.fd); 
			else
			{
				this->_clients[event->data.fd]->setLastRequestTime(time(0));
				this->_clients[event->data.fd]->handleRequest(); 
			}
		}
		if (event->events & EPOLLOUT) // check the CGI here
		{
			std::cout << "[DEBUG] - entering the response part" << std::endl;
			this->_clients[event->data.fd]->setLastRequestTime(time(0));
			if (this->_clients[event->data.fd]->getRequest()) //&& this->_clients[event->data.fd]->getRequest()->getRequestStatus() == true)
				this->_clients[event->data.fd]->sendResponse(_epollFd);
		}
		std::cout << "[DEBUG] - event handled" << std::endl;
	}
	catch (Client::DecoExc &e)
	{
		this->handleDc(event->data.fd);
	}
};


// this will accept the client fd thus create the client socket
// add the new client in the map of clients
// put the client socket in non blocking mode - if the fcntl fails we need to clear the client 
// F_GETL to get  the flag and then block the socket with O_NONBLOCK

void Server::handleConnection(int fd) // TODO -> mettre try catch avec exception dans cette fonction
{
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(struct sockaddr);

	int client_fd = accept(fd,(struct sockaddr *) &addr, &addrlen);
	if (client_fd == -1)
	{
		std::cerr << "accepting the client fail" << std::endl;
		this->handleDc(client_fd);
	}
	std::cout << "[DEBUG] - success accepting connection with client fd" << client_fd << std::endl;
	this->_clients[client_fd] = new Client(client_fd);
	int flags = fcntl(client_fd, F_SETFL, O_NONBLOCK);// TOCHECK: verifier les diff parametres
	if (flags == -1) 
	{	
		std::cerr << "fcntl fail" << std::endl;
		this->handleDc(client_fd);
	}
    if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		this->handleDc(client_fd);
	this->addSocket(_epollFd, client_fd, EPOLLIN | EPOLLOUT);
	std::cout << "[DEBUG] - success adding the client to the epoll instance" << std::endl;
}

void Server::handleDc(int fd)
{
	std::cout << "[DEBUG] - Client disconnected: " << NumberToString(fd) << std::endl;
	epoll_event ev;
	ev.data.fd = fd;
	epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, &ev);
	close(fd);
	delete _clients[fd];
	_clients.erase(fd);
}

void Server::addSocket(int epollFd, int fd, uint32_t flags)
{
	struct epoll_event ev;
	ev.events = flags;
	ev.data.fd = fd;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev) == -1)
	{
		std::cerr << "epoll_ctl fail" << std::endl;
		close(fd);
	}
}

void Server::closeServer()
{
	_working = false;
	_break = true;
}