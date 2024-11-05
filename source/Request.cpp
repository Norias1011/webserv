#include "../include/Request.hpp"
#include <bits/basic_string.h>
#include <stdexcept> 

Request::Request() : _client(NULL), _request(""),  _path(""),_method(""), _httpVersion(""),_serverCode(200), _init(true), _working(false),_isMethodParsed(false),_isHttpParsed(false), _isPathParsed(false),_isFirstLineParsed(false),_isHeadersParsed(false),_isChunked(false),_lastRequestTime(0)
{
}

Request::Request(Client* client):_client(client), _request(""),  _path(""),_method(""), _httpVersion(""),_serverCode(200), _init(true), _working(false),_isMethodParsed(false),_isHttpParsed(false), _isPathParsed(false),_isFirstLineParsed(false),_isHeadersParsed(false),_isChunked(false),_lastRequestTime(0)
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
		this->_configLocation = src._configLocation;
		this->_configServer = src._configServer;
		this->_request = src._request;
		this->_method = src._method;
		this->_path = src._path;
        this->_httpVersion = src._httpVersion;
		this->_init = src._init;
		this->_working = src._working;
		this->_lastRequestTime = src._lastRequestTime;
		this->_serverCode = src._serverCode;
		this->_headers = src._headers;
		this->_body = src._body;
		this->_isChunked = src._isChunked;
		this->_isFirstLineParsed = src._isFirstLineParsed;
		this->_isHeadersParsed =src._isHeadersParsed;
		this->_isHttpParsed = src._isHttpParsed;
		this->_isMethodParsed = src._isMethodParsed;
		this->_isPathParsed = src._isPathParsed;
	}
	return *this;
}

void Request::parseRequest(const std::string &raw_request)
{
	this->_request += raw_request;

	Log::logVar(Log::DEBUG, "Parsing request {}", this->_request);
	this->parseFirstLine();
	if (this->getFirstLineParsed() == true)
		this->parseRequestHeaders();
	else
		Log::log(Log::DEBUG, "The First line is not properly parsed");
	if (this->getHeadersParsed() == true)
	{
		Log::log(Log::DEBUG, "The Headers are parsed");
		//this->_request->parseBody();
	}
	else
		Log::log(Log::DEBUG, "headers are not properly parsed");
}

void Request::parseFirstLine(void)
{
	if (this->_isFirstLineParsed == true)
		return;
	// METHOD PARSING AND CHECK
	int i = 0;
	int size = _request.size();
	bool flag = 0;
	while (i < size)
	{
		if (this->_request[i] == ' ')
		{
			flag = 1;
			break;
		}
		if (!std::isalpha(this->_request[i]))
			_serverCode = 400;
		this->_method += this->_request[i];
		i++;
	}
	_request.erase(0,flag? i + 1 : i);
	if (flag)
	{
		if (_method.empty())
		{
			_serverCode = 400;
			return;
		}
		if (isMethod(_method) == "WRONG METHOD")
		{
			_serverCode = 405;
			return;
		}
		Log::logVar(Log::DEBUG,"method is:", _method);
		_isMethodParsed = true;
	}
	// PATH PARSING AND CHECK
	i = 0;
	size = this->_request.size();
	flag = false;
	while (i < size)
	{
		if (this->_path.empty() && (this->_request[i] == ' ' || this->_request[i] == '\t'))
		{
			i++;
			continue;
		}
		if (this->_request[i] == ' ')
		{
			flag = true;
			break;
		}
		if (!std::isprint(this->_request[i]))
		{
			_serverCode = 400;
			return;
		}
		this->_path += this->_request[i];
		i++;
	}
	_request.erase(0,flag ? i + 1 : i);
	if (flag)
	{
		if (this->_path.empty())
		{
			_serverCode = 400;
			return;
		}
		//if (this->checkFile(_path))
		Log::logVar(Log::DEBUG,"path is:", _path);
		_isPathParsed = true;
	}
	// HTTP PARSING AND CHECK
	i = 0;
	size = this->_request.size();
	flag = false;
	while (i < size)
	{
		if (this->_httpVersion.empty() && (this->_request[i] == ' ' || this->_request[i] == '\t'))
		{
			i++;
			continue;
		}
		if (this->_request[i] != 'H' && this->_request[i] != 'T' && this->_request[i] != 'P' && this->_request[i] != '/' && this->_request[i] != '.' && !std::isdigit(this->_request[i]))
		{
			flag = true;
			break;
		}
		this->_httpVersion += this->_request[i];
		i++;
	}
	_request.erase(0,flag ? i + 1 : i);
	if (flag)
	{
		if (this->_httpVersion.empty())
		{
			_serverCode = 400;
			return;
		}
		if (!isHttpVersionValid(_httpVersion))
		{
			_serverCode = 505;
			return;
		}
		Log::logVar(Log::DEBUG,"http version is:", _httpVersion);
		_isHttpParsed = true;
	}
	if (_isHttpParsed == true && _isMethodParsed == true && _isPathParsed == true)
	{
		this->_request.erase(0, this->_request.find_first_not_of(" \t"));
		if (_request.empty())
			return;
		if (this->_request[0] == '\n')
		{
			this->_request.erase(0, 1);
			_isFirstLineParsed = true;
			return;
		}
		if (this->_request[0] == '\r')
		{
			if (this->_request.size() < 2)
				return ;
			if (this->_request[1] == '\n')
			{
				this->_request.erase(0, 2);
				_isFirstLineParsed = true;
				return ;
			}
			_serverCode = 400;
			return;
		}
		_serverCode = 400;
		return;
	}
}

