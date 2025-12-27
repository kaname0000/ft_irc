/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:26:08 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:26:09 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Operation.hpp"

void ping(Client *client, Operation &operation, Server *server)
{
    (void)server;
    std::string token;
    const std::vector<std::string> &params = operation.getParameter();

    if (!params.empty())
        token = params[0];
    else
        token = operation.getTrailing();

    if (token.empty())
        token = client->getNickname();

    client->sendMessage("PONG :" + token);
}
