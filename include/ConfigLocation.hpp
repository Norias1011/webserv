/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLocation.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 12:43:11 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/28 16:14:44 by akinzeli         ###   ########.fr       */
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
        bool is_empty(std::ifstream& pFile);
        const std::string &getPath() const { return _path; };
        void setPath(const std::string &path) { _path = path; };
        void print();

        const std::string &getRoot() const { return this->_root; };
        const std::string &getUploadPath() const { return this->_uploadPath; };
        const std::string &getAlias() const { return this->_alias; };
        const bool &getAutoindex() const { return this->_autoindex; };
        const std::string &getFilename() const { return this->_filename; };
        const std::vector<std::string> &getIndex() const { return _index; };
        const std::map<std::string, std::string> &getCgi() const { return this->_cgi; };
        const std::pair<int, std::string> &getRewrite() const { return this->_rewrite; };
        const std::vector<std::string> &getMethods() const { return this->_methods; };
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