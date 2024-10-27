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

Client &Client::operator=(Client const &src)
{
    if (this != &src)
    {
        this->_fd = src._fd;
		//this->_server = new Server();
        this->_request = new Request(this);
        this->_response = new Response(this);
		this->_lastRequestTime = src._lastRequestTime;
		this->_requestStatus = src._requestStatus;
    }
    return *this;
}

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
				Log::logVar(Log::INFO,"Raw Request append {}",buffer);

				//Content-Length
				std::string content_length = this->_request->getHeaders("Content-Length");
				Log::logVar(Log::INFO,"Content-Length is {}.", content_length);
				if (!content_length.empty())
				{
					bool valid = true;
					for (std::string::iterator it = content_length.begin(); it != content_length.end(); ++it)
					{
						if (!std::isdigit(*it))
						{
							valid = false;
							break;
						}
					}
					if (valid)
					{
						std::stringstream ss(content_length);
						ss >> len;
						if (ss.fail() || !ss.eof()) 
						Log::log(Log::ERROR,"Invalid content length format.");
					}
				}
				else if (this->_request->getMethod() == "POST")
					Log::logVar(Log::ERROR, "Content-Length contains non-digit characters: ", content_length);
			}
		}
		// Body
		if (headers_received && len > 0)
		{
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
	_requestStatus = true;
}

void Client::sendResponse(int fd)
{
    std::cout << "[DEBUG] Sending response to client: " << this->_fd << std::endl;
    if (this->_response->giveAnswer() == -1)
    {
        std::cerr << "Error: Unable to send response" << std::endl;
        return ;
    }
    int sendResponse = -1;
    if (_fd != -1)
        sendResponse = send(_fd, this->_response->getResponse().c_str(), _response->getResponse().size(), 0);

    if (sendResponse < 0)
    {
        std::cerr << "Error: Unable to send response" << std::endl; // throw une erreur ici
        throw std::runtime_error("Error: Unable to send response");
    }
    else
        std::cout << "Response sent" << std::endl;
    if (this->getResponse()->_done == true)
    {
        if (this->_request->getRequestStatus() != true)
            throw DecoExc();
        std::cout << "[DEBUG] - response is done" << std::endl;
        delete this->_request;
        delete this->_response;
        this->_request = new Request(this);
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