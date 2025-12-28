/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: okaname <okaname@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:25:49 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/28 17:14:03 by okaname          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"
#include "../../includes/utils.hpp"

#include <algorithm>

std::string toUpper(std::string s) {
    for (size_t i = 0; i < s.length(); i++) {
        s[i] = std::toupper(static_cast<unsigned char>(s[i]));
    }
    return s;
}

bool same_name(const std::map<int, Client*>& clients,
               const std::string& newNickname,
               const Client* self)
{
    std::string newUpper = toUpper(newNickname);

    for (std::map<int, Client*>::const_iterator it = clients.begin();
         it != clients.end();
         ++it)
    {
        if (it->second == self)
            continue;

        if (it->second) {
            if (toUpper(it->second->getNickname()) == newUpper) {
                return true;
            }
        }
    }
    return false;
}

void nick(Client *client, Operation &operation, Server *server)
{
    std::vector<std::string> param = operation.getParameter();

    if (!client->isAuthenticated()) {
        client->sendMessage(":server 451 * NICK :You have not registered (PASS required)");
        return;
    }

    if (param.size() != 1) {
        client->sendMessage(":server 431 " + client->getNickname() + " :No nickname given");
        return;
    }

    std::string newNickname = param[0];
    std::string oldNickname = client->getNickname();

    if (!isValidName(newNickname, "-_[]\\`^{}", 9)) {
        client->sendMessage(":server 432 " + oldNickname + " " + newNickname + " :Erroneous nickname");
        return;
    }
    // (void)server;
    if (same_name(server->getClients(), newNickname, client))
    {
        client->sendMessage("433 * " + newNickname + " :Nickname is already in use");
        return;
    }

    client->setRegisteredNickname(true);

    if (client->isAuthenticated() && client->isRegisteredNickname() && client->isRegisteredUsername() && !client->isRegistered()) {
        client->setRegistered(true);
        client->sendMessage(":server.example.com 001 " + newNickname + " :Welcome to the Internet Relay Network " + client->getClientdata());
    }

    std::string prefix = client->getNickname();
    if (!client->getUsername().empty() && !client->getHostname().empty())
        prefix += "!" + client->getUsername() + "@" + client->getHostname();
    std::string msg = ":" + prefix + " NICK :" + newNickname;
    client->sendMessage(msg);
	client->setNickname(newNickname);
}
