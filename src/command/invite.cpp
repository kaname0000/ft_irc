/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:25:26 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:25:27 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"

void invite(Client *client, Operation &operation, Server *server) {
    const std::vector<std::string> &params = operation.getParameter();
    if (params.size() < 2) {
        client->sendMessage("461 " + client->getNickname() + " INVITE :Not enough parameters");
        return;
    }

    Client *target = server->getClient(params[0]);
    Channel *channel = server->getChannel(params[1]);

    if (!target) {
        client->sendMessage("401 " + client->getNickname() + " " + params[0] + " :No such nick");
        return;
    }
    if (!channel) {
        client->sendMessage("403 " + client->getNickname() + " " + params[1] + " :No such channel");
        return;
    }
    if (!channel->isMember(client->getFd())) {
        client->sendMessage("442 " + client->getNickname() + " " + params[1] + " :You're not on that channel");
        return;
    }
    if (channel->getMode('i') && !channel->isOperator(client->getFd())) {
        client->sendMessage("482 " + client->getNickname() + " " + params[1] + " :You're not channel operator");
        return;
    }
    if (channel->isMember(target->getFd())) {
        client->sendMessage("443 " + client->getNickname() + " " + target->getNickname() + " " + params[1] + " :is already on channel");
        return;
    }

    channel->addInvite(target->getFd());
    client->sendMessage("341 " + client->getNickname() + " " + target->getNickname() + " " + params[1]);
    target->sendMessage(":" + client->getNickname() + " INVITE " + target->getNickname() + " :" + params[1]);
}
