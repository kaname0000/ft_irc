#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"
#include "../../includes/utils.hpp"

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
        // std::cout << client->getNickname() << "->" << dest->getNickname() << std::endl;
        // dest->sendMessage(client->getClientdata() + " PRIVMSG " + target + " :" + message);
        dest->sendMessage(":" + client->getNickname() + " PRIVMSG " + target + " :" + message);
    }
}
