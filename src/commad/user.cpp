#include "Operation.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "utils.hpp"

// USER username

void nick(Client *client, Operation &operation, Server *server)
{
    (void)server;

    std::vector<std::string> param = operation.getParameter();

    // disallow mult parameter or no parameters
    if (param.size() != 1)
        return;

    std::string newUsername = param[0];

    // check Username
    if (!isValidName(newUsername, "-_", INT16_MAX))
        return;

    client->setUsername(operation.getParameter()[0]);
}
