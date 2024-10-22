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
#include <iostream>
#include <string.h>
#include <map>
#include <fcntl.h>
#include "../include/Config.hpp"
#include "../include/Client.hpp"

class Client; 
class Config; 

class Server 
{
	public:
		Server();
		Server(const Config &config);
		Server(const Server &copy);
		~Server();

		Server &operator=(Server const &src);

		long getSocketFd() const;
		long getEpollFd() const;

		int createSocket();
		void BindandListen();
		int runServer();
		void handleConnection(int fd, int epollfd);
		void handleDc(int fd);
		void handleEvent(epoll_event &event, int epollfd);
		void addSocket(int epollFd, int socketFd, uint32_t flags);
		
	private:
		Config _config;
		std::map<int, struct sockaddr_in> _sockets;
		std::map<int,Client*> _clients;
		std::map<std::string, std::vector<ConfigServer> > _serv_list;
		std::vector<ConfigServer> new_server;
		bool _done;
		bool _working;
		bool _break;
		int _epollFd;
	
};


#endif // SERVER_HPP