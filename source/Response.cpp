#include "Response.hpp"

Response::Response() :  _done(false) , _working(false), _request(NULL), _newFd(-1)
{
    _errorPage = ErrorPage();
}

Response::Response(Client* client) : _done(false) , _working(false), _request(client->getRequest()), _newFd(-1)
{
    _errorPage = ErrorPage();
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
        manageGetRequest();
    }
    else if (_request->getMethod() == "POST")
    {
        std::cout << "[DEBUG] - Response::giveAnswer - POST method" << std::endl;
        managePostRequest();
    }
    else if (_request->getMethod() == "PUT")
    {
        std::cout << "[DEBUG] - Response::giveAnswer - PUT method" << std::endl;
        managePutRequest();
    }
    else if (_request->getMethod() == "DELETE")
    {
        std::cout << "[DEBUG] - Response::giveAnswer - DELETE method" << std::endl;
        manageDeleteRequest();
    }
    else
    {
        std::cerr << "Error: Method not implemented" << std::endl;
        return -1;
    }
    return 0;
}

void Response::manageDeleteRequest()
{
    
    std::string body = "{\n";
    body += "\"method\": \"DELETE\",\n";
    body += "\"path\": \"" + _request->getPath() + "\",\n";
    body += "\"info\": \"File deleted\",\n";
    body += "}\n";
    _response = "HTTP/1.1 200 OK\r\n";
    _response += "Content-Type: application/html\r\n";
    _response += "Content-Length: " + numberToString(body.size()) +"\r\n";
    _response += "\r\n";
    _response += body;
}


void Response::managePutRequest()
{
    std::string body = "{\n";
    body += "\"method\": \"PUT\",\n";
    body += "\"path\": \"" + _request->getPath() + "\",\n";
    body += "\"info\": \"File uploaded\",\n";
    body += "}\n";
    _response = "HTTP/1.1 200 OK\r\n";
    _response += "Content-Type: application/html\r\n";
    _response += "Content-Length: " + numberToString(body.size()) +"\r\n";
    _response += "\r\n";
    _response += body;
}


void Response::managePostRequest()
{
    std::string body = "{\n";
    body += "\"method\": \"POST\",\n";
    body += "\"path\": \"" + _request->getPath() + "\",\n";
    body += "\"info\": \"File uploaded\",\n";
    body += "}\n";
    _response = "HTTP/1.1 200 OK\r\n";
    _response += "Content-Type: application/html\r\n";
    _response += "Content-Length: " + numberToString(body.size()) +"\r\n";
    _response += "\r\n";
    _response += body;
}

void Response::manageGetRequest()
{
    if (this->_request->getConfigLocation() != NULL)
    {
        std::cout << "[DEBUG] - Response::manageGetRequest - location found" << std::endl;
        handleLocation();
    }
    else
    {
        std::cout << "[DEBUG] - Response::manageGetRequest - location not found" << std::endl;
        handleRoot();
    }
}

void Response::handleRoot()
{
    std::string path = "";
    std::vector<std::string> FullServerPaths = getFullPathsServer();
    for (std::vector<std::string>::iterator it = FullServerPaths.begin(); it != FullServerPaths.end(); it++)
    {
        if (checkFileExist(*it))
        {
            path = *it;
            return ;
        }
    }
    if (path.empty())
    {
        std::string notFound = _request->getConfigServer()->getRoot();
        struct stat buffer;
        if (stat(notFound.c_str(), &buffer) == 0)
            _response = _errorPage.getConfigErrorPage(this->_request->getConfigServer()->getErrorPages(), 403);
        else
            _response = _errorPage.getConfigErrorPage(this->_request->getConfigServer()->getErrorPages(), 404);
        return ;
    }
    if (checkLargeFile(path))
        chunkedResponse(path);
    else
        normalResponse(path);
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
        std::string notFound = root + _request->getPath();
        struct stat buffer;
        if (stat(notFound.c_str(), &buffer) == 0)
            _response = _errorPage.getConfigErrorPage(this->_request->getConfigServer()->getErrorPages(), 403);
        else
            _response = _errorPage.getConfigErrorPage(this->_request->getConfigServer()->getErrorPages(), 404);
        return ;
    }
    if (checkLargeFile(path))
        chunkedResponse(path);
    else
        normalResponse(path);
}


void Response::normalResponse(std::string &path)
{
    std::cout << "[DEBUG] - Response::normalResponse - path: " << path << std::endl;
    std::ifstream file(path.c_str());
    if (file.is_open())
    {
        std::stringstream buffer;
        buffer << file.rdbuf();
        _response = "HTTP/1.1 200 OK\r\n";
        _response += "Content-Type: text/html\r\n";
        _response += "Content-Length: " + numberToString(buffer.str().size()) + "\r\n";
        _response += "\r\n";
        _response += buffer.str();
        file.close();
    }
    else
    {
        std::cerr << "Error: Unable to open file" << std::endl;
        struct stat buffer;
        if (stat(path.c_str(), &buffer) == 0)
            _response = _errorPage.getConfigErrorPage(this->_request->getConfigServer()->getErrorPages(), 403);
        else
            _response = _errorPage.getConfigErrorPage(this->_request->getConfigServer()->getErrorPages(), 404);
    }
}

