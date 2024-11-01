/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLocation.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 14:13:32 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/31 14:05:54 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigLocation.hpp"

ConfigLocation::ConfigLocation() : _path(""), _root(""), _uploadPath(""), _methods(), _cgi(), _autoindex(false)
{
    this->_allowedMethods.push_back("GET");
    this->_allowedMethods.push_back("POST");
    this->_allowedMethods.push_back("DELETE");
    this->_allowedMethods.push_back("PUT");
    this->_doubleInformation["root"] = 0;
    this->_doubleInformation["upload_path"] = 0;
    this->_doubleInformation["alias"] = 0;
    this->_doubleInformation["autoindex"] = 0;
    this->_doubleInformation["allow_method"] = 0;
}

ConfigLocation::ConfigLocation(std::string filename) : _path(""), _root(""), _uploadPath(""), _methods(), _cgi(), _autoindex(false)
{
    this->_filename = filename;
    this->_allowedMethods.push_back("GET");
    this->_allowedMethods.push_back("POST");
    this->_allowedMethods.push_back("DELETE");
    this->_allowedMethods.push_back("PUT");
}

ConfigLocation::~ConfigLocation()
{
}

ConfigLocation::ConfigLocation(ConfigLocation const &src)
{
    if (this != &src)
    {
        *this = src;
    }
}

ConfigLocation &ConfigLocation::operator=(ConfigLocation const &src)
{
    if (this != &src)
    {
        this->_path = src._path;
        this->_root = src._root;
        this->_uploadPath = src._uploadPath;
        this->_methods = src._methods;
        this->_index = src._index;
        this->_cgi = src._cgi;
        this->_autoindex = src._autoindex;
        this->_rewrite = src._rewrite;
        this->_alias = src._alias;
        this->_filename = src._filename;
        this->_doubleInformation = src._doubleInformation;
        this->_allowedMethods = src._allowedMethods;
    }
    return *this;
}

ConfigLocation ConfigLocation::parseLocation(std::ifstream &fileConfig, std::string &option)
{
    std::string line;
    std::vector<std::string> locationLines;
    int index = 0;

    this->_path = option;
    while (std::getline(fileConfig, line))
    {
        /*if (line.empty() || line[0] == '#' || line.find_first_not_of(" \n\t\v\f\r") != std::string::npos)
            continue;
        if (line[line.length() - 1] != ';')
        {
            std::cerr << "Error: Missing ; at the end of the line : " << line << std::endl;
            exit(1);
        }
        else 
            line.erase(line.size() - 1);*/
        std::cout << "Line : " << line << "|" << std::endl;
        int i = 0;
        while (isspace(line[i]))
            line.erase(i, 1);
        if (line.empty() || line[0] == '#') // || line.find_first_not_of(" \n\t\v\f\r") != std::string::npos)
            continue;
        if (line[line.length() - 1] != ';' && line.find("}") == std::string::npos && line.find("{") == std::string::npos)
        {
            std::cout << "Line : " << line << "|" << std::endl;
            std::cerr << "Error: Missing ; at the end of the line : " << line << std::endl;
            throw std::runtime_error("[ERROR] Config Location Error: Missing ; at the end of the line");
        }
        else if (line[line.length() - 1] == ';')
            line.erase(line.size() - 1);
        std::cout << "Line : " << line << "|" << std::endl;
        locationLines = split(line, " ");
        for (std::vector<std::string>::iterator it = locationLines.begin(); it != locationLines.end(); it++)
        {
            std::cout << "Before erase : |" << *it << "|" << std::endl;
            (*it).erase(std::remove_if((*it).begin(), (*it).end(), isspace), (*it).end());
            std::cout << "After erase : |" << *it << "|" << std::endl;
        }
        if (locationLines[0] == "}" && locationLines.size() == 1)
        {
            index = 1;
            break;
        }
        else if (checkLocationLine(locationLines, locationLines[0]))
            continue;
        else
        {
            std::cerr << "Error: Invalid location line: " << line << std::endl; //throw une error ici 
            throw std::runtime_error("[ERROR] Config Location Error: Invalid location line");
        }
    }
    if (index == 0 && is_empty(fileConfig))
    {
        std::cerr << "Error: Missing closing bracket for location" << std::endl; //throw une error ici
        throw std::runtime_error("[ERROR] Config Location Error: Missing closing bracket for location");
    }
    checkDoubleInformation();
    defaultValue();
    return (*this);
}

void ConfigLocation::checkDoubleInformation()
{
    if (this->_root != "" && this->_alias != "")
    {
        std::cerr << "Error: Cannot have both root and alias in the same location" << std::endl; //throw une error ici
        throw std::runtime_error("[ERROR] Config Location Error: Cannot have both root and alias in the same location");
    }
    for (std::map<std::string, int>::iterator it = this->_doubleInformation.begin(); it != this->_doubleInformation.end(); it++)
    {
        if (it->second > 1)
        {
            std::cerr << "Error: Duplicate information in location" << std::endl; //throw une error ici
            throw std::runtime_error("[ERROR] Config Location Error: Duplicate information in location");
        }
    }
}

