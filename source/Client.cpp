#include "Client.hpp"
#include <bits/basic_string.h>
#include <stdexcept>

Client::Client() : _fd(-1),  _request(new Request(this)) , _response(new Response(this)), _lastRequestTime(0)
{
}

Client::Client(int fd) : _fd(fd), _request(new Request(this)), _response(new Response(this)), _lastRequestTime(0)
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
        this->_request = new Request(this);
        //this->_response = src._response;
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
			// je check que je recois d'abord tous les headers et je les parse
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

				// j'ai la len du message alors j'attend que ce soit = ou > et je parse le body
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
				else
					Log::logVar(Log::ERROR, "Content-Length contains non-digit characters: ", content_length);
			}
		}
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
	this->_request-> // voir si le page est valide ou non
	// A ce moment la j'ai parsé toute la requete normalement COMPLETE!
    std::string method = _request->getMethod();
    std::string path = _request->getPath();
    std::string httpVersion = _request->getHttpVersion(); 
    std::string filePath = "docs" + path;
    std::ifstream file(filePath.c_str());
    if (file)
    {
        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Connection: close\r\n";
        response << "\r\n";
        response << file.rdbuf();
        std::string responseStr = response.str();
        send(_fd, responseStr.c_str(), responseStr.size(), 0);
    }
    else
    {
        std::ostringstream response;
        response << "HTTP/1.1 404 Not Found\r\n";
        response << "Content-Length: 0\r\n";
        response << "Connection: close\r\n";
        response << "\r\n";
       std::string responseStr = response.str();
        send(_fd, responseStr.c_str(), responseStr.size(), 0);
    }*/
    // TO IMPLEMENT THE RESPONSE CLASS THIS IS JUST A TEST
    //this->_response->generateResponse();
    //this->sendResponse(std::string(buffer)); 
}



/*void Client::sendResponse(const std::string &response) // to REDO
{
    //if (this->_response->buildResponse() == -1)
    send(this->_fd, response.c_str(), response.size(), 0);
}*/

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