/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ErrorPage.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/17 13:25:21 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/17 14:47:58 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ErrorPage.hpp"

ErrorPage::ErrorPage()
{
    _errorMessages[200] = "OK";
    _errorMessages[201] = "Created";
    _errorMessages[202] = "Accepted";
    _errorMessages[204] = "No Content";

    _errorMessages[301] = "Moved Permanently";
    _errorMessages[302] = "Found";
    _errorMessages[303] = "See Other";
    _errorMessages[304] = "Not Modified";
    _errorMessages[307] = "Temporary Redirect";
    _errorMessages[308] = "Permanent Redirect";

    _errorMessages[400] = "Bad Request";
    _errorMessages[401] = "Unauthorized";
    _errorMessages[403] = "Forbidden";
    _errorMessages[404] = "Not Found";
    _errorMessages[405] = "Method Not Allowed";
    _errorMessages[406] = "Not Acceptable";
    _errorMessages[408] = "Request Timeout";
    _errorMessages[409] = "Conflict";
    _errorMessages[410] = "Gone";
    _errorMessages[411] = "Length Required";
    _errorMessages[413] = "Payload Too Large";
    _errorMessages[414] = "URI Too Long";
    _errorMessages[415] = "Unsupported Media Type";
    _errorMessages[416] = "Range Not Satisfiable";
    _errorMessages[417] = "Expectation Failed";
    _errorMessages[418] = "I'm a teapot";
    _errorMessages[426] = "Upgrade Required";
    _errorMessages[428] = "Precondition Required";
    _errorMessages[429] = "Too Many Requests";

    _errorMessages[500] = "Internal Server Error";
    _errorMessages[501] = "Not Implemented";
    _errorMessages[502] = "Bad Gateway";
    _errorMessages[503] = "Service Unavailable";
    _errorMessages[504] = "Gateway Timeout";
    _errorMessages[505] = "HTTP Version Not Supported";

    _errorMessages[100] = "Continue";
    _errorMessages[101] = "Switching Protocols";
    _errorMessages[102] = "Processing";
    _errorMessages[103] = "Early Hints";
    _errorMessages[226] = "IM Used";
    _errorMessages[207] = "Multi-Status";
    _errorMessages[208] = "Already Reported";
    _errorMessages[226] = "IM Used";
}

ErrorPage::~ErrorPage()
{
}

ErrorPage::ErrorPage(ErrorPage const &copy)
{
    *this = copy;
}

ErrorPage &ErrorPage::operator=(ErrorPage const &src)
{
    if (this != &src)
    {
        _errorMessages = src._errorMessages;
    }
    return *this;
}

std::string ErrorPage::getErrorPage(int code)
{
    std::string messageError = getErrorMessages(code);
    std::string errorPage = "<!DOCTYPE html> <html><head><title>" + messageError + "</title></head><body>";
    errorPage += "<h1>" + messageError + "</h1>";
    errorPage += "</body></html>";
    std::string header = "HTTP/1.1 " + numberToString(code) + " " + messageError + "\r\n";
    header += "Content-Type: text/html\r\n";
    header += "Content-Length: " + numberToString(errorPage.size()) + "\r\n";
    header += "\r\n";
    errorPage = header + errorPage;
    return errorPage;
}

std::string ErrorPage::getConfigErrorPage(std::map<int, std::string> configErrorPage , int code)
{
    std::map<int, std::string>::const_iterator it = configErrorPage.find(code);
    if (it == configErrorPage.end())
        return "Error code not found";
    std::string messageError = it->second;
    std::string errorPage;
    std::ifstream file(messageError.c_str());
    if (file.is_open())
    {
        std::string line;
        while (getline(file, line))
        {
            errorPage += line;
        }
        file.close();
    }
    else
    {
        std::cout << "Error: Unable to open file in Error Page" << std::endl;
        return "";
    }
    std::string header = "HTTP/1.1 " + numberToString(code) + " " + getErrorMessages(code) + "\r\n";
    header += "Content-Type: text/html\r\n"; // change with the type of file
    header += "Content-Length: " + numberToString(errorPage.size()) + "\r\n";
    header += "\r\n";
    errorPage = header + errorPage;
    return errorPage;
}

std::string ErrorPage::getErrorMessages(int code) const
{
    std::map<int, std::string>::const_iterator it = _errorMessages.find(code);
    if (it != _errorMessages.end())
    {
        return it->second;
    }
    else
    {
        return "Error code not found";
    }
}

std::string ErrorPage::numberToString(int number)
{
    std::string result;
    std::ostringstream convert;
    convert << number;
    result = convert.str();
    return result;
}