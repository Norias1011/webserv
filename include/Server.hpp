#ifndef SERVER_HPP
#define SERVER_HPP

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
#include <map>
#include "Config.hpp"
#include "Client.hpp"

class Server 
{
	public:
		Server();
		Server(const Server &copy);
		~Server();

		Server &operator=(Server const &src);

		long getSocketFd() const;
		long getEpollFd() const;

		int createSocket();
		int runServer();
		void handleConnection(int fd);
		void handleDc(int fd);
		void handleEvent(epoll_event &event);
		
	private:
		Config _config;
		std::map<int,Client*> _clients;
		struct sockaddr_in _addr;
		long	_socketFd;
		long 	_epollFd;
	
	void addSocket(int epollFd, int socketFd, uint32_t flags);
};

#endif // SERVER_HPP