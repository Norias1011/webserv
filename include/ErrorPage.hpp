/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPage.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 13:25:17 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/17 14:38:42 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORPAGE_HPP
#define ERRORPAGE_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <sstream>

class ErrorPage
{
    public:
        ErrorPage();
        ErrorPage(std::string errorPage);
        ~ErrorPage();
        ErrorPage(ErrorPage const &copy);
        ErrorPage& operator=(ErrorPage const &src);
        std::string getErrorPage(int code);
        std::string getConfigErrorPage(std::map<int, std::string> configErrorPage , int code);

        std::string getErrorMessages(int code) const;
        std::string numberToString(int number);

    private:
        std::map<int, std::string> _errorMessages;
};

#endif