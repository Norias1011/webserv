
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "Log.hpp"
#include <bits/basic_string.h>

#define CLIENT_BUFFER 1024

class Request;
class Response;
class Server; 

class Client 
{
	public:
		Client();
		Client(int fd, Server* server);
		Client(const Client &copy);
		~Client();

		Client &operator=(Client const &src);

		int getFd() const;
		Server* getServer() const { return _server; };
		Request* getRequest() const { return _request; };
		Response* getResponse() const { return _response; };
		bool getRequestStatus() const { return _requestStatus; };
		time_t getLastRequestTime() const { return _lastRequestTime; };

		void setLastRequestTime(time_t time) { _lastRequestTime = time; };

		void handleRequest(int fd);
		void sendResponse(int fd);

		void changeEpoll(int fd);

		void setRequestStatus(bool status) { _requestStatus = status; };

		class DecoExc: public std::exception
		{
			public:
				const char* what() const throw();
		};

	
	private:
		int		_fd;
		Server*		_server;
		Request*	_request;
		Response*	_response;
		time_t		_lastRequestTime;
		bool		_requestStatus;

};

std::ostream& operator<<(std::ostream& os, const Client &client);

#endif //CLIENT_HPP