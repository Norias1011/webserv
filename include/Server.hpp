
#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/wait.h>
#include "Config.hpp"

class Server 
{
	public:
		Server();
		Server(const Server &copy);
		~Server();

		Server &operator=(Server const &src);

		long getFD();
		int createSocket();
		int bindAndListen();
		int handleConnexion();
		
	private:
		Config _config;
		struct sockaddr_in _addr;
		long	_fd;
}
