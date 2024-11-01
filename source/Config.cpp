/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 12:00:02 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/31 14:26:22 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Config.hpp"

Config::Config() : _filename("")
{
}

Config::~Config()
{
}

Config::Config(Config const &src)
{
    _filename = src._filename;
    _servers = src._servers;
    _serverConfigs = src._serverConfigs;
}

Config &Config::operator=(Config const &src)
{
    if (this != &src)
    {
        this->_filename = src._filename;
        this->_servers = src._servers;
        this->_serverConfigs = src._serverConfigs;
    }
    return *this;
}

void Config::parseConfig(const std::string &filename)
{
    this->_filename = filename;
    std::ifstream fileConfig(_filename.c_str());
    std::string line;
    std::vector<std::string> serverLines;
    int num_file = 0;

    if (!fileConfig.is_open())
        throw Config::ErrorException("Could not open file " + _filename);
    while (getline(fileConfig, line))
    {
        //int num_file = 0; // Pour connaitre le numero de la ligne dans le config file
        num_file++;
        if (line.empty() || line[0] == '#')
            continue;
        serverLines = split(line, " "); // Split the line by space
        for (std::vector<std::string>::iterator it = serverLines.begin(); it != serverLines.end(); it++)
            (*it).erase(std::remove_if((*it).begin(), (*it).end(), isspace), (*it).end()); // Remove all isspace for every tokens
        if ((serverLines.size() == 2 && serverLines[0] == "server" && serverLines[1] == "{") || (serverLines.size() == 1 && serverLines[0] == "server{"))
        {
            ConfigServer server(_filename);
            _servers.push_back(server.parseServer(fileConfig));
        }
        else
            throw Config::ErrorException("Invalid syntax in config file at line " + numberToString(num_file));
    }
    checkDoubleServerName();
    configBlock();
    fileConfig.close();
}

std::vector<std::string> Config::split(std::string& s, const std::string& delimiter) {
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

void Config::configBlock()
{
    std::cout << "Number of servers: " << this->_servers.size() << std::endl;
    std::cout << "Number of listens: " << this->_servers[0].getListens().size() << std::endl;
    for (size_t i = 0; i < _servers.size(); i++)
    {
        std::map<std::string, ConfigListen> listens = _servers[i].getListens();
        std::cout << "Server " << i << " has " << listens.size() << " listens" << std::endl;
        for (std::map<std::string, ConfigListen>::iterator it = listens.begin(); it != listens.end(); it++)
        {
            std::cout << "Listen: " << it->first << std::endl;
            _serverConfigs[it->first].push_back(_servers[i]);
        }
    }
    std::cout << "Number of server configs: " << _serverConfigs.size() << std::endl;
}

void Config::checkDoubleServerName()
{
    for (size_t i = 0; i < _servers.size(); i++)
    {
        std::vector<std::string> serverNames = _servers[i].getNewServerNames();
        for (size_t j = i + 1; j < _servers.size(); j++)
        {
            if (_servers[j].checkDoubleServerName(serverNames))
                throw Config::ErrorException("Duplicate server name");
        }
    }
}


void Config::printAll()
{
    std::cout << "**************  Webserv data  *************" << std::endl;

    std::cout << "Filename: " << _filename << std::endl;
    int i = 1;

    std::cout << "Number of server configs: " << _serverConfigs.size() << std::endl;

    for (std::map<std::string, std::vector<ConfigServer> >::iterator it = _serverConfigs.begin(); it != _serverConfigs.end(); it++)
    {
        std::cout << "********** Data server " << i << " **********" << std::endl;
        std::cout << "IP and port of the Server: " << it->first << std::endl;
        for (size_t i = 0; i < it->second.size(); i++)
        {
            std::cout << "Server name: ";
            for (size_t j = 0; j < it->second[i].getServerNames().size(); j++)
            {
                std::cout << it->second[i].getServerNames()[j];
            }
            std::cout << std::endl;
            std::cout << "Root: " << it->second[i].getRoot() << std::endl;
            std::cout << "Index: ";
            for (size_t j = 0; j < it->second[i].getIndex().size(); j++)
            {
                std::cout << it->second[i].getIndex()[j] << " ";
            }
            std::cout << std::endl;
            std::cout << "Error pages: ";
            const std::map<int, std::string>& errorPages = it->second[i].getErrorPages();
            for (std::map<int, std::string>::const_iterator it2 = errorPages.begin(); it2 != errorPages.end(); ++it2)
            {
                std::cout << it2->first << " " << it2->second << " ";
            }
            std::cout << std::endl;
            std::cout << "Client max body size: " << it->second[i].getMaxBodySize() << std::endl;
            int k = 1;
            //std::cout << " ******* Locations: " << std::endl;
            for (size_t j = 0; j < it->second[i].getLocationss().size(); j++)
            {
                std::cout << " ******* Locations " << k << ": " << std::endl;
                std::cout << "Path: " << it->second[i].getLocationss()[j].getPath() << std::endl;
                std::cout << "Root: " << it->second[i].getLocationss()[j].getRoot() << std::endl;
                std::cout << "Upload path: " << it->second[i].getLocationss()[j].getUploadPath() << std::endl;
                std::cout << "Alias: " << it->second[i].getLocationss()[j].getAlias() << std::endl;
                std::cout << "Auto index: " << it->second[i].getLocationss()[j].getAutoindex() << std::endl;
                std::cout << "Filename: " << it->second[i].getLocationss()[j].getFilename() << std::endl;
                for (size_t k = 0; k < it->second[i].getLocationss()[j].getIndex().size(); k++)
                {
                    std::cout << "Index: " << it->second[i].getLocationss()[j].getIndex()[k] << std::endl;
                }
                const std::map<std::string, std::string>& cgiMap = it->second[i].getLocationss()[j].getCgi();
                for (std::map<std::string, std::string>::const_iterator it2 = cgiMap.begin(); it2 != cgiMap.end(); ++it2)
                {
                    std::cout << "Cgi: " << it2->first << " " << it2->second << std::endl;
                }
                for (size_t k = 0; k < it->second[i].getLocationss()[j].getMethods().size(); k++)
                {
                    std::cout << "Methods: " << it->second[i].getLocationss()[j].getMethods()[k] << std::endl;
                }
                std::cout << "Rewrite: " << it->second[i].getLocationss()[j].getRewrite().first << " " << it->second[i].getLocationss()[j].getRewrite().second << std::endl;
                k++;
            }
        }
        i++;
    }
}

std::string Config::numberToString(int number)
{
    std::string result;
    std::ostringstream convert;
    convert << number;
    result = convert.str();
    return result;
}
