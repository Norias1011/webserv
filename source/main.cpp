/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 11:31:24 by akinzeli          #+#    #+#             */
/*   Updated: 2024/10/10 11:15:08 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Config.hpp"
#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include <signal.h>

void signalHandler(int signum)
{
    std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
    exit(signum);
}

int main(int argc, char *argv[])
{
    if (argc == 2 || argc == 1)
    {
        std::string config_file;
       // Config config;
		Server server;
        Config config;
        config_file = (argc == 2) ? argv[1] : "configs/default.conf";
        signal(SIGINT, signalHandler);
        config.parseConfig(config_file);
        config.printAll();
        //config.parseConfig(config_file);
		if (server.createSocket()== - 1)
			return(-1);
        if (server.runServer()== - 1)
			return(-1);
    }
    else
    {
        std::cout << "Wrong format : ./webserv [config_file]" << std::endl;
        return 1;
    }
    return 0;
}