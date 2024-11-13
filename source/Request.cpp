#include "../include/Request.hpp"
#include <bits/basic_string.h>
#include <stdexcept> 

Request::Request() : _client(NULL), _request(""),  _path(""),_uri(""), _query(""), _method(""), _httpVersion(""),_serverCode(200), _init(true), _working(false),_configDone(false),_isMethodParsed(false),_isHttpParsed(false), _isPathParsed(false),_isFirstLineParsed(false),_isHeadersParsed(false),_isBodyParsed(false),_isChunked(false),_bodySize(0), _chunkSize(-1),_contentLength(0), _infoCgi(this), _lastRequestTime(0)
{
}

Request::Request(Client* client):_client(client), _request(""),  _path(""), _uri(""), _query(""),_method(""), _httpVersion(""),_serverCode(200), _init(true), _working(false),_configDone(false),_isMethodParsed(false),_isHttpParsed(false), _isPathParsed(false),_isFirstLineParsed(false),_isHeadersParsed(false),_isBodyParsed(false),_isChunked(false),_bodySize(0), _chunkSize(-1),_contentLength(0),_infoCgi(this), _lastRequestTime(0)
{
	const std::map<std::string, std::vector<ConfigServer> >& serverConfigs = _client->getServer()->getConfig().getConfigServer();
    if (!serverConfigs.empty())
    {
        const std::vector<ConfigServer>& defaultServers = serverConfigs.begin()->second;
        if (!defaultServers.empty())
        {
            _configServer = &defaultServers[0];
            Log::log(Log::INFO, "Default config server set at Request in constructor\u2713");
        }
    }
	else
	{
		Log::log(Log::ERROR, "No server config found");
		_serverCode = 500;
		this->_client->setRequestStatus(true);
	}
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
		this->_uri = src._uri;
		this->_query = src._query;
        this->_httpVersion = src._httpVersion;
		this->_init = src._init;
		this->_working = src._working;
		this->_configDone = src._configDone;
		this->_lastRequestTime = src._lastRequestTime;
		this->_serverCode = src._serverCode;
		this->_headers = src._headers;
		this->_postHeaders = src._postHeaders;
		this->_fileHeaders = src._fileHeaders;
		this->_uploadedFilename = src._uploadedFilename;
		this->_body = src._body;
		this->_isChunked = src._isChunked;
		this->_isFirstLineParsed = src._isFirstLineParsed;
		this->_isHeadersParsed =src._isHeadersParsed;
		this->_isHttpParsed = src._isHttpParsed;
		this->_isMethodParsed = src._isMethodParsed;
		this->_isPathParsed = src._isPathParsed;
		this->_isBodyParsed = src._isBodyParsed;
		this->_contentLength = src._contentLength;
		this->_infoCgi = src._infoCgi;
	}
	return *this;
}

void Request::parseRequest(const std::string &raw_request)
{
	if (this->_client->getRequestStatus() == true)
	{
		Log::log(Log::DEBUG,"Request already handled");
		return;
	}
	if (raw_request.empty())
	{
		Log::log(Log::ERROR,"Empty request");
		return;
	}
	this->_request += raw_request;

	Log::logVar(Log::DEBUG, "Parsing request {}", this->_request);
	this->parseFirstLine();
	if (this->getFirstLineParsed() == true)
		this->parseRequestHeaders();
	else
		Log::log(Log::DEBUG, "The First line is not properly parsed");
	if (this->getHeadersParsed() == true)
	{
		Log::log(Log::DEBUG, "The Headers are parsed we can now parse the body");
		this->parseBody();
	}
	else
		Log::log(Log::DEBUG, "headers are not properly parsed");
	if (_isBodyParsed == true)
	{
		Log::log(Log::DEBUG, "The body is parsed");
		this->_client->setRequestStatus(true);
	}
	else
		Log::log(Log::DEBUG, "The body is not properly parsed");
	Log::logVar(Log::DEBUG, "Request is parsed ?: {}", this->_client->getRequestStatus());
}

