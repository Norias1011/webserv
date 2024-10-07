#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "Client.hpp"

class Client;

class Request
{
    public:
        Request();
        ~Request();
        Request(Request const &copy);
        Request& operator=(Request const &src);
        
        int parseRequest(std::string const &request);

        Client* getClient() const;
        std::string getMethod() const;
        std::string getRawRequest() const;
        std::string getHttpVersion() const;
    
    private:
        Client* _client;
        std::string _method;
        std::string _rawRequest; //raw request before parsing
        std::string _httpVersion;

        parseHeader();
        parseBody();

        //to complete + parser a discuter avec Antho pr le parsing des requetes 
};

#endif //REQUEST_HPP