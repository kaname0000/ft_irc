#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Operation.hpp"

// Stub: ignore WHO/WHOIS requests to avoid 421 Unknown
void who(Client *client, Operation &operation, Server *server)
{
    (void)client;
    (void)operation;
    (void)server;
    // No reply; just accept to suppress Unknown command
}
