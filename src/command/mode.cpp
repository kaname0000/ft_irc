/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:25:45 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:25:46 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"

#include <cstdlib>

void mode(Client *client, Operation &operation, Server *server) {
    const std::vector<std::string> &params = operation.getParameter();

    if (params.size() < 1) {
        client->sendMessage("461 " + client->getNickname() + " MODE :Not enough parameters");
        return;
    }

    if (params.size() >= 2 && params[1].find('b') != std::string::npos) {
        return;
    }

    if (params[0] == client->getNickname()) {
        return;
    }

    Channel *channel = server->getChannel(params[0]);
    if (!channel) {
        client->sendMessage("403 " + client->getNickname() + " " + params[0] + " :No such channel");
        return;
    }

    if (params.size() == 1) {
        std::string modes = "+";
        if (channel->getMode('i')) modes += "i";
        if (channel->getMode('t')) modes += "t";
        if (channel->getMode('k')) modes += "k";
        if (channel->getMode('l')) modes += "l";
        client->sendMessage("324 " + client->getNickname() + " " + channel->getName() + " " + modes);
        return;
    }

    if (!channel->isOperator(client->getFd())) {
        client->sendMessage("482 " + client->getNickname() + " " + params[0] + " :You're not channel operator");
        return;
    }

    std::string modes = params[1];
    bool active = true;
    size_t p_idx = 2;
    std::string broadcastParams = params[1];
    for (size_t i = 2; i < params.size(); ++i) {
        broadcastParams += " " + params[i];
    }

    for (size_t i = 0; i < modes.length(); ++i) {
        if (modes[i] == '+') { active = true; continue; }
        if (modes[i] == '-') { active = false; continue; }

        switch (modes[i]) {
            case 'i': channel->setMode('i', active); break;
            case 't': channel->setMode('t', active); break;
            case 'k':
                if (active && p_idx < params.size()) {
                    channel->setMode('k', true);
                    channel->setKey(params[p_idx++]);
                } else {
                    channel->setMode('k', false);
                    channel->setKey("");
                }
                break;
            case 'l':
                if (active && p_idx < params.size()) {
                    channel->setMode('l', true);
                    channel->setLimit(std::atoi(params[p_idx++].c_str()));
                } else {
                    channel->setMode('l', false);
                }
                break;
            case 'o':
                if (p_idx < params.size()) {
                    Client *target = server->getClient(params[p_idx++]);
                    if (target) {
                        active ? channel->addOperator(target) : channel->removeOperator(target->getFd());
                    }
                }
                break;
        }
    }
    channel->broadcast(":" + client->getNickname() + " MODE " + channel->getName() + " " + broadcastParams);
}
