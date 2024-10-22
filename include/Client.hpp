
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"

#define CLIENT_BUFFER 1024

class Request;
class Response;

class Client 
{
	public:
		Client();
		Client(int fd);
		Client(const Client &copy);
		~Client();

		Client &operator=(Client const &src);

		int getFd() const;
		Request* getRequest() const;
		Response* getResponse() const { return _response; };
		time_t getLastRequestTime() const { return _lastRequestTime; };
		void setLastRequestTime(time_t time) { _lastRequestTime = time; };

		void handleRequest();
		void sendResponse(int fd);

		class DecoExc: public std::exception
		{
			public:
				const char* what() const throw();
		};

	
	private:
		int		_fd;
		Request*	_request;
		Response*	_response;
		time_t		_lastRequestTime;
		//Response*	_response; // to create

};

std::ostream& operator<<(std::ostream& os, const Client &client);

#endif //CLIENT_HPP