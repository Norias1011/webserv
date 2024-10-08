/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 12:00:02 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/08 17:17:10 by akinzeli         ###   ########.fr       */
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
    if (this != &src)
    {
        *this = src;
    }
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
    {
        // On peut mettre un throw ici si on fait un try catch dans le main
        std::cerr << "Error: Could not open file " << _filename << std::endl;
        exit(1);
    }
    while (getline(fileConfig, line))
    {
        //int num_file = 0; // Pour connaitre le numero de la ligne dans le config file
        num_file++;
        if (line.empty() || line[0] == '#') // || line.find_first_not_of(" \n\t\v\f\r") != std::string::npos)
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
        {
            std::cerr << "Error: Invalid syntax in config file at line " << num_file << std::endl; // on peut throw une erreur ici
            exit(1);
        }
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
    for (size_t i = 0; i < this->_servers.size(); i++)
    {
        for (size_t j = 0; j < this->_servers[i].getServerNames().size(); j++)
        {
            for (size_t k = 0; k < this->_servers.size(); k++)
            {
                if (i != k)
                {
                    for (size_t l = 0; l < this->_servers[k].getServerNames().size(); l++)
                    {
                        if (this->_servers[i].getServerNames()[j] == this->_servers[k].getServerNames()[l])
                        {
                            std::cerr << "Error: Duplicate server name" << std::endl; //throw une error ici
                            exit(1);
                        }
                    }
                }
            }
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
                std::cout << it->second[i].getServerNames()[j] << " ";
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
            std::cout << " ******* Locations: " << std::endl;
            for (size_t j = 0; j < it->second[i].getLocations().size(); j++)
            {
                std::cout << "Path: " << it->second[i].getLocations()[j].getPath() << std::endl;
                std::cout << "Root: " << it->second[i].getLocations()[j].getRoot() << std::endl;
                std::cout << "Upload path: " << it->second[i].getLocations()[j].getUploadPath() << std::endl;
                std::cout << "Alias: " << it->second[i].getLocations()[j].getAlias() << std::endl;
                std::cout << "Auto index: " << it->second[i].getLocations()[j].getAutoindex() << std::endl;
                std::cout << "Filename: " << it->second[i].getLocations()[j].getFilename() << std::endl;
                for (size_t k = 0; k < it->second[i].getLocations()[j].getIndex().size(); k++)
                {
                    std::cout << "Index: " << it->second[i].getLocations()[j].getIndex()[k] << std::endl;
                }
                const std::map<std::string, std::string>& cgiMap = it->second[i].getLocations()[j].getCgi();
                for (std::map<std::string, std::string>::const_iterator it2 = cgiMap.begin(); it2 != cgiMap.end(); ++it2)
                {
                    std::cout << "Cgi: " << it2->first << " " << it2->second << std::endl;
                }
                for (size_t k = 0; k < it->second[i].getLocations()[j].getMethods().size(); k++)
                {
                    std::cout << "Methods: " << it->second[i].getLocations()[j].getMethods()[k] << std::endl;
                }
                std::cout << "Rewrite: " << it->second[i].getLocations()[j].getRewrite().first << " " << it->second[i].getLocations()[j].getRewrite().second << std::endl;
            }
        }
        i++;
    }
}
