/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:26:05 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:26:06 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"
#include "../../includes/utils.hpp"


void pass(Client *client, Operation &operation, Server *server)
{
    if (client->isRegisteredNickname() || client->isRegisteredUsername()) {
        client->sendMessage(":server 462 " + (client->getNickname().empty() ? "*" : client->getNickname()) + " :Unauthorized command (already registered)");
        return;
    }
    if (client->isAuthenticated()) {
        client->sendMessage(":server 462 " + (client->getNickname().empty() ? "*" : client->getNickname()) + " :Unauthorized command (already registered)");
        return;
    }

    std::vector<std::string> param = operation.getParameter();

    if (param.size() < 1) { 
        client->sendMessage(":server 461 * PASS :Not enough parameters");
        return;
    }

    if (param[0] != server->getPassword()) {
        client->sendMessage(":server 464 * :Password incorrect");
        return;
    }

    client->setAuthenticated(true);

    if (client->isAuthenticated() && client->isRegisteredUsername() && client->isRegisteredNickname())
    {
        if (!client->isRegistered())
            client->setRegistered(true);
        client->sendMessage(":server 001 " + client->getNickname() + " :Welcome to the Internet Relay Network " + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname());
    }
}
