/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 14:13:35 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/07 18:07:07 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigServer.hpp"

ConfigServer::ConfigServer() : _filename(""), _maxBodySize(1048576) // equal to 1MB
{
    _doubleInformation["root"] = 0;
    _doubleInformation["client_max_body_size"] = 0;
}

ConfigServer::~ConfigServer()
{
}

ConfigServer::ConfigServer(std::string filename) : _filename(filename), _maxBodySize(1048576)
{
    _doubleInformation["root"] = 0;
    _doubleInformation["client_max_body_size"] = 0;
}

ConfigServer::ConfigServer(ConfigServer const &src)
{
    if (this != &src)
    {
        *this = src;
    }
}

ConfigServer &ConfigServer::operator=(ConfigServer const &src)
{
    if (this != &src)
    {
        this->_filename = src._filename;
        this->_serverNames = src._serverNames;
        this->_root = src._root;
        this->_index = src._index;
        this->_ip = src._ip;
        this->_port = src._port;
        this->_locations = src._locations;
        this->_joinIpPort = src._joinIpPort;
        this->_errorPages = src._errorPages;
        this->_maxBodySize = src._maxBodySize;
    }
    return *this;
}

ConfigServer ConfigServer::parseServer(std::ifstream &fileConfig)
{
    std::string line;
    std::vector<std::string> serverLines;
    int index = 0;

    while (std::getline(fileConfig, line))
    {
        if (line.empty() || line[0] == '#' || line.find_first_not_of(" \n\t\v\f\r") != std::string::npos)
            continue;
        serverLines = split(line, " ");
        for (std::vector<std::string>::iterator it = serverLines.begin(); it != serverLines.end(); it++)
            (*it).erase(std::remove_if((*it).begin(), (*it).end(), isspace), (*it).end());
        if (serverLines[0] == "}" && serverLines[0].size() == 1 && serverLines.size() == 1)
        {
            index = 1;
            break;
        }
        else if (this->checkServerLine(serverLines, serverLines[0], fileConfig))
            continue;
        else
        {
            std::cout << "Error: Invalid server configuration" << std::endl; // on peut throw une erreur ici
            exit(1);
        }
    }
    if (index == 0 && is_empty(fileConfig))
    {
        std::cerr << "Error: Missing closing bracket for server" << std::endl; // on peut throw une erreur ici
        exit(1);
    }
    checkDoubleInformation();
    defaultValues();
    checkDoubleLocation();
    pathsClean();
    return *this;
}


void ConfigServer::pathsClean()
{
    for (size_t i = 0; i < this->_locations.size(); i++)
    {
        if (!this->_locations[i].getPath().empty() && this->_locations[i].getPath()[this->_locations[i].getPath().size() - 1] == '/')
            this->_locations[i].setPath(this->_locations[i].getPath().substr(0, this->_locations[i].getPath().size() - 1));
    }

    if (!this->_root.empty() && this->_root[this->_root.size() - 1] == '/')
        this->_root = this->_root.substr(0, this->_root.size() - 1);

    for (std::map<int, std::string>::iterator it = this->_errorPages.begin(); it != this->_errorPages.end(); it++)
    {
        if (!it->second.empty() && it->second[it->second.size() - 1] == '/')
            it->second = it->second.substr(0, it->second.size() - 1);
    }
    /*for (size_t i = 0; i < this->_locations.size(); i++)
    {
        if (this->_locations[i].getPath().back() == '/')
            this->_locations[i].setPath(this->_locations[i].getPath().substr(0, this->_locations[i].getPath().size() - 1));
    }

    if (this->_root.back() == '/')
        this->_root = this->_root.substr(0, this->_root.size() - 1);

    for (std::map<int, std::string>::iterator it = this->_errorPages.begin(); it != this->_errorPages.end(); it++)
    {
        if (it->second.back() == '/')
            it->second = it->second.substr(0, it->second.size() - 1);
    }*/
}