void Request::parseRequestHeaders()
{
	if (this->_isHeadersParsed == true)
	{
		Log::log(Log::DEBUG,"Headers are already parsed");
		return;
	}
	size_t start = 0;
	size_t end = _request.find("\r\n\r\n", start);
	if (end == std::string::npos)
	{
		Log::log(Log::DEBUG,"End of the headers not found");
		_serverCode = 400;
		return;
	}
	std::string all_headers = _request.substr(start, end - start);
	std::stringstream headers_stream(all_headers);
	std::string header;
	while(std::getline(headers_stream, header, '\n'))
	{
		size_t pos = header.find(":");
		if (pos == std::string::npos)
		{
			Log::log(Log::ERROR, "Invalid header");
			_serverCode = 400;
			return;
		}
		std::string key = header.substr(0, pos);
		std::string value = header.substr(pos + 1);
		_headers[key] = value;
		std::cout << "[DEBUG] Parsed header: " << key << " = " << value << std::endl;
	}
	this->_request.erase(0 ,end + 4);
	_isHeadersParsed = true;
	Log::log(Log::DEBUG, "We found the end of the headers we can config");
	if (this->findConfigServer() == -1)
		return;
}

/*void Request::parseBody()
{
	if (_method == "POST")
	{
		if (_headers["Content-Type"].find("multipart/form-data") != std::string::npos) 
		{
			Log::logVar(Log::INFO,"Entering multipart/form-data - le body {}", _body );
			std::string boundary = "--" + _headers["Content-Type"].substr(_headers["Content-Type"].find("boundary=") + 9);
			boundary.erase(0,boundary.find_first_not_of("\t\n\r",1));
			boundary.erase(boundary.find_last_not_of(" \t\n\r") + 1);
			parseMultipartFormData(_body, boundary);
		}
		else if (_headers["Content-Type"].find("text/plain") != std::string::npos) 
		{
			Log::logVar(Log::INFO,"Entering text/plain- le body {}", _body );
			return;
		}
		else if (_headers["Transfer-encoding"].find("chunked") != std::string::npos)
		{
			while (true)
			{
				size_t old_size = _body.size();
				parseChunkedBody();
				if (_body.size() == old_size)
                    break;
			}
		}
	}
}*/

void Request::handleDelete()
{
	std::string path = this->_configLocation ? this->_configLocation->getRoot() + this->getPath() : this->_configServer->getRoot() + this->getPath();

    if (access(path.c_str(), F_OK) != -1)
    {
        if (remove(path.c_str()) == 0)
        {
            Log::logVar(Log::INFO, "File deleted: {}", path);
            _serverCode = 200; 
        }
        else
        {
            Log::logVar(Log::ERROR, "Failed to delete file: {}", path);
            _serverCode = 500; 
        }
    }
    else
    {
        Log::logVar(Log::ERROR, "File not found: {}", path);
        _serverCode = 404;
    }
}

bool Request::isHttpVersionValid(std::string const &version)
{
    return (version == "HTTP/1.1");
}

