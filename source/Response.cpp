#include "Response.hpp"

Response::Response() : _request(NULL) 
{
}

Response::Response(Client* client) : _request(client->getRequest())
{
}


Response::~Response()
{
}

int Response::giveAnswer()
{
    std::cout << "[DEBUG] - Response::giveAnswer" << std::endl;
    if (!_response.empty())
    {
        std::cout << "[DEBUG] - Response::giveAnswer - response is not empty" << std::endl;
        _response.clear();
    }
    if (_request->getMethod() == "GET")
    {
        std::cout << "[DEBUG] - Response::giveAnswer - GET method" << std::endl;
        //handleGetRequest();
    }
    else if (_request->getMethod() == "POST")
    {
        std::cout << "[DEBUG] - Response::giveAnswer - POST method" << std::endl;
        //handlePostRequest();
    }
    else if (_request->getMethod() == "PUT")
    {
        std::cout << "[DEBUG] - Response::giveAnswer - PUT method" << std::endl;
        //handlePutRequest();
    }
    else if (_request->getMethod() == "DELETE")
    {
        std::cout << "[DEBUG] - Response::giveAnswer - DELETE method" << std::endl;
        //handleDeleteRequest();
    }
    else
    {
        std::cerr << "Error: Method not implemented" << std::endl;
        return -1;
    }
    return 0;
}

void Response::manageGetRequest()
{
    if (this->_request->getConfigLocation() != NULL)
    {
        std::cout << "[DEBUG] - Response::manageGetRequest - location found" << std::endl;
        //handleLocation();
    }
    else
    {
        std::cout << "[DEBUG] - Response::manageGetRequest - location not found" << std::endl;
        //handleRoot();
    }
}


void Response::handleLocation()
{
    std::string root;
    std::string path = "";
    if (this->_request->getConfigLocation()->getRoot().empty())
        root = this->_request->getConfigServer()->getRoot();
    else
        root = this->_request->getConfigLocation()->getRoot();

    std::vector<std::string> FullPath = getFullPaths();
    for (std::vector<std::string>::iterator it = FullPath.begin(); it != FullPath.end(); it++)
    {
        if (checkFileExist(*it))
        {
            path = *it;
            break;
        }
    }
    if (path.empty())
    {
        if (this->_request->getConfigLocation()->getAutoindex() == true)
        {
            std::string newAlias = this->_request->getConfigLocation()->getAlias();
            if (!newAlias.empty())
            {
                std::string tmpPath = _request->getPath().substr(_request->getConfigLocation()->getPath().size());
                _response = FullResponse(newAlias + tmpPath, newAlias);
            }
            else
                _response = FullResponse(root + _request->getPath(), root);
            return ;
        }

    }
}

std::string Response::FullResponse(std::string path, std::string root)
{
    
}


bool Response::checkFileExist(std::string const &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

std::vector<std::string> Response::getFullPaths()
{
    std::string pathRequest = _request->getPath();
    std::string root = _request->getConfigServer()->getRoot();
    std::vector<std::string> allIndex = _request->getConfigServer()->getIndex();
    std::vector<std::string> FullPaths;

    if (pathRequest[pathRequest.size() - 1] == '/')
    {
        for (std::vector<std::string>::iterator it = allIndex.begin(); it != allIndex.end(); it++)
        {
            std::string index = *it;
            std::string tmpPath = pathRequest;
            if (pathRequest == "/")
                pathRequest = root + "/" + index;
            else
                pathRequest = root + pathRequest + index;
            FullPaths.push_back(pathRequest);
            pathRequest = tmpPath;
        }
    }
    else
        FullPaths.push_back(root + pathRequest);
    return FullPaths;
}