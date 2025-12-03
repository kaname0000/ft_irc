#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Operation.hpp"

Server::Server() {}
Server::~Server() {}

void Server::handleClientMessage(Client *client, const std::string &msg)
{
    Operation operation(msg);

    CommandFunc command = operation.getCommandFunc();
    command(client, operation, this->data);
}
