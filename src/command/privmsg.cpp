/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: okaname <okaname@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:26:12 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/28 16:38:28 by okaname          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
        Channel *channel = server->getChannel(target);
        if (!channel)
        {
            client->sendMessage("403 " + target + " :No such channel");
            return;
        }
        if (!channel->isMember(client->getFd()))
        {
            client->sendMessage("442 " + client->getNickname() + " " + target + " :You're not on that channel");
            return;
        }
        std::string prefix = client->getClientdata();
        channel->broadcast(prefix + " PRIVMSG " + target + " :" + message, client->getFd());
    }
    else
    {
        Client *dest = server->getClient(target);
        if (!dest)
        {
            client->sendMessage("401 " + target + " :No such nick");
            return;
        }
        dest->sendMessage(client->getClientdata() + " PRIVMSG " + target + " :" + message);
        std::cout << message << std::endl;
    }
}