void Request::parseFirstLine(void)
{
	if (this->_isFirstLineParsed == true)
		return;
	size_t pos = this->_request.find('\n');
	if (pos == std::string::npos)
	{
		Log::log(Log::DEBUG,"End of the first line not found");
		return;
	}
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
		{
			_serverCode = 400;
			Log::log(Log::ERROR,"Wrong Method");
			//this->_client->setRequestStatus(true);
		}
		this->_method += this->_request[i];
		i++;
	}
	_request.erase(0,flag? i + 1 : i);
	if (flag)
	{
		if (_method.empty())
		{
			_serverCode = 400;
			Log::log(Log::ERROR,"Method empty");
			this->_client->setRequestStatus(true);
			return;
		}
		if (isMethod(_method) == "WRONG METHOD")
		{
			_serverCode = 405;
			Log::log(Log::ERROR,"Wrong Method");
			this->_client->setRequestStatus(true);
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
		if (this->_uri.empty() && (this->_request[i] == ' ' || this->_request[i] == '\t'))
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
			Log::log(Log::ERROR,"Wrong URI");
			this->_client->setRequestStatus(true);
			return;
		}
		this->_uri += this->_request[i];
		i++;
	}
	_request.erase(0,flag ? i + 1 : i);
	if (flag)
	{
		if (this->_uri.empty())
		{
			_serverCode = 400;
			Log::log(Log::ERROR,"Empty URI");
			this->_client->setRequestStatus(true);
			return;
		}
		if (this->checkUri() == -1)
			return;
		Log::logVar(Log::DEBUG,"uri is:", _uri );
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
			Log::log(Log::ERROR,"Empty http version");
			this->_client->setRequestStatus(true);
			return;
		}
		if (!isHttpVersionValid(_httpVersion))
		{
			_serverCode = 505;
			Log::log(Log::ERROR,"Invalid HttpVersion");
			this->_client->setRequestStatus(true);
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
		//this->_client->setRequestStatus(true);
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
			this->_client->setRequestStatus(true);
			return;
		}
		std::string key = header.substr(0, pos);
		std::string value = header.substr(pos + 1);
		_headers[key] = value;
	}
	this->_request.erase(0 ,end + 4);
	if (this->checkConfig() == -1)
		return;
	else
	{
		Log::log(Log::DEBUG, "We found the end of the headers we can config");
		_isHeadersParsed = true;
	}
}

int Request::checkConfig()
{
	Log::log(Log::DEBUG, "Entering checkConfig");
	if (this->findConfigServer() == -1)
		return -1;
	if (this->isMethodAllowed() == -1)
		return -1;
	if (this->getHeaders("Transfer-Encoding").find("chunked")!= std::string::npos) // to check other TransferEncoding?
	{
		_isChunked = true;
		Log::log(Log::DEBUG, "Chunked body");
	}
	if (this->checkSize() == -1)
		return -1;
	Log::logVar(Log::DEBUG, "is checksize found 0 or -1? if 0 we continue : ", this->checkSize());
	if (this->findCGI() == 0)
		Log::log(Log::DEBUG, "CGI is found in the request");
	else
		Log::log(Log::DEBUG, "CGI is not found in the request");
	std::cout << "is CGI found ? " << this->findCGI() << std::endl;
	Log::logVar(Log::DEBUG, "is CGI found ? ", this->findCGI());
	/*if (this->_contentLength > this->checkConfig->getMaxBodySize())
	{
		Log::logVar(Log::ERROR, "Content-Length is too big: {}", this->_contentLength);
		_serverCode = 413;
		return (-1)
	}*/
	if (this->_client->getRequestStatus() == true)
		return -1;
	return 0;

}

