#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Client.hpp"
#include <map>
#include <string>
#include <algorithm> // remove
#include <sstream> // stringstream

class Client;

class Request
{
    public:
		Request();
        Request(Client *client);
        ~Request();
        Request(Request const &copy);
        Request& operator=(Request const &src);
        
        int parseRequest(std::string const &request);

        //Client* getClient() const;

        std::string getMethod() const;
        std::string getRequest() const;
        std::string getHttpVersion() const;
        std::string getPath() const;
        std::map<std::string, std::string> getHeaders() const;
        std::string getBody() const;
        Client* getClient() const { return _client; };
        ConfigServer* getConfigServer() const { return _configServer; };
        ConfigLocation* getConfigLocation() const { return _configLocation; };
        time_t getLastRequestTime() const { return _lastRequestTime; };

        void timeoutChecker();

		void parseFirstLine(std::string const &line);
        void parseHeader();
        void parseBody();

		bool isHttpVersionValid(std::string const &version);
    
    private:

        Client* _client;
        ConfigServer* _configServer;
        ConfigLocation* _configLocation;
		std::string _request;
		std::string _path;
        std::string _method;
        std::string _httpVersion;
		std::map<std::string, std::string> _headers;
		std::string _body;
        bool _done;
        bool _working;
        time_t _lastRequestTime;
        int _serverCode;
};

#endif //REQUEST_HPP