
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

    Channel *channel = server->getChannel(params[0]);
    if (!channel) {
        client->sendMessage("403 " + client->getNickname() + " " + params[0] + " :No such channel");
        return;
    }

    if (params.size() == 1) {
        return;
    }

    if (!channel->isOperator(client->getFd())) {
        client->sendMessage("482 " + client->getNickname() + " " + params[0] + " :You're not channel operator");
        return;
    }

    std::string modes = params[1];
    bool active = true;
    size_t p_idx = 2;

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
    channel->broadcast(":" + client->getNickname() + " MODE " + channel->getName() + " " + modes);
}
