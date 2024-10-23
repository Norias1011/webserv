#include "../include/Request.hpp"

Request::Request() : _client(NULL), _request(""),  _path(""),_method(""), _httpVersion(""), _isParsed(false), _init(true), _working(false), _lastRequestTime(0)
{
}

Request::Request(Client* client): _client(client), _request(""),_path(""),_method(""), _httpVersion(""), _isParsed(false), _init(true), _working(false), _lastRequestTime(0)
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
		this->_isParsed = src._isParsed;
		this->_init = src._init;
		this->_working = src._working;
	}
	return *this;
}

int Request::parseRequest(std::string const &raw_request) // ajouter la root serveR avec le Parsing de Antho - getRootpath?
{
	if (this->_isParsed == true)
	{
		std::cerr << "Error: request already parsed" << std::endl;
		return 1;
	}
	if (raw_request.empty())
	{
		std::cerr << "Error: empty request" << std::endl;
		return -1;
	}
	// NEED TO ADD THE POST REQUEST PARSED OPTION TODO
	_request = raw_request;
	std::string extracted = raw_request.substr(0,raw_request.find("\n"));
	this->_lastRequestTime = time(0) + 10;
	if (extracted.empty())
		return -1;

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
		std::cerr << "[ERROR] invalid request" << std::endl;
		return -1;
	}
	_method = isMethod(objects[0]);
	if (_method == "WRONG METHOD")
	{
		std::cerr << "[ERROR]: invalid method" << std::endl;
		return -1;
	}
	_path = objects[1];
	if (!isHttpVersionValid(objects[2]))
	{
		std::cerr << "[ERROR]: invalid http version" << std::endl;
		return -1;
	}
	_httpVersion = objects[2];
	std::cout << "[DEBUG] - method is: " <<  _method << std::endl;
	std::cout << "[DEBUG] - path is : " << _path << std::endl;
	std::cout << "[DEBUG] - version is : " << _httpVersion << std::endl;
	std::cout << "[DEBUG] - starting to parse the headers" << std::endl;

	size_t start = _request.find("\r\n") + 2; 
	size_t end = _request.find("\r\n\r\n", start);
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
	std::cout << "[DEBUG] - headers are: " <<  all_headers << std::endl;
	
	if (_method == "POST")
	{
		if (_headers["Content-Type"].find("multipart/form-data") != std::string::npos) 
		{
            std::string boundary = "--" + _headers["Content-Type"].substr(_headers["Content-Type"].find("boundary=") + 9);
            size_t b_start = _request.find("\r\n\r\n") + 4;
            _body = _request.substr(b_start);
            parseMultipartFormData(_body, boundary);
            std::cout << "[DEBUG] - body is: " << _body << std::endl;
		}
		else // Handle non-multipart POST
		{
			size_t b_start = _request.find("\r\n\r\n");
			if(b_start != std::string::npos)
			_body = _request.substr(b_start + 4);
		}
	}
	_isParsed = true;
	//PARSER LE BODY
	std::cout << "[DEBUG] - body is: " <<  _body << std::endl;
	return 0;
}

bool Request::isHttpVersionValid(std::string const &version)
{
    return (version == "HTTP/1.1");
}

void Request::timeoutChecker()
{
	if (_lastRequestTime == 0 || _isParsed == true)
		return;
	time_t now = time(0);
	if (now > _lastRequestTime + 10)
	{
		_isParsed = true;
		std::cout << "Timeout" << std::endl;
		_serverCode = 408;
	}
}

std::string Request::isMethod(std::string const &method)
{
	if (method == "GET")
		return "GET";
	if (method == "POST")
		return "POST";
	if (method == "DELETE")
		return "DELETE";
	return "WRONG METHOD";
}

void Request::parseMultipartFormData(std::string& body, const std::string& boundary) {
    size_t pos = 0;

    std::string delimiter = "--" + boundary;
    while ((pos = body.find(delimiter)) != std::string::npos) 
	{
        std::string part = body.substr(0, pos);
        body.erase(0, pos + delimiter.length());
        if (part.empty() || part == "\r\n") continue;

        size_t header_end = part.find("\r\n\r\n");
        if (header_end == std::string::npos) continue; 

        std::string headers = part.substr(0, header_end);
        std::string content = part.substr(header_end + 4);

		 _postHeaders.clear();
        std::istringstream header_stream(headers);
        std::string header_line;
        while (std::getline(header_stream, header_line))
		{
			size_t pos2 = header_line.find("Content-Disposition:");
			size_t pos3 = header_line.find("Content-Type:");

			if (pos2 != std::string::npos) 
			{
				std::string key = "Content-Disposition";
				std::string value = header_line.substr(pos2 + std::string("Content-Disposition:").length());
				_postHeaders[key] = value;
			} 
			else if (pos3 != std::string::npos) 
			{
				std::string key = "Content-Type";
				std::string value = header_line.substr(pos3 + std::string("Content-Type:").length());
				_postHeaders[key] = value;
			}
        }

		// dans les header de post il y a les nom et le file name a recuperer
        std::string field_name;
        std::string file_name;
        size_t name_start = _postHeaders["Content-Disposition"].find("name=\"") + 6;
        size_t name_end = _postHeaders["Content-Disposition"].find("\"", name_start);
        if (name_start != std::string::npos && name_end != std::string::npos) {
            field_name = _postHeaders["Content-Disposition"].substr(name_start, name_end - name_start);
        }

        size_t filename_start = _postHeaders["Content-Disposition"].find("filename=\"") + 10;
        size_t filename_end = _postHeaders["Content-Disposition"].find("\"", filename_start);
        if (filename_start != std::string::npos && filename_end != std::string::npos) {
            file_name = _postHeaders["Content-Disposition"].substr(filename_start, filename_end - filename_start);
        }
		std::cout << "[DEBUG] - file name is: " <<  file_name << std::endl;
		std::cout << "[DEBUG] - field name is: " <<  field_name << std::endl;
        // Save the file if filename is provided meaning there is a file to upload
        if (!file_name.empty()) 
		{
			std::string file_path = "uploads/" + file_name;
			std::ofstream file(file_path.c_str(), std::ios::binary);
			if (!file) 
                std::cerr << "[ERROR] Failed to open file: " << file_path << std::endl;
			else 
			{
                file.write(content.data(), content.size());
                if (!file) 
                    std::cerr << "[ERROR] Failed to write to file: " << file_path << std::endl;
				else 
                    std::cout << "[DEBUG] File uploaded: " << file_name << std::endl;
            }
        } 
		else 
		{
            std::cout << "[DEBUG] Field: " << field_name << ", Value: " << content << std::endl;
        }
    }
}