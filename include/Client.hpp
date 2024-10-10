
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#include "Request.hpp"


#define CLIENT_BUFFER 1024

class Request;

class Client 
{
	public:
		Client();
		Client(int fd);
		Client(const Client &copy);
		~Client();

		Client &operator=(Client const &src);

		int getFd() const;

		void handleRequest();
		void sendResponse(const std::string &response);

		class DecoExc: public std::exception
		{
			public:
				const char* what() const throw();
		};

	
	private:
		int		_fd;
		Request*	_request;
		//Response*	_response; // to create

};

std::ostream& operator<<(std::ostream& os, const Client &client);

#endif //CLIENT_HPP