/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InfoCgi.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 16:47:15 by akinzeli          #+#    #+#             */
/*   Updated: 2024/11/05 16:50:53 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/InfoCgi.hpp"

InfoCgi::InfoCgi(Request* request) : _request(request), _statusCgi(false), _path(""), _cgiPath("")
{
}

InfoCgi::InfoCgi() : _request(NULL), _statusCgi(false), _path(""), _cgiPath("")
{
}

InfoCgi::InfoCgi(InfoCgi const &src)
{
    if (this != &src)
    {
        *this = src;
    }
}

InfoCgi &InfoCgi::operator=(InfoCgi const &src)
{
    if (this != &src)
    {
        this->_request = src._request;
        this->_statusCgi = src._statusCgi;
        this->_path = src._path;
        this->_cgiPath = src._cgiPath;
    }
    return *this;
}

InfoCgi::~InfoCgi()
{
}

