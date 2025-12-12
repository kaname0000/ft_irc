#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"
#include "../../includes/utils.hpp"

// PASS password

void pass(Client *client, Operation &operation, Server *server)
{

    if (client->isAuthenticated())
    {
        client->sendMessage("* You are already registered");
        return;
    }
    std::vector<std::string> param = operation.getParameter();

    // disallow mult parameter or no parameters

    // std::cout << "pass func\n";
    if (param.size() != 1)
    {
        client->sendMessage(":server 461 PASS :Not enough parameters");
        std::cout << "error\n";
        return;
    }

    if (param[0] != server->getPassword())
    {
        client->sendMessage(":server 464 * :Password incorrect");
        std::cout << "error\n";
        return;
    }
    else
    {
        if (client->isAuthenticated() && client->isRegisteredUsername())
            client->sendMessage(":server 001 " + client->getNickname() + " :Welcome to the Internet Relay Network " + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname());
        client->setAuthenticated(true);
        // std::cout << "pass ok!\n";
        return;
    }
}
