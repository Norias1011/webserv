/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ehamm <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 11:34:07 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/10 14:02:34 by ehamm            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <map>
#include <algorithm>
#include "ConfigServer.hpp"
#include "ConfigLocation.hpp"
#include "ConfigListen.hpp"
#include <cstdlib>

class ConfigServer;

class Config
{
    public:
        Config();
        ~Config();
        Config(Config const &src);
        Config &operator=(Config const &src);
        void parseConfig(const std::string &filename);
        std::vector<std::string> split(std::string& s, const std::string& delimiter);
        void checkDoubleServerName();
        void configBlock();
        void printAll();

    	std::map<std::string, std::vector<ConfigServer> > getConfigServer() { return _serverConfigs; }
    
	protected:


    private:
        std::string _filename;
        std::vector<ConfigServer> _servers;
        std::map<std::string, std::vector<ConfigServer> > _serverConfigs;
};


#endif