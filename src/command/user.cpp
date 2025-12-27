/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:26:32 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:26:33 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"
#include "../../includes/utils.hpp"

#define INT16_MAX 32767

void user(Client *client, Operation &operation, Server *server)
{
    (void)server;
    if (!client->isAuthenticated())
    {
        client->sendMessage(":server.example.com 451 * USER :You have not registered (PASS required)");
        return;
    }
    if (client->isRegisteredUsername())
    {
        client->sendMessage(":server.example.com 462 " + client->getNickname() + " :Unauthorized command (already registered)");
        return;
    }

    std::vector<std::string> param = operation.getParameter();

    if (param.size() < 3 || operation.getTrailing().empty())
    {
        client->sendMessage(":server.example.com 461 " + (client->getNickname().empty() ? "*" : client->getNickname()) + " USER :Not enough parameters");
        return;
    }

    client->setUsername(param[0]);
    client->setHostname("127.0.0.1"); 
    client->setServername(param[2]);
    client->setRealname(operation.getTrailing());
    client->setRegisteredUsername(true);

    if (client->isAuthenticated() && client->isRegisteredNickname() && client->isRegisteredUsername())
    {
        std::string nick = client->getNickname();
        std::string user = client->getUsername();
        std::string host = client->getHostname();

        if (!client->isRegistered())
            client->setRegistered(true);
        client->sendMessage(":server.example.com 001 " + nick + " :Welcome to the Internet Relay Network " + nick + "!" + user + "@" + host);
    }
}
