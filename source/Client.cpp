#include "Client.hpp"
#include "Server.hpp"
#include <bits/basic_string.h>
#include <stdexcept>

/*Client::Client() : _fd(-1),_request(new Request(this)) , _response(new Response(this)), _server(new Server()), _lastRequestTime(0), _requestStatus(false)
{
}*/

Client::Client(int fd, Server* server) : _fd(fd), _server(server),_request(new Request(this)), _response(new Response(this)), _lastRequestTime(0), _requestStatus(false)
{    
}

Client::Client(const Client &copy)
{
    *this = copy;
}

Client::~Client()
{
    if (_request != NULL)
	    delete _request;
    if (_response != NULL)
	   	delete _response;
    if (_fd != -1)
        close(_fd);
}

/*Client &Client::operator=(Client const &src)
{
    if (this != &src)
    {
        this->_fd = src._fd;
		this->_server = new Server();
        this->_request = new Request(this);
        this->_response = new Response(this);
		this->_lastRequestTime = src._lastRequestTime;
		this->_requestStatus = src._requestStatus;
    }
    return *this;
}*/

int Client::getFd() const
{
    return _fd;
}


void Client::handleRequest(int fd)
{
	char buffer[CLIENT_BUFFER + 1];
	bzero(buffer, CLIENT_BUFFER + 1);
	int bytes = recv(this->_fd, buffer, CLIENT_BUFFER , 0);
	Log::logVar(Log::DEBUG,"bytes received {}.", bytes);

	if (bytes > 0)
	{
		buffer[bytes] = '\0';
	}
	else if (bytes < 0)
		throw std::runtime_error("Error recv function");
	else if (bytes == 0)
		throw DecoExc();
	
	if (this->_requestStatus == true)
	{
		Log::log(Log::DEBUG,"Request already handled");
		return;
	}
	std::string raw_request(buffer,bytes);
	
	if (raw_request.empty())
	{
		Log::log(Log::ERROR,"the raw request is empty");
		return;
	}
	this->_request->parseRequest(raw_request);
	changeEpoll(fd);
}

