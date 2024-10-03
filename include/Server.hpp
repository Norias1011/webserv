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
#include "Client.hpp"

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
		void handleConnection();
		void handleDc();
		void handleEvent();
		
	private:
		Config _config;
		std::map<int,Client*> _clients;
		struct sockaddr_in _addr;
		long	_socketFd;
		long 	_epollFd;
		struct epoll_event _ev; // adding to the epoll instance 
		//std::map<long, std::string>	_sockets;
	
	void addSocket(int epollFd, int socketFd, uint32_t flags);
};
