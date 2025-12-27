/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:25:32 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:25:33 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"

static bool isValidChannelName(const std::string &name) {
    if (name.empty() || (name[0] != '#' && name[0] != '&') || name.length() > 50) return false;
    return true;
}

void join(Client *client, Operation &operation, Server *server) {
    const std::vector<std::string> &params = operation.getParameter();

    if (!(client->isAuthenticated() && client->isRegisteredNickname() && client->isRegisteredUsername())) {
        client->sendMessage("451 " + client->getNickname() + " JOIN :You have not registered");
        return;
    }
    if (params.empty()) {
        client->sendMessage("461 " + client->getNickname() + " JOIN :Not enough parameters");
        return;
    }

    std::string channel_name = params[0];
    std::string key = (params.size() > 1) ? params[1] : "";

    if (!isValidChannelName(channel_name)) {
        client->sendMessage("403 " + client->getNickname() + " " + channel_name + " :No such channel");
        return;
    }

    Channel *channel = server->getChannel(channel_name);
    if (!channel) {
        channel = server->createChannel(channel_name);
        channel->addOperator(client);
    } else {
        if (channel->isMember(client->getFd())) return;
        if (channel->getMode('i') && !channel->isInvited(client->getFd())) {
            client->sendMessage("473 " + client->getNickname() + " " + channel_name + " :Cannot join channel (+i)");
            return;
        }
        if (channel->getMode('k') && channel->getKey() != key) {
            client->sendMessage("475 " + client->getNickname() + " " + channel_name + " :Cannot join channel (+k)");
            return;
        }
        if (channel->getMode('l') && channel->getLimit() > 0 && channel->getMemberCount() >= channel->getLimit()) {
            client->sendMessage("471 " + client->getNickname() + " " + channel_name + " :Cannot join channel (+l)");
            return;
        }
    }

    channel->addMember(client);
    if (channel->isInvited(client->getFd())) channel->removeInvite(client->getFd());

    std::string join_msg = ":" + client->getNickname() + "!" + client->getUsername() + "@localhost JOIN :" + channel_name;
    channel->broadcast(join_msg);

    if (!channel->getTopic().empty())
        client->sendMessage("332 " + client->getNickname() + " " + channel_name + " :" + channel->getTopic());

    channel->sendNamesReply(client);
}
