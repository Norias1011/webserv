#include "Server.hpp"

//#define IP 1270001  //  to link to parsing
#define PORT 8001 // to link to parsing
#define MAX_CO 10
#define BUFFER 1024

Server::Server() : _socketFd(-1), _epollFd(-1)
{
}

Server::Server(Server const &copy)
{
	*this = copy;
}

Server::~Server()
{
	close(_socketFd);
	close(_epollFd);
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
		_socketFd = rhs._socketFd;
		_epollFd = rhs._epollFd;
		_addr = rhs._addr;
		//_config = rhs._config; //check this
	}
	return *this;
}

// getters mais je sais pas si ils vont etre neceassaires ?=> a voir
long Server::getSocketFd() const
{
	return _socketFd;
}

long Server::getEpollFd() const
{
	return _epollFd;
}

// Creation du server socket, bind, listen to incoming connection
int Server::createSocket()
{
	_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socketFd == -1)
	{
		std::cerr << "Error: Unable to create socket" << std::endl;
        return (-1);
	}
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(PORT);
	_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(_addr.sin_zero),8);
    if (bind(_socketFd, (struct sockaddr *)&_addr, sizeof(struct sockaddr)) == -1) 
	{
        std::cerr << "Error: Unable to bind socket" << std::endl;
        return (-1);
    }
	if (listen(_socketFd,MAX_CO) == -1)
	{
		std::cerr << "Error: Unable to listen" << std::endl;
		return(-1);
	}
	return _socketFd;
}

// the epoll instance is created and the server socket is added to it to monitor the events.
// especially EPOLLIN means that the socket is ready to read.
// when new connection attemtpt is made by the client, the server socket will be ready to read.
// and the epoll wait will return an event indicating the server socket is ready to read.
// -1 corresponding to timeout  - to see if we add one.

int Server::runServer()
{
	epoll_event events[MAX_CO];
	_epollFd = epoll_create1(EPOLL_CLOEXEC);
	if (_epollFd == -1)
	{
		std::cerr << "epoll_create1 fail" << std::endl;
		// to close here
        return -1;
	}
	addSocket(_epollFd, _socketFd, EPOLLIN);
	while (true)
	{
		int num_fds = epoll_wait(_epollFd, events, MAX_CO, -1); 
		if (num_fds == -1)
		{
			std::cerr << "epoll_wait fail" << std::endl;
			return -1;
		}
		for (int i = 0; i < num_fds; i++)
			handleEvent(events[i]);
	}
	return 0;
}

// this will handle the event on the server socket
// check if the event in on the server socket
// then if yes will accept the connection
// check if the event is on the client socket
// will handle the request 

void Server::handleEvent(epoll_event &event)
{
	try
	{
		if (event.events & (EPOLLOUT | EPOLLET | EPOLLHUP)) // TOCHECK: check the flags
			throw Client::EpollErrorExc();
		if (event.events & EPOLLIN)
		{
			if (event.data.fd == _socketFd) 
				this->handleConnection(event.data.fd); 
			else
				this->_clients[event.data.fd]->handleRequest(); // TODO
		}
		if (event.events & EPOLLOUT)
			this->_clients[event.data.fd]->sendResponse("Hi"); //->sendResponse(event.data.fd
	}
	catch (Client::EpollErrorExc &e)
	{
		this->handleDc(event.data.fd);
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
	std::cerr << "DEBUG - accept work" << std::endl;
	this->_clients[client_fd] = new Client(client_fd);
	int flags = fcntl(client_fd, F_GETFL, 0);// TOCHECK: verifier les diff parametres
	if (flags == -1) 
	{	
		std::cerr << "fcntl fail" << std::endl;
		this->handleDc(client_fd);
	}
    if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1)
		this->handleDc(client_fd);
	this->addSocket(_epollFd, client_fd, EPOLLIN); // TOCHECK: verifier les flags REQUEST FLAGS
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