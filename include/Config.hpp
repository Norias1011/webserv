/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 11:34:07 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/23 16:20:34 by akinzeli         ###   ########.fr       */
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
#include <exception>
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
        std::string numberToString(int number);

    	const std::map<std::string, std::vector<ConfigServer> >& getConfigServer() const { return _serverConfigs; }

        class ErrorException : public std::exception
		{
			private:
				std::string _error_message;
			public:
				ErrorException(std::string error_message) throw()
				{
					_error_message = "[ERROR] Config Parsing Error: " + error_message;
				}
				virtual const char *what() const throw()
				{
					return (_error_message.c_str());
				}
				virtual ~ErrorException() throw() {}
		};
    
	protected:


    private:
        std::string _filename;
        std::vector<ConfigServer> _servers;
        std::map<std::string, std::vector<ConfigServer> > _serverConfigs;
};


#endif