/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 12:43:16 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/08 17:42:04 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGSERVER_HPP
#define CONFIGSERVER_HPP

#include "Config.hpp"
#include "ConfigLocation.hpp"
#include "ConfigListen.hpp"

class ConfigLocation;
class ConfigListen;

class ConfigServer
{
    public:
        ConfigServer();
        ~ConfigServer();
        ConfigServer(std::string filename);
        ConfigServer(ConfigServer const &src);
        ConfigServer &operator=(ConfigServer const &src);
        ConfigServer parseServer(std::ifstream &fileConfig);
        std::vector<std::string> split(std::string& s, const std::string& delimiter);
        bool checkServerLine(std::vector<std::string>& serverLine, std::string& line, std::ifstream &fileConfig);
        bool is_empty(std::ifstream& pFile);
        void addListen(std::string &ipLine);
        void checkDoubleInformation();
        void defaultValues();
        void checkDoubleLocation();
        void pathsClean();
        std::string getServerNames();
        const std::map<std::string, ConfigListen> &getListens() const { return this->_listens; };

        std::string getRoot() { return this->_root; };
        std::vector<std::string> getIndex() { return this->_index; };
        std::map<int, std::string> getErrorPages() { return this->_errorPages; };
        unsigned long long getMaxBodySize() { return this->_maxBodySize; };
        std::vector<ConfigLocation> getLocations() { return this->_locations; };
    protected:

    private:
        std::string _filename;
        std::vector<std::string> _serverNames;
        std::string _root;
        std::vector<std::string> _index;
        std::string _ip;
        unsigned int _port;
        std::vector<ConfigLocation> _locations;
        std::string _joinIpPort;
        std::vector<std::string> _IpPortTaken;
        std::map<int, std::string> _errorPages;
        unsigned long long _maxBodySize;
        std::map<std::string, int> _doubleInformation;
        std::map<std::string, ConfigListen> _listens;
};

#endif