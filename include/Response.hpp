#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include <map>
#include <string>
#include <sys/stat.h>
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
        int giveAnswer();

        void manageGetRequest();
        void managePostRequest();
        void managePutRequest();
        void manageDeleteRequest();
        void handleLocation();

        std::vector<std::string> getFullPaths();

        bool checkFileExist(std::string const &path);
        std::string FullResponse(std::string path, std::string root);


    private:

        Request* _request;
		std::string _response;

		//void handleGetRequest();
		//void handlePostRequest();
		//void handlePutRequest();
		//void handleDeleteRequest();
};

#endif //RESPONSE_HPP