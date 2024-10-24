#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "Request.hpp"
#include "Client.hpp"
#include "ErrorPage.hpp"
#include <iostream>
#include <map>
#include <string>
#include <sys/stat.h>
#include <dirent.h>
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
        bool _done;
        bool _working;
        bool _break;
        
        Request* getRequest() const;
        int giveAnswer();
        bool checkRewrite();

        void manageGetRequest();
        void managePostRequest();
        void managePutRequest();
        void manageDeleteRequest();
        void handleLocation();
        void handleRoot();

        std::vector<std::string> getFullPaths();
        std::vector<std::string> getFullPathsServer();
        std::string getResponse() const { return _response; };

        bool checkFileExist(std::string const &path);
        std::string FullResponse(std::string path, std::string root);
        void pathClean(std::string &path);
        std::string responsePage(std::vector<std::string> listFiles, std::string path, std::string root);
        bool checkLargeFile(std::string const &path);
        void chunkedResponse(std::string &path);
        void normalResponse(std::string &path);
        std::string setIntToHexa(int number);
        void chunkedHeader(std::string &path);

        std::string numberToString(int number);

    private:

        Request* _request;
		std::string _response;
        ErrorPage _errorPage;
        int _newFd;

		//void handleGetRequest();
		//void handlePostRequest();
		//void handlePutRequest();
		//void handleDeleteRequest();
};

#endif //RESPONSE_HPP