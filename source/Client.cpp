#include "Client.hpp"

Client::Client() : _fd(-1),  _request(new Request(this)) //_response(NULL)
{
}

Client::Client(int fd) : _fd(fd), _request(new Request(this)) 
{    
}

Client::Client(const Client &copy)
{
    *this = copy;
}

Client::~Client()
{
	delete _request;
	_request = NULL;
    close(_fd);
}

Client &Client::operator=(Client const &src)
{
    if (this != &src)
    {
        this->_fd = src._fd;
        this->_request = src._request;
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
    std::cout << "[DEBUG] Handling request from client: " << this->_fd << std::endl;
    char buffer[CLIENT_BUFFER + 1];
    bzero(buffer, CLIENT_BUFFER + 1);
    int bytes = recv(this->_fd, buffer, CLIENT_BUFFER , 0);
    if (bytes > 0)
    {
        std::cout << "[DEBUG] number of bytes received from client: " << bytes << std::endl;
        buffer[bytes] = '\0';
    }
    else
    {
        throw DecoExc();
    }
    std::string request(buffer);
    std::cout << "[DEBUG] Beginning parsing of the request." << request << std::endl;
    if (this->_request->parseRequest(request) == -1)
	{
		std::cerr << "Request is wrong " << std::endl;
		close(_fd);
	}
    //this->_response->generateResponse();
    //this->sendResponse(std::string(buffer)); 
}

void Client::sendResponse(const std::string &response) // to REDO
{
    //if (this->_response->buildResponse() == -1)
    send(this->_fd, response.c_str(), response.size(), 0);
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