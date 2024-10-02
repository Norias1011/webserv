#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/wait.h>
#include "Server.hpp"

#define IP "11.11.11.11"  //  to link to parsing
#define PORT 8002 // to link to parsing

/*#define MYPORT 3490 // to link to what?
#define DESTPORT 8002 // to link to parsing
#define DEST_IP "132.241.5.10" // to link to parsing
#define MAX_CO 10 // to link to main
*/

Server::Server()
{
};

Server::Server(Server const &copy)
{
	*this = copy;
};

Server::~Server()
{
};

Server &operator=(Server& const rhs)
{
	if (this != rhs)
	{
		_fd = rhs._fd;
		_addr = rhs._addr;
		_confg = rhs._config;
	}
	return *this;

};

long getFD();
void createSocket();
void acceptConnection();
void closeConnection();


int Server::createSocket()
{
	_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_fd == -1)
	{
		std::cerr << "Error: Unable to create socket" << std::endl;
        return (-1);
	}
	_addr.sin_family = AF_INET;
	_addr.sin_port = htons(PORT);
	_addr.sin_addr.s_addr = htonl(IP);
	bzero(&(my_addr.sin_zero),8);
    if (bind(_fd, (struct sockaddr *)&_addr, sizeof(struct sockaddr)) == -1) 
	{
        std::cerr << "Error: Unable to bind socket" << std::endl;
        return (-1);
    }

	if (listen(_fd,1000) == -1)
	{
		std::cerr << "Error: Unable to listen" << std::endl;
		return(-1);
	}
	return _fd;
}


