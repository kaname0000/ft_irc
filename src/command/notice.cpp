/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   notice.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:25:54 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:25:55 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
        std::string prefix = client->getClientdata();
        channel->broadcast(prefix + " NOTICE " + target + " :" + message, client->getFd());
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
