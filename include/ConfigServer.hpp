/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigServer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ehamm <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 12:43:16 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/10 15:10:48 by ehamm            ###   ########.fr       */
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

        const std::string &getRoot() const { return this->_root; };
        const std::vector<std::string> &getIndex() const { return this->_index; };
        const std::map<int, std::string> &getErrorPages() const { return this->_errorPages; };
        const unsigned long long &getMaxBodySize() const { return _maxBodySize; };
        const std::vector<ConfigLocation> &getLocations() const { return this->_locations; };
		const std::string &getIp() const { return _ip; };
		const unsigned int &getPort() const { return _port; };
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