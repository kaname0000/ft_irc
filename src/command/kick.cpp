/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kick.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:25:40 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:25:41 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"

void kick(Client *client, Operation &operation, Server *server) {
    const std::vector<std::string> &params = operation.getParameter();
    if (params.size() < 2) {
        client->sendMessage("461 " + client->getNickname() + " KICK :Not enough parameters");
        return;
    }

    Channel *channel = server->getChannel(params[0]);
    Client *target = server->getClient(params[1]);

    if (!channel) {
        client->sendMessage("403 " + client->getNickname() + " " + params[0] + " :No such channel");
        return;
    }
    if (!channel->isOperator(client->getFd())) {
        client->sendMessage("482 " + client->getNickname() + " " + params[0] + " :You're not channel operator");
        return;
    }
    if (!target || !channel->isMember(target->getFd())) {
        client->sendMessage("441 " + client->getNickname() + " " + params[1] + " " + params[0] + " :They aren't on that channel");
        return;
    }

    std::string reason = operation.getTrailing().empty() ? "Kicked by operator" : operation.getTrailing();
    channel->broadcast(":" + client->getNickname() + " KICK " + params[0] + " " + target->getNickname() + " :" + reason);
    channel->removeMember(target->getFd());
}
