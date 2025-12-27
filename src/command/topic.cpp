/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:26:26 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:26:27 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"

void topic(Client *client, Operation &operation, Server *server) {
    const std::vector<std::string> &params = operation.getParameter();
    if (params.empty()) {
        client->sendMessage("461 " + client->getNickname() + " TOPIC :Not enough parameters");
        return;
    }

    Channel *channel = server->getChannel(params[0]);
    if (!channel) {
        client->sendMessage("403 " + client->getNickname() + " " + params[0] + " :No such channel");
        return;
    }

    if (operation.getTrailing().empty() && params.size() == 1) {
        if (channel->getTopic().empty())
            client->sendMessage("331 " + client->getNickname() + " " + channel->getName() + " :No topic is set");
        else
            client->sendMessage("332 " + client->getNickname() + " " + channel->getName() + " :" + channel->getTopic());
        return;
    }

    if (!channel->isMember(client->getFd())) {
        client->sendMessage("442 " + client->getNickname() + " " + params[0] + " :You're not on that channel");
        return;
    }
    if (channel->getMode('t') && !channel->isOperator(client->getFd())) {
        client->sendMessage("482 " + client->getNickname() + " " + params[0] + " :You're not channel operator");
        return;
    }

    std::string new_topic = operation.getTrailing();
    channel->setTopic(new_topic);
    channel->broadcast(":" + client->getNickname() + " TOPIC " + channel->getName() + " :" + new_topic);
}
