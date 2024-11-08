/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRun.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 14:18:40 by akinzeli          #+#    #+#             */
/*   Updated: 2024/11/06 17:09:16 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIRUN_HPP
#define CGIRUN_HPP

#include "Config.hpp"
#include "InfoCgi.hpp"
#include "Request.hpp"

class CgiRun
{
    friend class InfoCgi;
    public:
        CgiRun(InfoCgi *infoCgi);
        CgiRun(CgiRun const &src);
        CgiRun &operator=(CgiRun const &src);
        ~CgiRun();
        std::string numberToString(int number);
        std::string executeCgi();
        char** maptochar() const;
        std::string checkContentLength(const std::string &response);
        char **cgiToArgv();

    private:
        std::map<std::string, std::string> _env;
        InfoCgi* _infoCgi;
        void _setEnv();


};

#endif