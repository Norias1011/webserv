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
    if (_request)
	    delete _request;
    if (_response)
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
    // TO IMPLEMENT THE RESPONSE CLASS THIS IS JUST A TEST
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
    }
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
        return ;
    }
    else
        std::cout << "Response sent" << std::endl;
    if (this->getResponse()->_done == true)
    {
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