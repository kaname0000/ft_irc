/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:26:43 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/28 16:27:05 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/class_hpp/Server.hpp"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <csignal>

volatile sig_atomic_t g_running = 1;

static void handle_sigint(int)
{
    g_running = 0;
}

static bool isValidPort(const std::string &s, int &port)
{
    char *end = NULL;
    long value = std::strtol(s.c_str(), &end, 10);
    if (*s.c_str() == '\0' || (end && *end != '\0'))
        return false;
    if (value < 1 || value > 65535)
        return false;
    port = static_cast<int>(value);
    return true;
}

int main(int argc, char **argv)
{
    std::signal(SIGPIPE, SIG_IGN);
    std::signal(SIGINT, handle_sigint);
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port> <password>" << std::endl;
        return 1;
    }

    int port = 0;
    if (!isValidPort(argv[1], port))
    {
        std::cerr << "Invalid port: " << argv[1] << std::endl;
        return 1;
    }

    try
    {
        Server server(port, argv[2]);
        std::cout << "Server socket initialized on port " << port << std::endl;
        std::cout << "Listening fd: " << server.getListenFd() << std::endl;
        server.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to initialize server: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
