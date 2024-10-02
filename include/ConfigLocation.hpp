/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLocation.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 12:43:11 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/02 13:35:31 by akinzeli         ###   ########.fr       */
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
        ConfigLocation &operator=(ConfigLocation const &src);

    protected:

    private:
        std::string _path;
        std::string _root;
        std::string _uploadPath;
        std::vector<std::string> _methods;
        std::vector<std::string> _index;
        std::map<std::string, std::string> _cgi;
        bool _autoindex;
};


#endif