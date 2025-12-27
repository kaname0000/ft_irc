/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:25:49 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:25:50 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"
#include "../../includes/utils.hpp"

void nick(Client *client, Operation &operation, Server *server)
{
    (void)server;
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

    client->setNickname(newNickname);
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

}
