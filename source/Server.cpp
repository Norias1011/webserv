#include "Server.hpp"

//#define IP 1270001  //  to link to parsing
//#define PORT 8001 // to link to parsing
#define MAX_CO 10
#define BUFFER 1024

Server::Server() : _config()
{
}

Server::Server(const Config &config) : _config(config)
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
	}
	return *this;
}


// Creation du server socket, bind, listen to incoming connection
int Server::createSocket()
{
	_serv_list = _config.getConfigServer();
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
			{
				std::cerr << "Error: Unable to create socket" << std::endl;
				return (-1);
			}
			std::cout << "[DEBUG] - success creating the socket" << std::endl;

			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port =  htons(new_server[i].getPort());
			std::cout << "[DEBUG] - success creating the socket" << new_server[i].getPort() << std::endl;
			addr.sin_addr.s_addr = INADDR_ANY;
			bzero(&(addr.sin_zero),8);
			_sockets[new_socket] = addr;
		}
	}
	BindandListen();
	return 0;
}
//AREVOIR LE REINTERPRET cast
void Server::BindandListen()
{
	for (std::map<int, struct sockaddr_in>::iterator it = _sockets.begin(); it != _sockets.end(); it++)
	{
		if (bind(it->first, reinterpret_cast<const struct sockaddr*>(&it->second), sizeof(struct sockaddr)) == -1) 
			std::cerr << "Error: Unable to bind socket" << std::endl;
		std::cout << "[DEBUG] - success binding the socket to" << it->second.sin_port << std::endl;
		if (listen(it->first,MAX_CO) == -1)
			std::cerr << "Error: Unable to listen" << std::endl;
		std::cout << "[DEBUG] - success listenning to port"  << std::endl;
	}
}

// the epoll instance is created and the server socket is added to it to monitor the events.
// especially EPOLLIN means that the socket is ready to read.
// when new connection attemtpt is made by the client, the server socket will be ready to read.
// and the epoll wait will return an event indicating the server socket is ready to read.
// -1 corresponding to timeout  - to see if we add one.

int Server::runServer()
{
	for (std::map<int, struct sockaddr_in>::iterator it = _sockets.begin(); it != _sockets.end(); it++)
	{
		epoll_event events[MAX_CO];
		int epollFd = epoll_create1(EPOLL_CLOEXEC);
		if (epollFd == -1)
		{
			std::cerr << "epoll_create1 fail" << std::endl;
			return -1;
		}
		std::cout << "[DEBUG] - success creating the epoll instance" << std::endl;
		this->addSocket(epollFd,it->first, EPOLLIN);
		while (true)
		{
			std::cout << "Epoll wait for loop" << std::endl;
			int num_fds = epoll_wait(epollFd, events, MAX_CO, -1); 
			std::cout << "num fds:" << num_fds << std::endl;
			if (num_fds == -1)
			{
				std::cerr << "epoll_wait fail" << std::endl;
				return -1;
			}
			for (int i = 0; i < num_fds; i++)
				handleEvent(events[i], it->first ,epollFd);
		}
	}
	return 0;
}

// this will handle the event on the server socket
// check if the event in on the server socket
// then if yes will accept the connection
// check if the event is on the client socket
// will handle the request 

void Server::handleEvent(epoll_event &event, int fd, int epollfd)
{
	try
	{
		if (event.events & (EPOLLOUT | EPOLLET | EPOLLHUP)) // TOCHECK: check the flags
			throw Client::DecoExc();
		if (event.events & EPOLLIN)
		{
			std::cout << "[DEBUG] - event data fd:" << event.data.fd << "fd: " << fd << std::endl;
			if (event.data.fd == fd) 
				this->handleConnection(event.data.fd, epollfd); 
			else
				this->_clients[event.data.fd]->handleRequest(); // TODO
		}
		//if (event.events & EPOLLOUT)
		//	this->_clients[event.data.fd]->sendResponse("Hi"); //->sendResponse(event.data.fd)
	}
	catch (Client::DecoExc &e)
	{
		this->handleDc(event.data.fd);
	}
};


// this will accept the client fd thus create the client socket
// add the new client in the map of clients
// put the client socket in non blocking mode - if the fcntl fails we need to clear the client 
// F_GETL to get  the flag and then block the socket with O_NONBLOCK

void Server::handleConnection(int fd, int epollfd) // TODO -> mettre try catch avec exception dans cette fonction
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
	int flags = fcntl(client_fd, F_GETFL, 0);// TOCHECK: verifier les diff parametres
	if (flags == -1) 
	{	
		std::cerr << "fcntl fail" << std::endl;
		this->handleDc(client_fd);
	}
    if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		this->handleDc(client_fd);
	this->addSocket(epollfd, client_fd, EPOLLIN);
}

void Server::handleDc(int fd)
{
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