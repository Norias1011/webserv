#include "Client.hpp"
#include "Server.hpp"
#include <bits/basic_string.h>
#include <stdexcept>

/*Client::Client() : _fd(-1),_request(new Request(this)) , _response(new Response(this)), _server(new Server()), _lastRequestTime(0), _requestStatus(false)
{
}*/

Client::Client(int fd, Server* server) : _fd(fd), _server(server),_request(new Request(this)), _response(new Response(this)), _lastRequestTime(0), _requestFinish(false)
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


void Client::handleRequest()
{
	std::string request;
	std::string body;
	bool headers_received = false;
	unsigned long len = 0;

	if (this->_requestFinish == true)
		return;
	Log::logVar(Log::DEBUG,"Handling request from client fd {}.", this->_fd);
    while (true)
	{
		char buffer[CLIENT_BUFFER + 1];
		bzero(buffer, CLIENT_BUFFER + 1);
		int bytes = recv(this->_fd, buffer, CLIENT_BUFFER , 0);

		if (bytes <= 0) 
		{
			throw DecoExc();
		}
		request.append(buffer,bytes);

		if (!headers_received)
		{
			size_t pos = request.find("\r\n\r\n");
			if (pos != std::string::npos)
			{
				headers_received = true;
				std::string headers = request.substr(0,pos);
				Log::log(Log::DEBUG,"Beginning parsing of the Headers.");
				if (this->_request->parseRequestHeaders(headers) == -1)
					Log::log(Log::ERROR,"Error in the parsing of the Headers.");
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
		Log::logVar(Log::ERROR, "Len is: {}", len);
		if (headers_received)
		{
			Log::logVar(Log::ERROR, "headers_received is ok ? {}", headers_received);
			Log::log(Log::DEBUG,"Getting server configuration to handle the request...");
			this->_request->findConfigServer();
			Log::log(Log::DEBUG,"End of configuration of the request , now parsing the body...");
		}
		// Body
		if (headers_received && len > 0)
		{
			Log::log(Log::DEBUG,"End of configuration of the request , now parsing the body for POST method..");
			size_t body_p = request.find("\r\n\r\n") + 4;
			std::string body = request.substr(body_p);
			this->_request->setBody(body);
			if (body.size() >= len)
			{
				this->_request->setRequest(request);
				this->_request->parseBody();
				break;
			}
		}
		else if (len == 0)
			break;
	}
	Log::log(Log::DEBUG,"Request headers, configuration server and boy are ready");
	this->_requestFinish = true;
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
        std::cerr << "Error: Unable to send response" << std::endl; // throw une erreur ici
        throw std::runtime_error("Error: Unable to send response");
    }
    else
	{
        std::cout << "Response sent" << std::endl;
	}
    if (this->getResponse()->_responseDone == true)
    {
        if (this->getRequestFinish() != true)
            throw DecoExc();
		Log::logVar(Log::DEBUG, "getRequestFinish:", this->getRequestFinish());	
		Log::logVar(Log::DEBUG, "getResponse() _done ? : ", this->getResponse()->_responseDone);	
		Log::log(Log::DEBUG, "the response is sent we need to reset:");	
        delete this->_request;
		this->_request = new Request(this);
        delete this->_response;
        this->_response = new Response(this);
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