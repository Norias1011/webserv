#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <netinet/in.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/wait.h>
#include <sys/epoll.h>
#include <vector>
#include "Server.hpp"

#define IP 1270001  //  to link to parsing
#define PORT 8002 // to link to parsing
#define MAX_CO 10

Server::Server()
{
}

Server::Server(Server const &copy)
{
	*this = copy;
}

Server::~Server()
{
}

Server &Server::operator=(Server const &rhs)
{
	if (this != &rhs)
	{
		_socketFd = rhs._socketFd;
		_epollFd = rhs._epollFd;
		_addr = rhs._addr;
		_config = rhs._config; //check this
	}
	return *this;
}

long Server::getSocketFd()
{
	return _socketFd;
}

long Server::getEpollFd()
{
	return _epollFd;
}

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
	_addr.sin_addr.s_addr = htonl(IP);
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

int Server::runServer()
{
	epoll_event events[MAX_CO];
	_epollFd = epoll_create1(EPOLL_CLOEXEC);
	if (_epollFd == -1)
	{
		std::cerr << "epoll_create1 fail" << std::endl;
        return -1;
	}
	if (epoll_ctl(_epollFd, EPOLL_CTL_ADD,_socketFd, &events) == -1)
	{
		std::cerr << "epoll_ctl fail" << std::endl;
		return -1;
	}
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
		// add un timeout ?
	}
}

void Server::handleEvent(epoll_event &event)
{

}