#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include <map>
#include <string>
#include <algorithm> // remove
#include <sstream> // stringstream

class Request;
class Client;

class Response
{
    public:
		Response();
        Response(Client *client);
        ~Response();
        Response(Response const &copy);
        Response& operator=(Response const &src);
        

        Request* getRequest() const;


    private:

        Request* _request;
		std::string _response;

		//void handleGetRequest();
		//void handlePostRequest();
		//void handlePutRequest();
		//void handleDeleteRequest();
};

#endif //RESPONSE_HPP