void Response::chunkedResponse(std::string &path)
{
    if (_newFd == -1)
    {
        chunkedHeader(path);
    }

    char buf[1024];
    int ret = read(_newFd, buf, 1024);
    if (ret == -1)
    {
        std::cerr << "Error: Unable to read file" << std::endl; // throw error
        struct stat buffer;
        if (stat(path.c_str(), &buffer) == 0)
            _response = _errorPage.getConfigErrorPage(this->_request->getConfigServer()->getErrorPages(), 403);
        else
            _response = _errorPage.getConfigErrorPage(this->_request->getConfigServer()->getErrorPages(), 404);
    }
    else if (ret == 0)
    {
        close(_newFd);
        _newFd = -1;
        _response += "0\r\n\r\n";
    }
    else
    {
        std::cout << "[DEBUG] - Response::chunkedResponse - ret: " << ret << std::endl;
        std::string size = setIntToHexa(ret) + "\r\n";
        std::string chunk = std::string(buf, ret) + "\r\n";
        _response += size + chunk;
    }
}


void Response::chunkedHeader(std::string &path)
{
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0)
    {
        std::string header = "HTTP/1.1 200 OK\r\n";
        header += "Content-Type: text/html\r\n";
        header += "Transfer-Encoding: chunked\r\n";
        header += "\r\n";
        _response = header;
        _newFd = open(path.c_str(), O_RDONLY);
        if (_newFd == -1)
        {
            std::cerr << "Error: Unable to open file" << std::endl;
            struct stat buffer;
            if (stat(path.c_str(), &buffer) == 0)
                _response = _errorPage.getConfigErrorPage(this->_request->getConfigServer()->getErrorPages(), 403);
            else
                _response = _errorPage.getConfigErrorPage(this->_request->getConfigServer()->getErrorPages(), 404);
        }
    }
    else
    {
        std::cerr << "Error: Unable to open file" << std::endl;
        struct stat buffer;
        if (stat(path.c_str(), &buffer) == 0)
            _response = _errorPage.getConfigErrorPage(this->_request->getConfigServer()->getErrorPages(), 403);
        else
            _response = _errorPage.getConfigErrorPage(this->_request->getConfigServer()->getErrorPages(), 404);
    }
}


std::string Response::setIntToHexa(int number)
{
    std::string result;
    std::ostringstream convert;
    convert << std::hex << number;
    result = convert.str();
    return result;
}

std::string Response::FullResponse(std::string path, std::string root)
{
    pathClean(path);
    if (path[0] != '.')
        path.insert(0, ".");
    std::cout << "[DEBUG] - Response::FullResponse - path: " << path << std::endl;
    std::cout << "[DEBUG] - Response::FullResponse - root: " << root << std::endl;

    size_t i = 0;
    while (i < path.size() && i < root.size() && path[i] == root[i])
        i++;
    if (!(i == root.size()))
    {
        std::cerr << "Error: Path not in root" << std::endl;
        return _errorPage.getErrorPage(403);
    }
    std::vector<std::string> listFiles;
    DIR *dir = opendir(path.c_str());
    if (dir == NULL)
    {
        std::cerr << "Error: Unable to open directory" << std::endl;
        return _errorPage.getErrorPage(404);
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        listFiles.push_back(entry->d_name);
    }
    closedir(dir);
    std::string answer = responsePage(listFiles, path, root);
    return answer;
}


std::string Response::responsePage(std::vector<std::string> listFiles, std::string path, std::string root)
{
    (void)root;
    std::string answer = "<!DOCTYPE html> <html><head><title>Index of " + path + "</title></head><body>";
    answer += "<h1>Index of " + path + "</h1>";
    answer += "<ul>";
    for (std::vector<std::string>::iterator it = listFiles.begin(); it != listFiles.end(); it++)
    {
        if (*it == ".")
            continue;
        answer += "<li><a href=\"" + path + *it + "\">" + *it + "</a></li>";
    }
    answer += "</ul>";
    answer += "</body></html>";
    std::ostringstream answerSize;
    answerSize << answer.size();
    std::string answerSizeStr = answerSize.str();
    std::string header = "HTTP/1.1 200 OK\r\n";
    header += "Content-Type: text/html\r\n";
    header += "Content-Length: " + answerSizeStr + "\r\n";
    header += "\r\n";
    answer = header + answer;
    return answer;
}


void Response::pathClean(std::string &path)
{
    if (path[0] != '/')
        path = "/" + path;
    
    if (path[path.size() - 1] != '/')
        path += "/";

    size_t pos = path.find("/./");
    while (pos != std::string::npos)
    {
        path.erase(pos, 2);
        pos = path.find("/./");
    }
    while ((pos = path.find("/../")) != std::string::npos)
    {
        if (pos == 0)
        {
            path.erase(0, 3);
            continue;
        }
        size_t pos2 = path.rfind("/", pos - 1);
        if (pos2 != std::string::npos)
            path.erase(pos2, pos - pos2 + 3);
        else
            path.erase(0, pos + 3);
    }
    
}


bool Response::checkFileExist(std::string const &path)
{
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}


std::vector<std::string> Response::getFullPathsServer()
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

std::vector<std::string> Response::getFullPaths()
{
    std::string pathRequest = _request->getPath();
    std::string root = _request->getConfigLocation()->getRoot();
    std::vector<std::string> allIndex = _request->getConfigLocation()->getIndex();
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

bool Response::checkLargeFile(std::string const &path)
{
    struct stat buffer;
    if (stat(path.c_str(), &buffer) == 0)
    {
        if (buffer.st_size > 1000000)
            return true;
    }
    std::cout << "File is not large" << std::endl;
    return false;
}

std::string Response::numberToString(int number)
{
    std::string result;
    std::ostringstream convert;
    convert << number;
    result = convert.str();
    return result;
}
