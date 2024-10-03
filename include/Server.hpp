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
#include "Config.hpp"

class Server 
{
	public:
		Server();
		Server(const Server &copy);
		~Server();

		Server &operator=(Server const &src);

		long getSocketFd();
		long getEpollFd();

		int createSocket();
		int runServer();
		void handleEvent();
		
	private:
		Config _config;
		struct sockaddr_in _addr;
		long	_socketFd;
		long 	_epollFd;
		struct epoll_event _ev; // adding to the epoll instance 
		//std::map<long, std::string>	_sockets;
};