void Request::timeoutChecker()
{
	if (_lastRequestTime == 0 || _client->getRequestStatus() == true)
		return;
	time_t now = time(0);
	if (now > _lastRequestTime + 10)
	{
		_client->setRequestStatus(true);
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
				Log::logVar(Log::INFO, "Content-Disposition: {}",_postHeaders["Content-Disposition"]);
			} 
			else if (pos3 != std::string::npos) // pas besoin mais c'est parsed quand meme - to delete une fois sure
			{
				std::string key = "Content-Type";
				std::string value = header_line.substr(pos3 + std::string("Content-Type:").length());
				_postHeaders[key] = value;
				Log::logVar(Log::INFO, "Content-Type: {}",_postHeaders["Content-Type"]);
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
			//location path // pas de location a gerer
			//std::string file_path = this->_configLocation->getUploadPath() + "/ "+ filename ;
			//Log::logVar(Log::DEBUG, "upload path is : {}", this->_configLocation->getUploadPath()); 
			//mkdir(this->_configLocation->getUploadPath(), 0777);
			std::string file_path = "docs/uploads/" + filename;
			mkdir("docs/uploads/", 0777);
			std::ofstream file(file_path.c_str(), std::ios::binary);
			if (!file)
			{
				Log::logVar(Log::ERROR, " Failed to open file: {}", file_path);
				_serverCode = 500;
			}
			else 
			{
                file.write(content.data(), content.size());
                if (!file)
				{
					Log::logVar(Log::ERROR, " Failed to write to file: {}", file_path);
					_serverCode = 500;
				}
				else
				{
					Log::logVar(Log::INFO, " File uploaded on: {}", file_path);
					_serverCode = 200; // cest 201 je crois mais ca bug a cause de la reponse a voir
				}
            }
        } 
		else
		{
            Log::log(Log::DEBUG, "there is no file to upload");
			_serverCode = 400;
		}
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

void Request::parseChunkedBody()
{
	Log::log(Log::DEBUG, "Entering parsing chunk Body");
	if (_body.empty())
		_body = "";

	std::istringstream ss(_body);
	std::string chunk_size_str;
	std::string chunk;
	std::string full_chunk;

	while (std::getline(ss, chunk_size_str))
	{
		chunk_size_str.erase(std::remove(chunk_size_str.begin(), chunk_size_str.end(), '\r'), chunk_size_str.end());
		if (chunk_size_str.empty())
			continue;
		
		std::stringstream chunk_stream(chunk_size_str);
		size_t chunk_size;
		chunk_stream >> std::hex >> chunk_size;
        if (chunk_size == 0)
        {
            std::string trailing;
            std::getline(ss, trailing);
            break;
        }
		chunk.resize(chunk_size);
        ss.read(&chunk[0], chunk_size);
        full_chunk += chunk;

        std::string trailing;
        std::getline(ss, trailing);
	}
	_body = full_chunk;
}
int Request::findConfigServer() //should we check here the range of usable port - example the restricted one etc - to see with Antho si c'est deja check autre part?
{
	if(_configDone == true)
	{
		Log::log(Log::DEBUG, "Config server/locations already found");
        return -1;
	}
	std::string host = getHeaders("Host");
	if (host.empty())
	{
		Log::log(Log::ERROR, "Host is empty");
		_serverCode = 400;
		return -1;
	}
    const std::map<std::string, std::vector<ConfigServer> >& serverConfigs = this->_client->getServer()->getConfig().getConfigServer();
  	for (std::map<std::string, std::vector<ConfigServer> >::const_iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it)
    {
        const std::vector<ConfigServer> &servers = it->second;
        for (std::vector<ConfigServer>::const_iterator it2 = servers.begin(); it2 != servers.end(); ++it2)
        {
			Log::logVar(Log::DEBUG, "_configServer default: {}", _configServer);
			std::string host = getHeaders("Host");
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
			if (server_name_config == server_name && server_port_config == server_port && !server_name.empty() && !server_name_config.empty())
            {
				Log::log(Log::INFO, "Config server done with the match server \u2713");
                _configServer = &(*it2);
				_configServer->print();
				this->findConfigLocation();
                return (0);
            }
			else	
			{
				Log::log(Log::INFO, "Config server done with default server \u2713");
				_configServer = &(it2[0]); // to check  if it works
				_configServer->print();
				this->findConfigLocation();
				return (0);
			}
        }
    }
	return (0);
}

int Request::findConfigLocation() 
{
	if (_configServer == NULL)
	{
    	Log::log(Log::ERROR, "_configServer is NULL	");
		_serverCode = 500;
    	return -1;
	}
	Log::log(Log::DEBUG, "Je rentre dans find config location");
	const std::vector<ConfigLocation>& locations = this->_configServer->getLocations();

	for (std::vector<ConfigLocation>::const_iterator it = locations.begin(); it != locations.end(); ++it)
	{
		Log::logVar(Log::DEBUG, "le path dans la requete : {}", _path);
		Log::logVar(Log::DEBUG, "le path dans la config : {}", it->getPath());
		if (it->getPath() == _path)
		{
			this->_configLocation = &(*it);
			_configDone = true;
			_serverCode = 200;
			Log::log(Log::INFO, "Config location done with a match \u2713");
			//_configLocation->print(); //DEBUG
			return (0);
		}
		Log::log(Log::INFO, "No location block defined, using default behavior (the first one) \u2713");
		this->_configLocation = &(it[0]);
	}
	return (0);
}