/*void Client::handleRequest(int fd)
{
	Log::log(Log::DEBUG, "Entering the Client:handleRequest part");
	std::string request;
	std::string body;
	
	bool headers_received = false;
	unsigned long len = 0;

	if (this->_requestStatus == true)
		return;
	Log::logVar(Log::DEBUG,"Handling request from client fd {}.", this->_fd);
    while (true)
	{
		char buffer[CLIENT_BUFFER + 1];
		bzero(buffer, CLIENT_BUFFER + 1);
		int bytes = recv(this->_fd, buffer, CLIENT_BUFFER , 0);
		Log::logVar(Log::DEBUG,"bytes received {}.", bytes);
		
		if (bytes <= 0)
		{
			this->_request->setServerCode(405);
			throw DecoExc();
		}
		
		request.append(buffer,bytes);
		if (!headers_received)
		{
			Log::log(Log::ERROR, "Entering the !headers_received part");
			size_t pos = request.find("\r\n\r\n");
			if (pos != std::string::npos || request.find("chunked") != std::string::npos)
			{
				Log::log(Log::ERROR, "We find the end of the headers !");
				headers_received = true;
				std::string headers = request.substr(0,pos);
				Log::log(Log::DEBUG,"Beginning parsing of the Headers.");
				if (this->_request->parseRequestHeaders(headers) == -1)
				{
					Log::log(Log::ERROR,"Error in the parsing of the Headers.");
					this->_request->findConfigServer();
					return;
				}
				else
					Log::log(Log::DEBUG,"Headers are OK.");

				//Content-Length
				std::string content_length = this->_request->getHeaders("Content-Length");
				Log::logVar(Log::INFO,"Content-Length is {}.", content_length);
				if (!content_length.empty())
				{
					std::string filtered_length;
					for (std::string::iterator it = content_length.begin(); it != content_length.end(); ++it)
					{
						if (std::isdigit(*it))
							filtered_length += *it;
					}
					std::stringstream ss(filtered_length);
					ss >> len;
					if (ss.fail() || !ss.eof()) 
						Log::log(Log::ERROR,"Invalid content length format.");
					Log::logVar(Log::INFO, "Len of the body is: {}", len);
				}
				else if (this->_request->getMethod() == "POST")
					Log::log(Log::ERROR, "Content-Length contains non-digit characters");
			}
		}
	
		//once the heders are received, we can find the configuration
		//and if the headers are not fully received also to get the Error pages.
		Log::logVar(Log::ERROR, "Len of the body is: {}", len);
		Log::logVar(Log::ERROR, "headers_received is ok ? {}", headers_received);
		Log::log(Log::DEBUG,"Getting server configuration to handle the request...");
		this->_request->findConfigServer();
		this->_request->setRequest(request);
		// Body

		if ((headers_received && len > 0) || this->_request->getHeaders("Transfer-Encoding:") == "chunked")
		{
			Log::log(Log::DEBUG,"End of configuration of the request , now parsing the body for POST method..");
			size_t body_p = request.find("\r\n\r\n") + 4;
			std::string body = request.substr(body_p);
			this->_request->setBody(body);
			if (body.size() >= len)
			{
				this->_request->parseBody();
				break;
			}
		}
		else if (headers_received && len == 0)
		{	
			if (_request->getMethod() == "DELETE")
				_request->handleDelete();
			break;
		}
		else if (!headers_received && this->_request->getHeaders("Transfer-Encoding:") != "chunked")
			break;
	}
	Log::logVar(Log::INFO, "full request received:", request);
	if (!headers_received)
	{
		Log::log(Log::ERROR,"Invalid Request, missing headers - Error 400.");
		this->_request->setServerCode(400);
		this->_requestStatus = true;
	}
	else
	{
		Log::log(Log::DEBUG,"Request headers, configuration server and body are ready");
		this->_requestStatus = true;
	}
	changeEpoll(fd);
}*/

void Client::changeEpoll(int fd)
{
	epoll_event ev;
	ev.events = EPOLLOUT;
	ev.data.fd = _fd;
	epoll_ctl(fd, EPOLL_CTL_MOD, _fd, &ev);
}

void Client::sendResponse(int fd)
{
	Log::logVar(Log::DEBUG, "Sending response to client with fd {}",_fd);
    if (this->_response->giveAnswer() == -1)
    {
		Log::log(Log::ERROR, "Error: Unable to send response");
        return ;
    }
    int sendResponse = -1;
	Log::logVar(Log::DEBUG, "sendResponse fd is :",_fd);
    if (_fd != -1)
        sendResponse = send(_fd, this->_response->getResponse().c_str(), _response->getResponse().size(), 0);
    if (sendResponse < 0)
    {
		Log::log(Log::ERROR, "Error: Unable to send response");
		Log::logVar(Log::DEBUG, "sendResponse is :",sendResponse);
        throw std::runtime_error("Error: Unable to send response");
    }
    else
	{
        std::cout << "Response sent" << std::endl;
	}
    if (this->getResponse()->_done == true)
    {
		std::cout << "[DEBUG] - Response is done ... request and response are reseting.. " << std::endl;
       	if (this->getRequestStatus() != true)
            throw DecoExc();
		delete this->_response;	
        delete this->_request;
		this->_request = new Request(this);
        this->_response = new Response(this);
		this->setRequestStatus(false);
        epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = _fd;
        epoll_ctl(fd, EPOLL_CTL_MOD, _fd, &ev);
    }
}

const char* Client::DecoExc::what() const throw()
{
    return "Error: Disconnected from the server";
}

std::ostream& operator<<(std::ostream& os, const Client &client)
{
    os << "Client fd: " << client.getFd();
    return os;
}