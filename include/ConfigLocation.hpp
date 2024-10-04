/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLocation.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 12:43:11 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/04 16:16:54 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGLOCATION_HPP
#define CONFIGLOCATION_HPP

#include "Config.hpp"

class ConfigLocation
{
    public:
        ConfigLocation();
        ~ConfigLocation();
        ConfigLocation(ConfigLocation const &src);
        ConfigLocation (std::string filename);
        ConfigLocation &operator=(ConfigLocation const &src);
        ConfigLocation parseLocation(std::ifstream &fileConfig, std::string &option);
        std::vector<std::string> split(std::string& s, const std::string& delimiter);
        bool checkLocationLine(std::vector<std::string>& locationLine, std::string& line);
        void defaultValue();
        void checkDoubleInformation();
    protected:

    private:
        std::string _path;
        std::string _root;
        std::string _uploadPath;
        std::vector<std::string> _methods;
        std::vector<std::string> _index;
        std::map<std::string, std::string> _cgi;
        std::pair<int, std::string> _rewrite;
        std::string _alias;
        bool _autoindex;
        std::string _filename;
        std::vector<std::string> _allowedMethods;
        std::map<std::string, int> _doubleInformation;
};


#endif