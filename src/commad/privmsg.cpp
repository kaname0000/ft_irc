#include "Operation.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "../utils.hpp"

void privmsg(Client *client, Operation &op, Server *server)
{
    const std::vector<std::string> &params = op.getParameter();
    const std::string &message = op.getTrailing();

    if (params.empty())
    {
        client->sendMessage("431 :No recipient given");
        return;
    }

    std::string target = params[0];

    if (target[0] == '#')
    {
        // to channel
        Channel *channel = server->getChannel(target);
        if (!channel)
        {
            client->sendMessage("403 " + target + " :No such channel"); // ERR_NOSUCHCHANNEL
            return;
        }
        channel->broadcast(message);
    }
    else
    {
        // to client
        Client *dest = server->getClient(target);
        if (!dest)
        {
            client->sendMessage("401 " + target + " :No such nick"); // ERR_NOSUCHNICK
            return;
        }
        dest->sendMessage(":" + client->getNickname() + " PRIVMSG " + target + " :" + message);
    }
}
