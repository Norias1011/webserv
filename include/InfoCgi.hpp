/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InfoCgi.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 16:45:02 by akinzeli          #+#    #+#             */
/*   Updated: 2024/11/06 14:37:58 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INFOCGI_HPP
#define INFOCGI_HPP

#include "Config.hpp"

class Request;

class InfoCgi
{
    friend class Request;
    friend class CgiRun;
    public:
        InfoCgi();
        InfoCgi(Request* request);
        InfoCgi(InfoCgi const &src);
        InfoCgi &operator=(InfoCgi const &src);
        ~InfoCgi();

    private:
        Request* _request;
        bool _statusCgi;
        std::string _path;
        std::string _cgiPath;

};

#endif