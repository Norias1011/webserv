/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 12:00:02 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/07 15:07:48 by akinzeli         ###   ########.fr       */
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

    if (!fileConfig.is_open())
    {
        // On peut mettre un throw ici si on fait un try catch dans le main
        std::cerr << "Error: Could not open file " << _filename << std::endl;
        exit(1);
    }
    while (getline(fileConfig, line))
    {
        int num_file = 0; // Pour connaitre le numero de la ligne dans le config file
        num_file++;
        if (line.empty() || line[0] == '#' || line.find_first_not_of(" \n\t\v\f\r") != std::string::npos)
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
    if (this->_servers.empty())
    {
        ConfigServer server(_filename);
        _servers.push_back(server.parseServer(fileConfig));
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
    for (size_t i = 0; i < this->_servers.size(); i++)
    {
        std::map<std::string, ConfigListen> listens = this->_servers[i].getListens();
        for (std::map<std::string, ConfigListen>::iterator it = listens.begin(); it != listens.end(); it++)
        {
            _serverConfigs[it->first].push_back(this->_servers[i]);
        }
    }
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
