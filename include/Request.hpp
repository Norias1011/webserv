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
        std::string getBody() const;
		std::string getHeaders(const std::string& headername);

        int parseRequest(std::string const &request);
        void parseMultipartFormData(std::string& body, const std::string& boundary);

		bool isHttpVersionValid(std::string const &version);
        std::string isMethod(std::string const &method);

		void printHeaders() const;
		void printPostHeaders() const;
    
    private:

        Client* _client;
		std::string _request;
		std::string _path;
        std::string _method;
        std::string _httpVersion;
		std::map<std::string, std::string> _headers;
        std::map<std::string, std::string> _postHeaders;
        std::map<std::string, std::map<std::string, std::string> > _fileHeaders;
		std::string _body;
        bool _isParsed;
};

#endif //REQUEST_HPP