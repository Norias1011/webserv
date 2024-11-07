/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: akinzeli <akinzeli@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 11:31:24 by akinzeli          #+#    #+#             */
/*   Updated: 2024/11/07 15:39:04 by akinzeli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Config.hpp"
#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include <signal.h>

Server *global_server = NULL;

void signalHandler(int signum)
{
	if (global_server)
    {
        global_server->closeServer(); // TO DO
        delete global_server;
        global_server = NULL;
    }
    std::cout << "Interrupt signal (" << signum << ") received." << std::endl;
    exit(signum);
}

int main(int argc, char *argv[])
{
    //signal(SIGINT, signalHandler);
    if (argc == 2 || argc == 1)
    {
        try
        {
            std::string config_file;
            Config config;
            config_file = (argc == 2) ? argv[1] : "configs/default.conf";
            signal(SIGINT, signalHandler);
            config.parseConfig(config_file);
            config.printAll();
		
		    global_server= new Server(config);
		    global_server->createSocket();
            global_server->runServer();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            return (1);
        }
    }
    else
    {
        std::cout << "Wrong format : ./webserv [config_file]" << std::endl;
        return 1;
    }
    return 0;
}