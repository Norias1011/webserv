#include "../include/Request.hpp"

Request::Request() : _client(NULL), _request(""),  _path(""),_method(""), _httpVersion(""), _isParsed(false)
{
}

Request::Request(Client* client): _client(client), _request(""),_path(""),_method(""), _httpVersion(""), _isParsed(false)
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
	}
	return *this;
}

int Request::parseRequest(std::string const &raw_request) // ajouter la root serveR avec le Parsing de Antho - getRootpath?
{
	if (this->_isParsed == true)
	{
		std::cerr << "Error: request already parsed" << std::endl;
		return -1;
	}
	// NEED TO ADD THE POST REQUEST PARSED OPTION TODO
	_request = raw_request;
	std::string extracted = raw_request.substr(0,raw_request.find("\n"));
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
		else // Handle non-multipart POST the GET does not have body ?
		{
			size_t b_start = _request.find("\r\n\r\n");
			if(b_start != std::string::npos)
			_body = _request.substr(b_start + 4);
		}
	}
	_isParsed = true;
	std::cout << "[DEBUG] - boy is: " <<  _body << std::endl;
	return 0;
}

bool Request::isHttpVersionValid(std::string const &version)
{
    return (version == "HTTP/1.1");
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

void Request::parseMultipartFormData(const std::string& body, const std::string& boundary) {
    size_t pos = 0;

    // Split the body by the boundary
    std::string delimiter = "--" + boundary;
    while ((pos = body.find(delimiter)) != std::string::npos) {
        std::string part = body.substr(0, pos);
        // Skip the first part, which is before the first boundary
        body.erase(0, pos + delimiter.length());
        // Ignore empty parts (in case of trailing newlines)
        if (part.empty()) continue;

        // Now, we need to parse the headers and content
        size_t header_end = part.find("\r\n\r\n");
        if (header_end == std::string::npos) continue; // Invalid part

        // Extract headers
        std::string headers = part.substr(0, header_end);
        std::string content = part.substr(header_end + 4); // Skip "\r\n\r\n"

        // Process headers
        std::istringstream header_stream(headers);
        std::string header_line;
        std::string content_disposition;
        std::string content_type;
        while (std::getline(header_stream, header_line)) {
            if (header_line.find("Content-Disposition:") != std::string::npos) {
                content_disposition = header_line;
            } else if (header_line.find("Content-Type:") != std::string::npos) {
                content_type = header_line;
            }
        }

        // Extract field name and filename
        std::string field_name;
        std::string filename;
        size_t name_start = content_disposition.find("name=\"") + 6;
        size_t name_end = content_disposition.find("\"", name_start);
        if (name_start != std::string::npos && name_end != std::string::npos) {
            field_name = content_disposition.substr(name_start, name_end - name_start);
        }

        size_t filename_start = content_disposition.find("filename=\"") + 10;
        size_t filename_end = content_disposition.find("\"", filename_start);
        if (filename_start != std::string::npos && filename_end != std::string::npos) {
            filename = content_disposition.substr(filename_start, filename_end - filename_start);
        }

        // Save the file if filename is provided
        if (!filename.empty()) {
            std::ofstream file("uploads/" + filename, std::ios::binary);
            file.write(content.data(), content.size());
            file.close();
            std::cout << "[DEBUG] File uploaded: " << filename << std::endl;
        } else {
            // Handle form field (text input)
            std::cout << "[DEBUG] Field: " << field_name << ", Value: " << content << std::endl;
        }
    }
}