bool ConfigLocation::is_empty(std::ifstream& pFile)
{
    return pFile.peek() == std::ifstream::traits_type::eof();
}


void ConfigLocation::defaultValue()
{
    if (this->_uploadPath.empty())
        this->_uploadPath = "";
    if (this->_index.size() == 0)
        this->_index.push_back("index2.html");
    if (this->_alias.empty())
        this->_alias = "";
    if (this->_rewrite.second.empty())
        this->_rewrite.second = "";
    if (this->_methods.empty())
    {
        this->_methods.push_back("GET");
        this->_methods.push_back("POST");
        this->_methods.push_back("DELETE");
        this->_methods.push_back("PUT");
    }
}


std::vector<std::string> ConfigLocation::split(std::string& s, const std::string& delimiter) {
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


bool ConfigLocation::checkLocationLine(std::vector<std::string>& locationLine, std::string& line)
{
    if (locationLine.size() < 2)
        return false;
    if (line == "root" && locationLine.size() == 2)
    {
        this->_root = locationLine[1];
        this->_doubleInformation["root"]++;
        return true;
    }
    else if (line == "upload_path" && locationLine.size() == 2)
    {
        this->_uploadPath = locationLine[1];
        this->_doubleInformation["upload_path"]++;
        return true;
    }
    else if (line == "return" && locationLine.size() == 3)
    {
        this->_rewrite.first = std::atoi(locationLine[1].c_str());
        if (this->_rewrite.first < 300 || this->_rewrite.first > 399)
        {
            std::cerr << "Error: Invalid status code for rewrite" << std::endl; //throw une error ici
            throw std::runtime_error("[ERROR] Config Location Error: Invalid status code for rewrite");
        }
        this->_rewrite.second = locationLine[2];
        return true;
    }
    else if (line == "alias" && locationLine.size() == 2)
    {
        this->_alias = locationLine[1];
        this->_doubleInformation["alias"]++;
        return true;
    }
    else if (line == "autoindex" && locationLine.size() == 2)
    {
        if (locationLine[1] == "on")
            this->_autoindex = true;
        else if (locationLine[1] == "off")
            this->_autoindex = false;
        this->_doubleInformation["autoindex"]++;
        return true;
    }
    else if (line == "index" && locationLine.size() > 1)
    {
        for (size_t i = 1; i < locationLine.size(); i++)
        {
            if (std::find(this->_index.begin(), this->_index.end(), locationLine[i]) == this->_index.end())
                this->_index.push_back(locationLine[i]);
        }
        return true;
    }
    else if (line == "allow_methods" && locationLine.size() > 1)
    {
        for (size_t i = 1; i < locationLine.size(); i++)
        {
            if (std::find(this->_allowedMethods.begin(), this->_allowedMethods.end(), locationLine[i]) == this->_allowedMethods.end())
            {
                std::cerr << "Error: Invalid method in allow_methods" << std::endl; //throw une error ici
                throw std::runtime_error("[ERROR] Config Location Error: Invalid method in allow_methods");
            }
            this->_methods.push_back(locationLine[i]);
        }
        this->_doubleInformation["allow_method"]++;
        return true;
    }
    else if (line == "cgi_ext" && locationLine.size() == 3)
    {
        if (_cgi.find(locationLine[1]) != _cgi.end())
        {
            std::cerr << "Error: Duplicate cgi extension" << std::endl; //throw une error ici
            throw std::runtime_error("[ERROR] Config Location Error: Duplicate cgi extension");
        }
        this->_cgi[locationLine[1]] = locationLine[2];
        return true;
    }
    else if (line == "client_body_buffer_size") // FOR THE TESTER, TO DELETE
    {
        std::cout << "Client body buffer size" << std::endl;
        return true;
    }
    else if (line == "{")
        return true;
    else
    return false;
}

void ConfigLocation::print()
{
    Log::log(Log::DEBUG, "Config Location is as follows:");
    std::cout << "Path: " << this->_path << std::endl;
    std::cout << "Root: " << this->_root << std::endl;
    std::cout << "Upload Path: " << this->_uploadPath << std::endl;
    std::cout << "Alias: " << this->_alias << std::endl;
    std::cout << "Autoindex: " << this->_autoindex << std::endl;
    std::cout << "Rewrite: " << this->_rewrite.first << " " << this->_rewrite.second << std::endl;
    std::cout << "Index: ";
    for (std::vector<std::string>::iterator it = this->_index.begin(); it != this->_index.end(); it++)
        std::cout << *it << " ";
    std::cout << std::endl;
    std::cout << "Methods: ";
    for (std::vector<std::string>::iterator it = this->_methods.begin(); it != this->_methods.end(); it++)
        std::cout << *it << " ";
    std::cout << std::endl;
    std::cout << "CGI: ";
    for (std::map<std::string, std::string>::iterator it = this->_cgi.begin(); it != this->_cgi.end(); it++)
        std::cout << it->first << " " << it->second << " ";
    std::cout << std::endl;
}