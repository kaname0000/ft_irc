
#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"

void part(Client *client, Operation &operation, Server *server) {
    const std::vector<std::string> &params = operation.getParameter();
    if (params.empty()) {
        client->sendMessage("461 " + client->getNickname() + " PART :Not enough parameters");
        return;
    }

    Channel *channel = server->getChannel(params[0]);
    if (!channel) {
        client->sendMessage("403 " + client->getNickname() + " " + params[0] + " :No such channel");
        return;
    }
    if (!channel->isMember(client->getFd())) {
        client->sendMessage("442 " + client->getNickname() + " " + params[0] + " :You're not on that channel");
        return;
    }

    std::string reason = operation.getTrailing().empty() ? "Leaving" : operation.getTrailing();
    channel->broadcast(":" + client->getNickname() + " PART " + params[0] + " :" + reason);
    channel->removeMember(client->getFd());

    if (channel->getMemberCount() == 0) server->removeChannel(params[0]);
}
