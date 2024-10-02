/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 11:34:07 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/02 13:41:16 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG.HPP
#define CONFIG.HPP

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
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

    protected:


    private:
        ConfigServer _webServer;
        std::string _filename;
        std::vector<ConfigServer> _servers;
        std::map<std::string, std::vector<ConfigServer> > _serverConfigs;
        
};


#endif