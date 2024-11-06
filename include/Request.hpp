#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Client.hpp"
#include <map>
#include <string>
#include <algorithm> // remove
#include <sstream> // stringstream
#include <filesystem> 
#include <bits/fs_ops.h>

class Client;

class Request
{
    public:
		Request();
        Request(Client *client);
        ~Request();
        Request(Request const &copy);
        Request& operator=(Request const &src);

        //Client* getClient() const;

        std::string getMethod() const;
        std::string getRequest() const;
        std::string getHttpVersion() const;
        std::string getPath() const;
        std::map<std::string, std::string> getHeaders() const;
        std::string getHeaders(const std::string& headername);
        std::string getBody() const;
        Client* getClient() const { return _client; };
        const ConfigServer* getConfigServer() const { return _configServer; };
        const ConfigLocation* getConfigLocation() const { return _configLocation; };
        time_t getLastRequestTime() const { return _lastRequestTime; };
        int getServerCode() const { return _serverCode; };

		bool getConfigDone() const {return _configDone; };
		bool getFirstLineParsed() const {return _isFirstLineParsed; };
        bool getChunked() const {return _isChunked; };
		bool getMethodParsed() const {return _isMethodParsed;};
		bool getPathParsed() const {return _isPathParsed;};
		bool getHeadersParsed() const {return _isHeadersParsed;};
		bool getHttpParsed() const {return _isHttpParsed;};
        
        void timeoutChecker();

		void parseRequest(const std::string &raw_request);
		void parseFirstLine();
        void parseRequestHeaders();
		void parseBody();
        void parseMultipartFormData(const std::string& boundary);
        void parseChunkedBody();

        int checkConfig();
        int checkPath();
		int findConfigServer();
		int findConfigLocation();
		bool isHttpVersionValid(std::string const &version);
        std::string isMethod(std::string const &method);
        int isMethodAllowed();
        int checkSize();
        int checkUri();

        void setServerCode(int code) { _serverCode = code; };
        void setChunked(bool value) { _isChunked = value; };
		void setRequest(const std::string& request) {_request = request;}
		void setBody(const std::string& body) {_body = body;}
		
    private:

        Client* _client;
        const ConfigServer* _configServer;
        const ConfigLocation* _configLocation; // TODO checker la location avec le path
		std::string _request;
		std::string _path;
        std::string _uri;
        std::string _query;
        std::string _method;
        std::string _httpVersion;
		std::map<std::string, std::string> _headers;
        std::map<std::string, std::string> _postHeaders;
        std::map<std::string, std::map<std::string, std::string> > _fileHeaders;
		std::string _body;
        std::string _uploadedFilename;
        int _serverCode;
        bool _init;
        bool _working;
        bool _configDone;
		bool _isMethodParsed;
		bool _isHttpParsed;
		bool _isPathParsed;
		bool _isFirstLineParsed;
		bool _isHeadersParsed;
        bool _isBodyParsed;
		bool _isCGI;
        bool _isChunked;
        unsigned long long _contentLength;
        time_t _lastRequestTime;
};

bool fileExists(const std::string& path);
bool isDirectory(const std::string& path);

#endif //REQUEST_HPP