#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Operation.hpp"

// Accept CAP negotiation attempts and do nothing (no capabilities advertised)
void cap(Client *client, Operation &operation, Server *server)
{
    (void)client;
    (void)operation;
    (void)server;
    // Silently ignore; avoids sending 421 Unknown command to clients like irssi
}
