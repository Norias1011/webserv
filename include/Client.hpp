
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#define CLIENT_BUFFER 1024

class Client 
{
	public:
		Client();
		Client(int fd);
		Client(const Client &copy);
		~Client();

		Client &operator=(Client const &src);

		void handleRequest();
		void sendResponse(const std::string &response);

		class EpollErrorExc: public std::exception
		{
			public:
				const char* what() const throw();
		};
	
	private:
		int		_fd;
		//Request*	_request; // to do
		//Response*	_response; // to create

};

#endif //CLIENT_HPP