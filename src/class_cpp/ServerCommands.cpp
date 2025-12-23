
#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"

#include <iostream>
#include <cstdlib>

static bool isValidChannelName(const std::string &name) {
    if (name.empty() || name[0] != '#' || name.length() > 50) return false;
    return true;
}

void Server::handleJoin(Client *client, const std::vector<std::string> &params) {
    if (!client->isRegistered()) {
        client->sendMessage("451 * :You have not registered");
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

    Channel *channel = getChannel(channel_name);
    if (!channel) {
        channel = createChannel(channel_name);
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

void Server::handlePart(Client *client, const std::vector<std::string> &params) {
    if (params.empty()) {
        client->sendMessage("461 " + client->getNickname() + " PART :Not enough parameters");
        return;
    }

    Channel *channel = getChannel(params[0]);
    if (!channel) {
        client->sendMessage("403 " + client->getNickname() + " " + params[0] + " :No such channel");
        return;
    }
    if (!channel->isMember(client->getFd())) {
        client->sendMessage("442 " + client->getNickname() + " " + params[0] + " :You're not on that channel");
        return;
    }

    std::string reason = (params.size() > 1) ? params[1] : "Leaving";
    channel->broadcast(":" + client->getNickname() + " PART " + params[0] + " :" + reason);
    channel->removeMember(client->getFd());

    if (channel->getMemberCount() == 0) removeChannel(params[0]);
}

void Server::handleKick(Client *client, const std::vector<std::string> &params) {
    if (params.size() < 2) {
        client->sendMessage("461 " + client->getNickname() + " KICK :Not enough parameters");
        return;
    }

    Channel *channel = getChannel(params[0]);
    Client *target = getClient(params[1]);

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

    channel->broadcast(":" + client->getNickname() + " KICK " + params[0] + " " + target->getNickname() + " :Kicked");
    channel->removeMember(target->getFd());
}

void Server::handleMode(Client *client, const std::vector<std::string> &params) {
    if (params.size() < 2) return;
    Channel *channel = getChannel(params[0]);
    if (!channel || !channel->isOperator(client->getFd())) {
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
                if (active && p_idx < params.size()) channel->setMode('k', true), channel->setKey(params[p_idx++]);
                else channel->setMode('k', false), channel->setKey("");
                break;
            case 'l':
                if (active && p_idx < params.size()) channel->setMode('l', true), channel->setLimit(std::atoi(params[p_idx++].c_str()));
                else channel->setMode('l', false);
                break;
            case 'o':
                if (p_idx < params.size()) {
                    Client *t = getClient(params[p_idx++]);
                    if (t) active ? channel->addOperator(t) : channel->removeOperator(t->getFd());
                }
                break;
        }
    }
    channel->broadcast(":" + client->getNickname() + " MODE " + channel->getName() + " " + modes);
}

void Server::handleInvite(Client *client, const std::vector<std::string> &params) {
    if (params.size() < 2) {
        client->sendMessage("461 " + client->getNickname() + " INVITE :Not enough parameters");
        return;
    }

    Client *target = getClient(params[0]);
    Channel *channel = getChannel(params[1]);

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
    target->sendMessage(":" + client->getNickname() + " INVITE " + target->getNickname() + " :" + params[1] + "\r\n");
}

void Server::handleTopic(Client *client, const std::vector<std::string> &params) {
    if (params.empty()) {
        client->sendMessage("461 " + client->getNickname() + " TOPIC :Not enough parameters");
        return;
    }
    Channel *channel = getChannel(params[0]);
    if (!channel) {
        client->sendMessage("403 " + client->getNickname() + " " + params[0] + " :No such channel");
        return;
    }
    if (params.size() == 1) {
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

    channel->setTopic(params[1]);
    channel->broadcast(":" + client->getNickname() + " TOPIC " + channel->getName() + " :" + params[1]);
}

void Server::handleQuit(Client *client, const std::vector<std::string> &params) {
    std::string reason = (params.empty()) ? "Client Quit" : params[0];
    std::string quit_notification = ":" + client->getNickname() + "!" + client->getUsername() 
                                    + "@localhost QUIT :Quit: " + reason;

    std::map<std::string, Channel *>::iterator it = _channels.begin();
    while (it != _channels.end()) {
        Channel *channel = it->second;
        
        if (channel->isMember(client->getFd())) {
            channel->broadcast(quit_notification, client->getFd());
            channel->removeMember(client->getFd());        
            if (channel->getMemberCount() == 0) {
                std::string chan_name = it->first;
                std::map<std::string, Channel *>::iterator next_it = it;
                ++next_it;
                removeChannel(chan_name);
                it = next_it;
                continue;
            }
        }
        ++it;
    }

    client->sendMessage("ERROR :Closing link: (Quit: " + reason + ")");
    std::cout << "QUIT: User " << client->getNickname() << " has left the server." << std::endl;
}
