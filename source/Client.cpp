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
	//sleep(1);
	std::string request;
	char buffer[CLIENT_BUFFER + 1];
	int bytes = 0;
	memset(buffer,0, CLIENT_BUFFER + 1);
	bytes = recv(this->_fd, buffer, CLIENT_BUFFER , 0);
	Log::logVar(Log::DEBUG,"bytes received {}.", bytes);

	if (bytes > 0)
	{
		buffer[bytes] = '\0';
		Log::logVar(Log::DEBUG,"request received {}.", request);
	}
	else if (bytes < 0)
		throw std::runtime_error("Error recv function");
	else if (bytes == 0)
	{
		if (request.empty())
			throw DecoExc();
	}

	request.append(buffer,bytes);
	this->_request->parseRequest(request);
	if (this->getRequestStatus() == true)
		changeEpoll(fd);
}

void Client::changeEpoll(int epollfd)
{
	epoll_event ev;
	ev.events = EPOLLOUT; // Reponse est prete a etre envoyé / Request finished
	ev.data.fd = this->_fd;
	epoll_ctl(epollfd, EPOLL_CTL_MOD, this->_fd, &ev);
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
	Log::logVar(Log::DEBUG, "sendResponse fd is :", _fd);
	Log::logVar(Log::DEBUG, "sendResponse response is :", _response->getResponse());
    if (_fd != -1)
        sendResponse = send(_fd, this->_response->getResponse().c_str(), _response->getResponse().size(), 0);
    if (sendResponse < 0)
    {
        throw std::runtime_error("Error: Unable to send response");
    }
    else
	{
        Log::logVar(Log::DEBUG, "Response sent to client with fd {}", _fd);
		Log::logVar(Log::DEBUG, "Response sent to client with sendResponse {}", sendResponse);
	}
    if (this->getResponse()->_done == true)
    {
		std::cout << "[DEBUG] - Response is done ... request and response are reseting.. " << std::endl;
       	if (this->getRequestStatus() != true)
            throw DecoExc();
        delete this->_request;
		this->_request = new Request(this);
		delete this->_response;	
        this->_response = new Response(this);
		this->setRequestStatus(false);
        epoll_event ev;
        ev.events = EPOLLIN; // Quand le client envoie une nouvelle requete
        ev.data.fd = this->_fd;
        epoll_ctl(fd, EPOLL_CTL_MOD, this->_fd, &ev);
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


/*if (this->_request->findCGI() == 0)
			Log::log(Log::DEBUG,"CGI is found in the request");
		else
			Log::log(Log::DEBUG,"CGI is not found in the request");*/