/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quit.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:26:16 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:26:17 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/class_hpp/Operation.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Server.hpp"

void quit(Client *client, Operation &operation, Server *server)
{
    std::string reason = operation.getTrailing().empty() ? "Client Quit" : operation.getTrailing();

    std::string quit_notification = ":" + client->getNickname() + "!" + client->getUsername()
                                    + "@localhost QUIT :Quit: " + reason + "\r\n";

    std::map<std::string, Channel *> channels = server->getChannels();
    std::map<std::string, Channel *>::iterator it = channels.begin();

    while (it != channels.end()) {
        Channel *channel = it->second;
        if (channel->isMember(client->getFd())) {
            channel->broadcast(quit_notification, client->getFd());
            channel->removeMember(client->getFd());

            if (channel->getMemberCount() == 0) {
                std::string chan_name = it->first;
                ++it;
                server->removeChannel(chan_name);
                continue;
            }
        }
        ++it;
    }
    client->sendMessage("ERROR :Closing link: (Quit: " + reason + ")\r\n");
    std::cout << "QUIT: User " << client->getNickname() << " has left." << std::endl;
}
