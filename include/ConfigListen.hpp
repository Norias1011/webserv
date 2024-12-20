/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigListen.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/07 13:51:07 by akinzeli          #+#    #+#             */
/*   Updated: 2024/11/11 16:36:19 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGLISTEN_HPP
#define CONFIGLISTEN_HPP

#include "Config.hpp"
#include <cstring>
#include <sstream>

class ConfigListen
{
    public:
        ConfigListen();
        ~ConfigListen();
        ConfigListen(ConfigListen const &src);
        ConfigListen &operator=(ConfigListen const &src);
        ConfigListen(std::string ipLine);
        std::vector<std::string> split(std::string& s, const std::string& delimiter);
        bool goodIp();
        bool valid_part(char *s);
		
        const std::string &get_IpAndPort() const { return _IpAndPort; };
        const unsigned int &get_Port() const { return _port; };
        const std::string &get_IP() const { return _ip; }; 
    private:
        std::string _ip;
        unsigned int _port;
        std::string _IpAndPort;
};

template <typename T>
  std::string NumberToString ( T Number )
  {
     std::ostringstream ss;
     ss << Number;
     return ss.str();
  }

#endif