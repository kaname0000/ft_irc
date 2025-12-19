#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"
#include "../../includes/utils.hpp"

void notice(Client *client, Operation &op, Server *server)
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
        Channel *channel = server->getChannel(target);
        if (!channel)
        {
            client->sendMessage("403 " + target + " :No such channel");
            return;
        }
        channel->broadcast(message);
    }
    else
    {
        Client *dest = server->getClient(target);
        if (!dest)
        {
            client->sendMessage("401 " + target + " :No such nick");
            return;
        }
        dest->sendMessage(client->getClientdata() + " NOTICE " + target + " :" + message);
        
    }
}