void ConfigServer::checkDoubleLocation()
{
    for (size_t i = 0; i < this->_locations.size(); i++)
    {
        for (size_t j = i + 1; j < this->_locations.size(); j++)
        {
            if (this->_locations[i].getPath() == this->_locations[j].getPath())
            {
                std::cerr << "Error: Duplicate location" << std::endl; //throw une error ici
                exit(1);
            }
        }
    }
}

void ConfigServer::defaultValues()
{
    if (this->_root.empty())
        this->_root = ".";
    if (this->_index.empty())
        this->_index.push_back("index.html");
    if (this->_listens.empty())
    {
        ConfigListen listen("0.0.0.0:80");
        this->_listens[listen.get_IpAndPort()] = listen;
    }
}


void ConfigServer::checkDoubleInformation()
{
    for (std::map<std::string, int>::iterator it = this->_doubleInformation.begin(); it != this->_doubleInformation.end(); it++)
    {
        if (it->second > 1)
        {
            std::cerr << "Error: Duplicate information in location" << std::endl; //throw une error ici
            exit(1);
        }
    }
}

std::vector<std::string> ConfigServer::split(std::string& s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);
    return tokens;
}

bool ConfigServer::is_empty(std::ifstream& pFile)
{
    return pFile.peek() == std::ifstream::traits_type::eof();
}

bool ConfigServer::checkServerLine(std::vector<std::string>& serverLine, std::string& line, std::ifstream &fileConfig)
{
    if (serverLine.size() < 2)
        return false;
    if ( serverLine.size() == 3 && serverLine[0] == "location" && serverLine[2] == "{")
    {
        ConfigLocation location(_filename); //ConfigLocation::parseLocation(fileConfig, serverLine[1]);
        this->_locations.push_back(location.parseLocation(fileConfig, serverLine[1]));
        return true;
    }
    else if (line == "listen" && serverLine.size() == 2)
    {
        addListen(serverLine[1]);
        return true;
    }
    else if (line == "server_name" && serverLine.size() == 2)
    {
        for (size_t i = 0; i < this->_serverNames.size(); i++)
        {
            if (this->_serverNames[i] == serverLine[1])
            {
                std::cerr << "Error: Duplicate server name" << std::endl; //throw une error ici
                exit(1);
            }
        }
        this->_serverNames.push_back(serverLine[1]);
        return true;
    }
    else if (line == "root" && serverLine.size() == 2)
    {
        this->_root = serverLine[1];
        this->_doubleInformation["root"]++;
        return true;
    }
    else if (line == "index" && serverLine.size() > 1)
    {
        for (size_t i = 1; i < serverLine.size(); i++)
        {
            if (std::find(this->_index.begin(), this->_index.end(), serverLine[i]) == this->_index.end())
                this->_index.push_back(serverLine[i]);
        }
        return true;
    }
    else if (line == "error_page" && serverLine.size() == 3)
    {
        if (std::atoi(serverLine[1].c_str()) < 400 || std::atoi(serverLine[1].c_str()) > 599)
        {
            std::cerr << "Error: Invalid status code for error_page" << std::endl; //throw une error ici
            exit(1);
        }
        this->_errorPages[std::atoi(serverLine[1].c_str())] = serverLine[2];
        return true;
    }
    else if (line == "client_max_body_size" && serverLine.size() == 2)
    {
        this->_maxBodySize = std::strtoull(serverLine[1].c_str(), NULL, 10);
        this->_doubleInformation["client_max_body_size"]++;
        return true;
    }
    else
        return false; 
}


void ConfigServer::addListen(std::string &ipLine)
{
    ConfigListen listen(ipLine);

    if (_listens.find(listen.get_IpAndPort()) != _listens.end())
    {
        std::cerr << "Error: Ip and port already taken" << std::endl; //throw une error ici
        exit(1);
    }
    _listens[listen.get_IpAndPort()] = listen;
}

std::string ConfigServer::getServerNames()
{
    std::string serverNames = "";
    for (size_t i = 0; i < this->_serverNames.size(); i++)
    {
        serverNames += this->_serverNames[i];
        if (i != this->_serverNames.size() - 1)
            serverNames += ", ";
    }
    return serverNames;
}
