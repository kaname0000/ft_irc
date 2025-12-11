#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"
#include "../../includes/utils.hpp"

// USER username

void nick(Client *client, Operation &operation, Server *server)
{
    (void)server;

    std::vector<std::string> param = operation.getParameter();

    // disallow mult parameter or no parameters
    if (param.size() != 3)
        return;

    if (!isValidName(param[0], "-_", INT16_MAX))
    {
        client->sendMessage(":server.example.com 432 * :Erroneous nickname (invalid user)");
        return;
    }
    if (!isValidName(param[1], "-_", INT16_MAX))
    {
        client->sendMessage(":server.example.com 432 * :Erroneous nickname (invalid user)");
        return;
    }
    if (!isValidName(param[2], "-_", INT16_MAX))
    {
        client->sendMessage(":server.example.com 432 * :Erroneous nickname (invalid user)");
        return;
    }
    if (!isValidName(operation.getTrailing(), "-_", INT16_MAX))
    {
        client->sendMessage(":server.example.com 432 * :Erroneous nickname (invalid user)");
        return;
    }

    client->setUsername(param[0]);
    client->setHostname(param[1]);
    client->setServername(param[2]);
    client->setRealname(operation.getTrailing());
}
