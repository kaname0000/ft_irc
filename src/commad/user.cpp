#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"
#include "../../includes/utils.hpp"

#define INT16_MAX 32767

// USER username

void user(Client *client, Operation &operation, Server *server)
{

    if (client->isRegisteredUsername())
    {
        client->sendMessage("* You are already registered");
        return;
    }
    (void)server;

    std::vector<std::string> param = operation.getParameter();

    // disallow mult parameter or no parameters
    if (param.size() != 3)
        return;

    if (!isValidName(param[0], "-_", INT16_MAX))
    {
        std::cout << "1\n";
        client->sendMessage(":server.example.com 432 * :Erroneous nickname (invalid user)");
        return;
    }
    if (!isValidName(param[1], "-_", INT16_MAX))
    {
        std::cout << "2\n";
        client->sendMessage(":server.example.com 432 * :Erroneous nickname (invalid user)");
        return;
    }
    // if (!isValidName(param[2], "-_", INT16_MAX))
    // {
    //     std::cout << "3\n";
    //     client->sendMessage(":server.example.com 432 * :Erroneous nickname (invalid user)");
    //     return;
    // }
    if (!isValidName(operation.getTrailing(), "-_", INT16_MAX))
    {
        std::cout << "4\n";
        client->sendMessage(":server.example.com 432 * :Erroneous nickname (invalid user)");
        return;
    }

    client->setUsername(param[0]);
    // client->setHostname(param[1]);
    client->setHostname("127.0.0.1");
    client->setServername(param[2]);
    client->setRealname(operation.getTrailing());
    client->setRegisteredUsername(true);
    if (client->isAuthenticated() && client->isRegisteredNickname())
        client->sendMessage(":server.example.com 001 " + client->getNickname() + " :Welcome to the Internet Relay Network " + client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname());
    // std::cout << "user ok!\n";
}
