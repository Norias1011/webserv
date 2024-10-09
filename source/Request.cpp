#include "../include/Request.hpp"

Request::Request() : _client(NULL), _request(""),  _path(""),_method(""), _httpVersion("")
{
}

Request::Request(Client* client): _client(client), _request(""),_path(""),_method(""), _httpVersion("")
{
}

Request::~Request()
{
}

Request::Request (Request const &copy)
{
	*this = copy;
}

std::string Request::getMethod() const { return _method; }
std::string Request::getRequest() const { return _request; }
std::string Request::getHttpVersion() const { return _httpVersion;}
std::string Request::getPath() const { return _path; }
std::map<std::string, std::string> Request::getHeaders() const { return _headers; }
std::string Request::getBody() const { return _body; }

Request &Request::operator=(Request const &src)
{
	if (this != &src)
	{
		this->_client = src._client;
		this->_request = src._request;
		this->_method = src._method;
		this->_path = src._path;
        this->_httpVersion = src._httpVersion;
	}
	return *this;
}

int Request::parseRequest(std::string const &raw_request) // ajouter la root serveR avec le Parsing de Antho - getRootpath?
{
	// NEED TO ADD THE POST REQUEST PARSED OPTION TODO
	_request = raw_request;
	std::string extracted = raw_request.substr(0,raw_request.find("\n"));
	if (extracted.empty())
		return -1;
	//ajouter le bloc d'upload dûn fichier, then on passe a la requete HTTP
	//si HTTP est absend on prend le path du fichier
	//we can have this as a first line parse yes 
	if (extracted.find("HTTP") == std::string::npos)
	{
		_method = "GET";
		//_path = "/upload" 
		return -1;
	}
	extracted.erase(std::remove(extracted.begin(), extracted.end(), '\r'), extracted.end());
	std::stringstream ss(extracted);
	std::string object;
	std::vector<std::string> objects;
	while (std::getline(ss, object, ' '))
	{
		objects.push_back(object);
	}
	if (objects.size() != 3)
	{
		std::cerr << "Error: invalid request" << std::endl;
		return -1;
	}
	_method = objects[0];
	_path = objects[1];
	_httpVersion = objects[2];
	std::cout << "[DEBUG] - method is: " <<  _method << std::endl;
	std::cout << "[DEBUG] - path is : " << _path << std::endl;
	std::cout << "[DEBUG] - version is : " << _httpVersion << std::endl;
	std::cout << "[DEBUG] - starting to parse the headers" << std::endl;
	// then header - start = fin de la premiere ligne deja parse et ensuite double saut = fin des headers
	//extraction sous forme de chaine + creation flux pour lire les headers
	size_t start = _request.find("\r\n") + 2; 
	std::cout << "[DEBUG] - start of the headers is : " <<  start << std::endl;
	size_t end = _request.find("\r\n\r\n", start);
	std::cout << "[DEBUG] - end of the headers is " <<  end << std::endl;
	std::string all_headers = _request.substr(start, end - start);
	std::cout << "[DEBUG] - headers are: " <<  all_headers << std::endl;
	std::stringstream headers_stream(all_headers);
	std::string header;
	while(std::getline(headers_stream, header, '\n'))
	{
		size_t pos = header.find(": ");
		if (pos == std::string::npos)
		{
			std::cerr << "Error: invalid header" << std::endl;
			return -1;
		}
		std::string key = header.substr(0, pos);
		std::string value = header.substr(pos + 1);
		_headers[key] = value;
	}
	// then the body
	size_t b_start = _request.find("\r\n\r\n");
	if(b_start != std::string::npos)
		_body = _request.substr(b_start + 4);


	//this->parseFirstLine(firstLine);
	//this->parseHeader();
	//this->parseBody();
	return 0;
}

/*void Request::parseFirstLine(std::string const &line)
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
}*/

bool Request::isHttpVersionValid(std::string const &version)
{
    return (version == "HTTP/1.1");
}