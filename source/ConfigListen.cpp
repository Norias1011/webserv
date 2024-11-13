/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigListen.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 13:57:40 by akinzeli          #+#    #+#             */
/*   Updated: 2024/11/13 15:27:21 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigListen.hpp"

ConfigListen::ConfigListen() : _ip(""), _port(0), _IpAndPort("")
{
}

ConfigListen::~ConfigListen()
{
    
}

ConfigListen::ConfigListen(std::string ipLine) : _ip(""), _port(0), _IpAndPort(ipLine)
{
    std::vector<std::string> ipPort = split(ipLine, ":");
    if (ipPort.size() == 1)
    {
        if (ipPort[0].empty())
        {
            std::cerr << "Error: Invalid ip or port" << std::endl; //throw une error ici
            throw std::runtime_error("[ERROR] Config Listen Error: Invalid ip or port");
        }
        if (ipPort[0].find(".") != std::string::npos)
        {
            _ip = ipPort[0];
            _port = 80;
        }
        else
        {
            _ip = "0.0.0.0";
            _port = std::atoi(ipPort[0].c_str());
        }
    }
    else if (ipPort.size() == 2)
    {
        if (ipPort[0].empty() || ipPort[1].empty())
        {
            std::cerr << "Error: Invalid ip or port" << std::endl; //throw une error ici
            throw std::runtime_error("[ERROR] Config Listen Error: Invalid ip or port");
        }
        _ip = ipPort[0];
        _port = std::atoi(ipPort[1].c_str());
    }
    else
    {
        std::cerr << "Error: Invalid ip or port" << std::endl; //throw une error ici
        throw std::runtime_error("[ERROR] Config Listen Error: Invalid ip or port");
    }
    _IpAndPort = _ip + ":" + NumberToString(_port); //::to_string(this->_port);
    if (this->_port > 65535 || this->_port < 1)
    {
        std::cerr << "Error: Invalid port" << std::endl; //throw une error ici
        throw std::runtime_error("[ERROR] Config Listen Error: Invalid port");
    }
    if (!goodIp())
    {
        std::cerr << "Error: Invalid ip" << std::endl; //throw une error ici
        throw std::runtime_error("[ERROR] Config Listen Error: Invalid ip");
    }
}

ConfigListen::ConfigListen(ConfigListen const &src)
{
    if (this != &src)
    {
        *this = src;
    }
}

ConfigListen &ConfigListen::operator=(ConfigListen const &src)
{
    if (this != &src)
    {
        this->_ip = src._ip;
        this->_port = src._port;
        this->_IpAndPort = src._IpAndPort;
    }
    return *this;
}

std::vector<std::string> ConfigListen::split(std::string &s, const std::string &delimiter)
{
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    tokens.push_back(s);
    return tokens;
}



bool ConfigListen::goodIp()
{
    if (this->_ip.empty())
        return false;
    int dots = 0;
    int len = this->_ip.length();
    int count = 0;
     
    // the number dots in the original 
    // string should be 3
    // for it to be valid
    for (int i = 0; i < len; i++)
        if (this->_ip[i] == '.')
            count++;
    if (count != 3)
        return false;
     
    // See following link for strtok()
   
    char *ptr = strtok((char *)this->_ip.c_str(), ".");
    if (ptr == NULL)
        return false;
 
    while (ptr) {
 
        /* after parsing string, it must be valid */
        if (valid_part(ptr)) 
        {
            /* parse remaining string */
            ptr = strtok(NULL, ".");
            if (ptr != NULL)
                ++dots;
        }
        else
            return false;
    }
 
    /* valid IP string must contain 3 dots */
    // this is for the cases such as 1...1 where 
    // originally the no. of dots is three but 
    // after iteration of the string we find 
    // it is not valid
    if (dots != 3)
        return false;
    return true;
}

bool ConfigListen::valid_part(char *s)
{
    int n = strlen(s);
    if (n > 3)
        return false;
 
    // check for each character
    // if it is digit or not
    for (int i = 0; i < n; i++)
        if ((s[i] >= '0' && s[i] <= '9'))
            continue;
        else
            return false;
    if (atoi(s) < 0 || atoi(s) > 255)
        return false;
    return true;
}
