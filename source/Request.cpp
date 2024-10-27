#include "../include/Request.hpp"
#include <bits/basic_string.h>
#include <stdexcept> 

Request::Request() : _client(NULL), _request(""),  _path(""),_method(""), _httpVersion(""),_serverCode(200), _isParsed(false), _init(true), _working(false), _lastRequestTime(0)
{
}

Request::Request(Client* client): _client(client), _request(""),_path(""),_method(""), _httpVersion(""),_serverCode(200), _isParsed(false), _init(true), _working(false), _lastRequestTime(0)
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

int Request::parseRequestHeaders(std::string const &raw_headers) // ajouter la root server avec le parsing.
{
	_rawHeaders = raw_headers;
	std::string extracted = raw_headers.substr(0,raw_headers.find("\n"));
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
		Log::log(Log::ERROR, "Invalid request");
		_serverCode = 400;
		return -1;
	}
	_method = isMethod(objects[0]);
	if (_method == "WRONG METHOD")
	{
		Log::log(Log::ERROR, "Invalid method");
		_serverCode = 405;
		return -1;
	}
	_path = objects[1];
	if (!isHttpVersionValid(objects[2]))
	{
		Log::log(Log::ERROR, "Invalid http version");
		_serverCode = 505;
		return -1;
	}
	_httpVersion = objects[2];
	Log::logVar(Log::DEBUG, "method is:", _method);
	Log::logVar(Log::DEBUG, "path is :", _path);
	Log::logVar(Log::DEBUG, "version is", _httpVersion);
	size_t start = _rawHeaders.find("\r\n") + 2; 
	size_t end = _rawHeaders.find("\r\n\r\n", start);
	std::string all_headers = _rawHeaders.substr(start, end - start);
	std::stringstream headers_stream(all_headers);
	std::string header;
	while(std::getline(headers_stream, header, '\n'))
	{
		size_t pos = header.find(": ");
		if (pos == std::string::npos)
		{
			Log::log(Log::ERROR, "Invalid header");
			return -1;
		}
		std::string key = header.substr(0, pos);
		std::string value = header.substr(pos + 1);
		_headers[key] = value;
		 std::cout << "[DEBUG] Parsed header: " << key << " = " << value << std::endl;
	}
	return (0);
}

void Request::parseBody()
{
	if (_method == "POST")
	{
		if (_headers["Content-Type"].find("multipart/form-data") != std::string::npos) 
		{
			std::string boundary = "--" + _headers["Content-Type"].substr(_headers["Content-Type"].find("boundary=") + 9);
			boundary.erase(0,boundary.find_first_not_of("\t\n\r",1));
			boundary.erase(boundary.find_last_not_of(" \t\n\r") + 1);
			Log::logVar(Log::INFO,"Le boundary {}", boundary );
			Log::logVar(Log::INFO,"Le body {}", _body );
			parseMultipartFormData(_body, boundary);
		}
		//else if pour fichier python ou CGI ici a parse 
	}
	//printPostHeaders();
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

void Request::parseMultipartFormData(std::string& body, const std::string& boundary) 
{
    size_t pos = 0;
	Log::log(Log::DEBUG," Entering into the parsing of the body for multipartform data ");
	Log::logVar(Log::INFO," boundary: {} ", boundary);
	Log::logVar(Log::INFO," body: {} ", body);
	Log::logVar(Log::INFO," pos: {} ", body.find(boundary));
    while ((pos = body.find(boundary)) != std::string::npos) 
	{
        std::string part = body.substr(0, pos);
        body.erase(0, pos + boundary.length());
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
				Log::logVar(Log::DEBUG, "Content-Disposition: {}",_postHeaders["Content-Disposition"]);
			} 
			else if (pos3 != std::string::npos) // pas besoin mais c'est parsed quand meme - to delete une fois sure
			{
				std::string key = "Content-Type";
				std::string value = header_line.substr(pos3 + std::string("Content-Type:").length());
				_postHeaders[key] = value;
				Log::logVar(Log::DEBUG, "Content-Type: {}",_postHeaders["Content-Type"]);
			}
        }

        std::string name;
        std::string filename;
        size_t name_start = _postHeaders["Content-Disposition"].find("name=\"") + 6;
        size_t name_end = _postHeaders["Content-Disposition"].find("\"", name_start);// OSEF du name je pense a delete a la fin si vraiment pas besoin
        if (name_start != std::string::npos && name_end != std::string::npos) {
            name = _postHeaders["Content-Disposition"].substr(name_start, name_end - name_start);
        }

        size_t filename_start = _postHeaders["Content-Disposition"].find("filename=\"") + 10;
        size_t filename_end = _postHeaders["Content-Disposition"].find("\"", filename_start);
        if (filename_start != std::string::npos && filename_end != std::string::npos) {
            filename = _postHeaders["Content-Disposition"].substr(filename_start, filename_end - filename_start);
        }
		Log::logVar(Log::DEBUG, "name is : {}", name); 
		Log::logVar(Log::DEBUG, "filename is : {}", filename);
        if (!filename.empty()) 
		{
			//std::string file_path = ConfigLocation.getUploadPath()+ "/"+ name; TO ADD WITH PARSER le path
			std::string file_path = "docs/uploads/" + filename;
			std::ofstream file(file_path.c_str(), std::ios::binary);
			if (!file)
				Log::logVar(Log::ERROR, " Failed to open file: {}", file_path);
			else 
			{
                file.write(content.data(), content.size());
                if (!file) 
					Log::logVar(Log::ERROR, " Failed to write to file: {}", file_path);
				else 
					Log::logVar(Log::INFO, " File uploaded: {}", file_path);
            }
        } 
		else 
            Log::log(Log::DEBUG, "there is no file to upload");
    }
}

