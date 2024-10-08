#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Client.hpp"

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

        Client* getClient() const;
        std::string getMethod() const;
        std::string getRawRequest() const;
        std::string getHttpVersion() const;

		void parseFirstLine(std::string const &line);
        void parseHeader();
        void parseBody();

		bool isHttpVersionValid(std::string const &version);
    
    private:

        Client* _client;
		std::string _rawRequest; //raw request before parsing
		std::string _uri;
        std::string _method;
        std::string _httpVersion;
		std::map<std::string, std::string> headers;
		std::string body;

        //to complete + parser a discuter avec Antho pr le parsing des requetes 
};

#endif //REQUEST_HPP