void Request::parseBody()
{
	Log::log(Log::DEBUG, "Entering parseBody function");
	if (_isChunked == true)
	{
		parseChunkedBody();
	}
	if (_method == "POST") //et pas CGI
	{
		if (_headers["Content-Type"].find("multipart/form-data") != std::string::npos) 
		{
			std::string boundary = "--" + _headers["Content-Type"].substr(_headers["Content-Type"].find("boundary=") + 9);
			boundary.erase(0,boundary.find_first_not_of("\t\n\r",1));
			boundary.erase(boundary.find_last_not_of(" \t\n\r") + 1);
			parseMultipartFormData(boundary);
		}
		else if (_headers["Content-Type"].find("text/plain") != std::string::npos) 
		{
			Log::logVar(Log::INFO,"Entering text/plain- le body {}", _request );
			return;
		}
	}
	Log::logVar(Log::DEBUG, "request?: {}", this->_request);
	Log::logVar(Log::DEBUG, "Body is size: {}", this->_request.size());
	Log::logVar(Log::DEBUG, "content length: {}", this->_contentLength);
	if (_method == "GET" && this->_request.size() == 0)
	{
		_isBodyParsed = true;
		return;
	}
	else
		return;
	if (this->_request.size() == this->_contentLength)
	{
		_isBodyParsed = true;
		return;
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
	if (method == "")
		return "empty";
	return "WRONG METHOD";
}

void Request::parseMultipartFormData(const std::string& boundary) 
{
    size_t pos = 0;
	Log::log(Log::DEBUG," Entering into the parsing of the body for multipartform data ");
	Log::logVar(Log::INFO," boundary: {} ", boundary);
	Log::logVar(Log::INFO," body: {} ", _request);
	Log::logVar(Log::INFO," pos: {} ", _request.find(boundary));
    while ((pos = _request.find(boundary)) != std::string::npos) 
	{
        std::string part = _request.substr(0, pos);
        _request.erase(0, pos + boundary.length());
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
			std::string file_path = "docs/uploads/" + filename;
			mkdir("docs/uploads/", 0777);
			std::ofstream file(file_path.c_str(), std::ios::binary);
			if (!file)
			{
				Log::logVar(Log::ERROR, " Failed to open file: {}", file_path);
				this->_client->setRequestStatus(true);
				_serverCode = 403;
			}
			else 
			{
                file.write(content.data(), content.size());
                if (!file)
				{
					Log::logVar(Log::ERROR, " Failed to write to file: {}", file_path);
					this->_client->setRequestStatus(true);
					_serverCode = 403;
				}
				else
				{
					Log::logVar(Log::INFO, " File uploaded on: {}", file_path);
					this->_client->setRequestStatus(true);
					_serverCode = 200; // cest 201 je crois mais ca bug a cause de la reponse a voir
				}
            }
        } 
		else
		{
            Log::log(Log::DEBUG, "there is no file to upload");
			this->_client->setRequestStatus(true);
			_serverCode = 400;
			return;
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

void Request::parseChunkedBody()
{
	Log::log(Log::DEBUG, "Entering parseChunkedBody");
	int loop_count = 0 ;
	while (!this->_request.empty())
	{
      loop_count++;
		if (this->_chunkSize == -1)
		{
			size_t pos = this->_request.find("\r\n");
			if (pos == std::string::npos)
				return; 
			std::string line = this->_request.substr(0,pos);
			std::istringstream iss(line);
			if (!(iss >> std::hex >> this->_chunkSize))
			{
				Log::log(Log::ERROR, "Failed to parse chunk size");
				return;
			}
			this->_request.erase(0, pos + 2);
			if (this->_chunkSize == 0)
			{
				_isBodyParsed = true;
				return;
			}
			Log::logVar(Log::DEBUG, "Chunk size:", this->_chunkSize);
		}
		size_t pos = this->_request.find("\r\n");
		if (pos == std::string::npos)
			return ;
		if (pos != (size_t)this->_chunkSize)
		{
			Log::log(Log::ERROR, "Chunk size does not match");
			return;
		}

		std::ofstream tmpFile("tmp_file", std::ios::app);
		if (!tmpFile.is_open())
		{
			_serverCode = 500;
			Log::log(Log::ERROR, "Failed to open temporary file");
			this->_client->setRequestStatus(true);
		}
		tmpFile.write(this->_request.c_str(), this->_chunkSize);
		tmpFile.close();
		this->_request.erase(0, this->_chunkSize + 2);
		this->_chunkSize = -1;
		this->_bodySize += this->_chunkSize;
	}
}

int Request::findConfigServer() //should we check here the range of usable port - example the restricted one etc - to see with Antho si c'est deja check autre part?
{
	/*if(_configDone == true)
	{
		Log::log(Log::DEBUG, "Config server/locations already found");
        return -1;
	}*/
	std::string host = getHeaders("Host");
	if (host.empty())
	{
		Log::log(Log::ERROR, "Host is empty");
		_serverCode = 400;
		this->_client->setRequestStatus(true);
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
    	if (_method == "POST" && _path.size() >= 4 && _path.compare(_path.size() - 4, 4, ".bla") == 0)
		{
			this->_configLocation = &(locations[4]);
			_configDone = true;
			_serverCode = 200;
			this->_configLocation->print();
			Log::log(Log::INFO, "Config location done with a .bla match and POST method \u2713");
			return 0;
		}
		if (_path.compare(0, it->getPath().length(), it->getPath()) == 0 && (_path.length() == it->getPath().length() || _path[it->getPath().length()] == '/'))
		{
			this->_configLocation = &(*it);
			_configDone = true;
			_serverCode = 200;
			this->_configLocation->print();
			Log::log(Log::INFO, "Config location done with a match \u2713");
			return (0);
		}
		Log::log(Log::INFO, "No location block defined, using default behavior (the first one) \u2713");
		this->_configLocation = &(it[0]);
	}
	return (0);
}

int Request::isMethodAllowed()
{
	const std::vector<std::string>& allowedMethods = this->getConfigLocation()->getMethods();

    if (allowedMethods.empty())
    {
        Log::log(Log::INFO, "No methods defined in the location block, allowing all methods");
        return 0;
    }

    for (std::vector<std::string>::const_iterator it = allowedMethods.begin(); it != allowedMethods.end(); ++it)
    {
		Log::logVar(Log::DEBUG, "Method allowed: {}", *it);
        if (*it == _method)
        {
            Log::logVar(Log::INFO, "Method {} is allowed", _method);
            return 0;
        }
    }
    Log::logVar(Log::ERROR, "Method {} is not allowed", _method);
    _serverCode = 405;
	this->_client->setRequestStatus(true);
    return -1;

}

int Request::checkSize()
{
	std::string content_length_str = this->getHeaders("Content-Length");
	if (!content_length_str.empty())
	{
		std::istringstream ss(content_length_str);
		ss >> _contentLength;
		Log::logVar(Log::DEBUG, "Content-Length: {}", _contentLength);

		if (_contentLength > this->getConfigServer()->getMaxBodySize())
		{
			Log::logVar(Log::ERROR, "Content-Length is too big: {}", _contentLength);
			_serverCode = 413;
			this->_client->setRequestStatus(true);
			return -1;
		}
	}
	else
		Log::log(Log::DEBUG, "Content-Length header is missing");
	return 0;
}

int Request::checkUri()
{
	size_t pos = this->_uri.find("?");
	if (pos != std::string::npos)
	{
		_path = this->_uri.substr(0, pos);
		_query = this->_uri.substr(pos + 1);
	}
	else
		_path = this->_uri;
	return (0);
}

bool fileExists(const std::string& path) 
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

bool isDirectory(const std::string& path) 
{
	struct stat buffer;
	if (stat(path.c_str(), &buffer) == 0)
		return S_ISDIR(buffer.st_mode);
	return false;
}

int Request::findCGI()
{
	Log::log(Log::DEBUG, "Entering findCGI");
	if (this->_configLocation == NULL)
	{
		Log::log(Log::ERROR, "No location found for CGI");
		return -1;
	}
	std::vector<std::string> fullPathLocation = findFullPathLocation();
	for (size_t i = 0; i < fullPathLocation.size(); i++)
	{
		Log::logVar(Log::DEBUG, "Full path location: {}", fullPathLocation[i]);
		for (std::map<std::string, std::string>::const_iterator it = this->_configLocation->getCgi().begin(); it != this->_configLocation->getCgi().end(); it++)
		{
			Log::logVar(Log::DEBUG, "CGI path: {}", it->first);
			if (checkExtension(fullPathLocation[i]) == it->first)
			{
				if (checkfile(fullPathLocation[i]))
				{
					_infoCgi._statusCgi = true;
					_infoCgi._path = fullPathLocation[i];
					_infoCgi._cgiPath = it->second;
					return 0;
				}
			}
		}
	}
	Log::log(Log::DEBUG, "No CGI found");
	return -1;
}

std::vector<std::string> Request::findFullPathLocation()
{
    bool isAlias = false;
    std::vector<std::string> FullPaths;
    std::string pathRequest = this->_path;
    std::string root = this->_configLocation->getRoot();
    std::string alias = this->_configLocation->getAlias();
    std::vector<std::string> allIndex = this->_configLocation->getIndex();

    if (this->_configLocation == NULL)
        return std::vector<std::string>();
    if (root.empty())
        root = this->_configServer->getRoot();
    if (!alias.empty())
    {
        isAlias = true;
        root = alias;
    }
    if (pathRequest[pathRequest.size() - 1] != '/')
    {
        if (isAlias)
            pathRequest = pathRequest.substr(this->_configLocation->getPath().size());
        FullPaths.push_back(root + pathRequest);
		Log::logVar(Log::DEBUG, "Request::findFullPathLocation - root + pathRequest: {}", root + pathRequest);
    }
    for (size_t i = 0; i < allIndex.size(); i++)
    {
        std::string index = allIndex[i];
        std::string tmpPath = pathRequest;
        if (pathRequest == "/")
            pathRequest = root + "/" + index;
        else if (isAlias)
            pathRequest = root + "/" + index;
        else
            pathRequest = root + pathRequest + "/" + index;
        std::cout << "[DEBUG] - Request::findFullPathLocation - pathRequest: " << pathRequest << std::endl;
        FullPaths.push_back(pathRequest);
        pathRequest = tmpPath;
    }
    return FullPaths;
}

std::string Request::checkExtension(std::string const &path)
{
	size_t pos = path.find_last_of(".");
	std::cout << "[DEBUG] - Request::checkExtension - path: " << path.substr(pos + 1) << std::endl;
	if (pos == std::string::npos)
		return "";
	return ( "." + path.substr(pos + 1));
}

bool Request::checkfile(std::string const &path)
{
	struct stat buf;
	return (stat(path.c_str(), &buf) == 0 && S_ISREG(buf.st_mode));
}