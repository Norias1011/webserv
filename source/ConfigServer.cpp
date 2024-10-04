/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 14:13:35 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/04 13:18:32 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigServer.hpp"

ConfigServer::ConfigServer() : _filename(""), _maxBodySize(1048576) // equal to 1MB
{
}

ConfigServer::~ConfigServer()
{
}

ConfigServer::ConfigServer(std::string filename) : _filename(filename), _maxBodySize(1048576)
{
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
        std::vector<std::string> ipPort = split(serverLine[1], ":");
        if (ipPort.size() == 2)
        {
            this->_ip = ipPort[0];
            this->_port = std::stoi(ipPort[1]);
            this->_joinIpPort = serverLine[1];
            return true;
        }
    }
    else if (line == "server_name" && serverLine.size() == 2)
    {
        this->_serverNames.push_back(serverLine[1]);
        return true;
    }
    else if (line == "root" && serverLine.size() == 2)
    {
        this->_root = serverLine[1];
        return true;
    }
    else if (line == "index" && serverLine.size() > 1)
    {
        for (size_t i = 1; i < serverLine.size(); i++)
            this->_index.push_back(serverLine[i]);
        return true;
    }
    else if (line == "error_page" && serverLine.size() == 3)
    {
        this->_errorPages[std::stoi(serverLine[1])] = serverLine[2];
        return true;
    }
    else if (line == "client_max_body_size" && serverLine.size() == 2)
    {
        this->_maxBodySize = std::stoull(serverLine[1]);
        return true;
    }
    else
        return false; 
}

