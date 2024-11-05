#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Client.hpp"
#include "InfoCgi.hpp"
#include <map>
#include <string>
#include <algorithm> // remove
#include <sstream> // stringstream
#include <filesystem> 
#include <bits/fs_ops.h>

class Client;

class Request
{
    friend class InfoCgi;
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
        std::string getBody() const;
        Client* getClient() const { return _client; };
        const ConfigServer* getConfigServer() const { return _configServer; };
        const ConfigLocation* getConfigLocation() const { return _configLocation; };
        time_t getLastRequestTime() const { return _lastRequestTime; };
        int getServerCode() const { return _serverCode; };
		bool getConfigDone() const {return _configDone; };
        bool getChunked() const {return _isChunked; };
        bool getCgiStatus() const {return _infoCgi._statusCgi; };
        void handleDelete();

        void timeoutChecker();
		std::string getHeaders(const std::string& headername);

        int parseRequestHeaders(std::string const &headers);
		void parseBody();
        void parseMultipartFormData(std::string& body, const std::string& boundary);
        void parseChunkedBody();

		void findConfigServer();
		void findConfigLocation();
		bool isHttpVersionValid(std::string const &version);
        std::string isMethod(std::string const &method);
        int findCGI();
        std::vector<std::string> findFullPathLocation();
        std::string checkExtension(std::string const &path);
        bool checkfile(std::string const &path);

        void setServerCode(int code) { _serverCode = code; };
        void setChunked(bool value) { _isChunked = value; };

		void printHeaders() const;
		void printPostHeaders() const;

		void setRequest(const std::string& request) {_request = request;}
		void setBody(const std::string& body) {_body = body;}
    
    private:

        Client* _client;
        const ConfigServer* _configServer;
        const ConfigLocation* _configLocation; // TODO checker la location avec le path
		std::string _request;
		std::string _rawHeaders;
		std::string _path;
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
        bool _isCGI;
        bool _isChunked;
        InfoCgi _infoCgi;
        time_t _lastRequestTime;
};

#endif //REQUEST_HPP