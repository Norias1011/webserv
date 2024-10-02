/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 12:00:02 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/02 14:11:41 by akinzeli         ###   ########.fr       */
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
}