std::string Request::getHeaders(const std::string& headername)
{
	std::map<std::string,std::string> ::const_iterator it = _headers.find(headername);
	if (it != _headers.end())
		return it->second;
	return "";
}

void Request::printHeaders() const
{
	for (std::map<std::string, std::string >::const_iterator it = _headers.begin(); it != _headers.end(); it++)
	{
        std::cout << "[DEBUG]" << it->first << ": " << it->second << std::endl;
	}
}

void Request::printPostHeaders() const
{
	for (std::map<std::string, std::string >::const_iterator it = _postHeaders.begin(); it != _postHeaders.end(); it++)
	{
        std::cout << "[DEBUG]" << it->first << ": " << it->second << std::endl;
	}
}

void Request::findConfigServer() //should we check here the range of usable port - example the restricted one etc - to see with Antho si c'est deja check autre part?
{
	std::string host = getHeaders("Host");
	if (host.empty())
	{
		Log::log(Log::ERROR, "Host is empty");
		_serverCode = 400;
		return;
	}
	
    std::map<std::string, std::vector<ConfigServer> > serverConfigs = this->_client->getServer()->getConfig().getConfigServer();
  	for (std::map<std::string, std::vector<ConfigServer> >::iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it)
    {
        std::vector<ConfigServer> servers = it->second;
        for (std::vector<ConfigServer>::iterator it2 = servers.begin(); it2 != servers.end(); ++it2)
        {
         	std::string server_name = host.substr(0, host.find(":"));
            std::string port_str = host.substr(host.find(":") + 1);
			server_name.erase(std::remove_if(server_name.begin(), server_name.end(), ::isspace), server_name.end());
			port_str.erase(std::remove_if(port_str.begin(), port_str.end(), ::isspace), port_str.end());
			unsigned int server_port = 0;
			std::stringstream ss(port_str);
			ss >> server_port;

			Log::logVar(Log::DEBUG, "Server Name de la requete: {}", server_name);
			Log::logVar(Log::DEBUG, "Server Port la requete: {}", server_port);
			unsigned int server_port_config = it2->getPort();
			std::string server_name_config = it2->getServerNames();
            Log::logVar(Log::DEBUG, "Server Name de la config : {}", server_name_config);
			Log::logVar(Log::DEBUG, "Server Port de la config : {}", server_port_config);
			server_name_config.erase(std::remove_if(server_name_config.begin(), server_name_config.end(), ::isspace), server_name_config.end());
			if (server_name_config == server_name && server_port_config == server_port)
            {
				Log::log(Log::INFO, "Config server done \u2713");
                _configServer = &(*it2);
				_configServer->print(); //DEBUG
				this->findConfigLocation();
                return;
            }
        }
    }
}

void Request::findConfigLocation() 
{
	if (_configServer == NULL)
	{
    	Log::log(Log::ERROR, "_configServer is NULL	");
		_serverCode = 500;
    	return;
	}
	Log::log(Log::DEBUG, "Je rentre dans find config location");
	std::vector<ConfigLocation> locations = this->_configServer->getLocations();

	for (std::vector<ConfigLocation>::iterator it = locations.begin(); it != locations.end(); ++it)
	{
		Log::logVar(Log::DEBUG, "le path dans la requete : {}", _path);
		Log::logVar(Log::DEBUG, "le path dans la config : {}", it->getPath());
		if (it->getPath() == _path)
		{
			_configLocation = &(*it);
			Log::log(Log::INFO, "Config location done \u2713");
			_configLocation->print(); //DEBUG
			return;
		}
		Log::log(Log::ERROR, "No location found");
		_serverCode = 404;
	}
}