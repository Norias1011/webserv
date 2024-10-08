#include "../include/Request.hpp"

Request::Request() : _client(NULL), _rawRequest(""), _method(""), _httpVersion(""), _uri("")
{
}
Request::Request(Client* client): _client(client), _rawRequest(""), _method(""), _httpVersion(""), _uri("")
{
}

Request::~Request()
{
}
Request::Request (Request const &copy)
{
	*this = copy;
}

Request &Request::operator=(Request const &src)
{
	if (this != &src)
	{
		this->_client = src._client;
		this->_method = src._method;
		this->_uri = src._uri;
        this->_httpVersion = src._httpVersion;
	}
	return *this;
}

int Request::parseRequest(std::string const &request)
{
	size_t pos = request.find("\r\n");
	if (request.empty() || pos == std::string::npos) //si c'est vide ou ya pas de new line = error
		return -1;
	std::string firstLine = request.substr(0,pos);
	std::cout << firstLine << std::endl;
	this->parseFirstLine(firstLine);
	//this->parseHeader();
	//this->parseBody();
	return 0;
}

void Request::parseFirstLine(std::string const &line)
{
	std::cout << line << std::endl;
	size_t methodEnd = line.find(' ');
    if (methodEnd == std::string::npos) {
       	std::cerr << "Error: no method" << std::endl;
        return;
    }

    size_t uriEnd = line.find(' ', methodEnd + 1);
    if (uriEnd == std::string::npos) {
        std::cerr << "Error: no uri" << std::endl;
        return;
    }

	std::cout << _method << std::endl;
    _method = line.substr(0, methodEnd);
    _uri = line.substr(methodEnd + 1, uriEnd - methodEnd - 1);
    _httpVersion = line.substr(uriEnd + 1);

	std::cout << "[DEBUG] parsing method uri and http version:"<< std::endl;
	std::cout << "method:"<< _method << std::endl;
	std::cout << "uri:"<<_uri << std::endl;
	std::cout << "http version:"<<_httpVersion << std::endl;
}

bool Request::isHttpVersionValid(std::string const &version)
{
    return (version == "HTTP/1.1");
}