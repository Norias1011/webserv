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
#include <exception>
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

		void createSocket();
		void BindandListen();
		void runServer();
		void handleConnection(int fd);
		void handleDc(int fd);
		void handleEvent(epoll_event *event);
		void addSocket(int epollFd, int socketFd, uint32_t flags);
		void closeServer();

		class ErrorException : public std::exception
		{
			private:
				std::string _error_message;
			public:
				ErrorException(std::string error_message) throw()
				{
					_error_message = "[ERROR] Server Error: " + error_message;
				}
				virtual const char *what() const throw()
				{
					return (_error_message.c_str());
				}
				virtual ~ErrorException() throw() {}
		};
		
	private:
		Config _config;
		std::map<int, struct sockaddr_in> _sockets;
		std::map<int,Client*> _clients;
		std::map<std::string, std::vector<ConfigServer> > _serv_list;
		bool _done;
		bool _working;
		bool _break;
		int _epollFd;
		std::vector<ConfigServer> _new_server;
	
};


#endif // SERVER_HPP