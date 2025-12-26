#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"
#include "../../includes/utils.hpp"


void pass(Client *client, Operation &operation, Server *server)
{
    // PASS must be the first authentication step; reject if nick/user already set
    if (client->isRegisteredNickname() || client->isRegisteredUsername()) {
        client->sendMessage(":server 462 " + (client->getNickname().empty() ? "*" : client->getNickname()) + " :Unauthorized command (already registered)");
        return;
    }
    if (client->isAuthenticated()) {
        client->sendMessage(":server 462 " + (client->getNickname().empty() ? "*" : client->getNickname()) + " :Unauthorized command (already registered)");
        return;
    }

    std::vector<std::string> param = operation.getParameter();

    if (param.size() < 1) { 
        client->sendMessage(":server 461 * PASS :Not enough parameters");
        return;
    }

    if (param[0] != server->getPassword()) {
        client->sendMessage(":server 464 * :Password incorrect");
        return;
    }

    client->setAuthenticated(true);

    if (client->isAuthenticated() && client->isRegisteredUsername() && client->isRegisteredNickname())
    {
        if (!client->isRegistered())
            client->setRegistered(true);
        client->sendMessage(":server 001 " + client->getNickname() + " :Welcome to the Internet Relay Network " + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname());
    }
}
