/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 11:34:07 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/02 14:54:40 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG.HPP
#define CONFIG.HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include "ConfigServer.hpp"

class ConfigServer;

class Config
{
    public:
        Config();
        ~Config();
        Config(Config const &src);
        Config &operator=(Config const &src);
        void parseConfig(const std::string &filename);
        std::vector<std::string> Config::split(std::string& s, const std::string& delimiter);
    protected:


    private:
        ConfigServer _webServer;
        std::string _filename;
        std::vector<ConfigServer> _servers;
        std::map<std::string, std::vector<ConfigServer> > _serverConfigs;
        
};


#endif