/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiRun.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 14:18:36 by akinzeli          #+#    #+#             */
/*   Updated: 2024/11/11 16:25:38 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/CgiRun.hpp"

CgiRun::CgiRun(InfoCgi *infoCgi) : _infoCgi(infoCgi)
{
    this->_setEnv();
}

CgiRun::CgiRun(CgiRun const &src)
{
    *this = src;
}

CgiRun &CgiRun::operator=(CgiRun const &src)
{
    if (this != &src)
    {
        this->_env = src._env;
        this->_infoCgi = src._infoCgi;
    }
    return *this;
}

CgiRun::~CgiRun()
{
    if (!_env.empty())
        _env.clear();
}

void CgiRun::_setEnv()
{
    Log::log(Log::DEBUG, "Setting up CGI environment");

    this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
    this->_env["SERVER_SOFTWARE"] = "webserv";
    this->_env["SERVER_PORT"] = numberToString(this->_infoCgi->_request->getConfigServer()->getPort());
    this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
    this->_env["REDIRECT_STATUS"] = "200";
    this->_env["REQUEST_METHOD"] = _infoCgi->_request->getMethod();
    this->_env["REQUEST_URI"] = _infoCgi->_request->getPath();
    this->_env["SCRIPT_NAME"] = _infoCgi->_request->getPath();
    this->_env["PATH_INFO"] = _infoCgi->_request->getPath();
    this->_env["PATH_TRANSLATED"] = _infoCgi->_request->getPath();
    this->_env["QUERY_STRING"] = _infoCgi->_request->getPath();
    if (_infoCgi->_request->getHeaders("Content-Length") != "")
        this->_env["CONTENT_LENGTH"] = _infoCgi->_request->getHeaders("Content-Length");

    if (_infoCgi->_request->getHeaders("Content-Type") != "")
        this->_env["CONTENT_TYPE"] = _infoCgi->_request->getHeaders("Content-Type");
    
}

std::string CgiRun::numberToString(int number)
{
    std::string result;
    std::ostringstream convert;
    convert << number;
    result = convert.str();
    return result;
}

std::string CgiRun::executeCgi()
{
    Log::log(Log::DEBUG, "Executing CGI");
    pid_t pid;
    char **env;
    char **argv = cgiToArgv();
    std::string response;
    int ret;
    int pipefd[2];

    //pipe(pipefd);
    if (pipe(pipefd) == -1)
    {
        Log::log(Log::ERROR, "Error while creating pipe");
        return "";
    }
    env = maptochar();
    pid = fork();
    if (pid == -1)
    {
        Log::log(Log::ERROR, "Error while forking");
        return "";
    }
    else if (!pid)
    {
        close(pipefd[0]);
        dup2(pipefd[1], 1);
        execve(_infoCgi->_path.c_str(), argv, env);
        exit(0);
    }
    else
    {
        char buffer[1024] = {0};
        int status;
        waitpid(-1, &status, 0);
        close(pipefd[1]);
        dup2(pipefd[0], 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
        {
            Log::log(Log::ERROR, "Error while executing CGI");
            return "";
        }
        ret = 1;
        while (ret > 0)
        {
            memset(buffer, 0, 1024);
            ret = read(pipefd[0], buffer, 1024 - 1);
            response += buffer;
        }
    }

    close(pipefd[1]);
    close(pipefd[0]);

    for (size_t i = 0; env[i]; i++)
        delete[] env[i];
    delete[] env;
    for (size_t i = 0; argv[i]; i++)
        delete[] argv[i];
    delete[] argv;
    Log::log(Log::DEBUG, "CGI executed successfully");
    return ("HTTP/1.1 200 OK\r\n" + checkContentLength(response));
}

char **CgiRun::maptochar() const
{
    char **env = new char*[this->_env.size() + 1];
    int i = 0;
    for (std::map<std::string, std::string>::const_iterator it = this->_env.begin(); it != this->_env.end(); it++)
    {
        env[i] = new char[it->first.size() + it->second.size() + 2];
        strcpy(env[i], (it->first + "=" + it->second).c_str());
        i++;
    }
    env[i] = NULL;
    return env;
}

std::string CgiRun::checkContentLength(const std::string &response)
{
    size_t pos = response.find("\r\n\r\n");
    std::string header = response.substr(0, pos);
    std::string body = response.substr(pos + 4);
    std::string contentLength = "\nContent-Length: " + numberToString(body.size()) + "\r\n";
    return header + contentLength + "\r\n" + body;
}

char **CgiRun::cgiToArgv()
{
    char **argv = new char*[3];
    argv[0] = new char[this->_infoCgi->_cgiPath.size() + 1];
    strcpy(argv[0], _infoCgi->_cgiPath.c_str());
    argv[1] = new char[this->_infoCgi->_path.size() + 1];
    strcpy(argv[1], _infoCgi->_path.c_str());
    argv[2] = NULL;
    return argv;
}