#include "Client.hpp"

Client::Client() : _fd(-1)
{
}

Client::Client(int fd) : _fd(fd)
{    
}

Client::Client(const Client &copy)
{
    *this = copy;
}

Client::~Client()
{
    close(_fd);
}

Client &Client::operator=(Client const &src)
{
    if (this != &src)
    {
        this->_fd = src._fd;
        //this->_request = src._request;
        //this->_response = src._response;
    }
    return *this;
}

void Client::handleRequest()
{
    char buffer[CLIENT_BUFFER + 1];
    ssize_t bytes = recv(this->_fd, buffer, CLIENT_BUFFER , 0);
    if (bytes > 0)
    {
        buffer[bytes] = '\0';
        std::cout << "Received: " << buffer << std::endl;
        this->sendResponse(std::string(buffer));
    }
    else
    {
        throw EpollErrorExc();
    }
}

void Client::sendResponse(const std::string &response)
{
    send(this->_fd, response.c_str(), response.size(), 0);
}

const char* Client::EpollErrorExc::what() const throw()
{
    return "Epoll error occurred";
}