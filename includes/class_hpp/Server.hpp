/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:24:43 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:24:44 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <poll.h>
#include <csignal>

extern volatile sig_atomic_t g_running;

class Client;
class Channel;

class Server
{
private:
    std::map<int, Client *> _clients;
    std::map<std::string, Channel *> _channels;
    int _listen_fd;
    int _port;
    std::string _password;
    std::vector<struct pollfd> _pfds;

    Server();
    Server(const Server &other);
    Server &operator=(const Server &other);

    void initSocket();
    void addPollFd(int fd);
    void removePollFd(size_t index);
    void acceptNewClient();
    bool handleClientData(size_t index);
    void updatePollEvents(size_t index, short events);
    void refreshPollEvents();

public:
    Server(int port, const std::string &password);
    ~Server();

    int getListenFd() const;
    std::string getPassword() const;
    const std::vector<struct pollfd> &getPollFds() const;

    void run();
    std::map<int, Client *> getClients() const;
    Client *getClient(const int) const;
    Client *getClient(const std::string &) const;
    std::map<std::string, Channel *> getChannels() const;
    Channel *getChannel(const std::string &) const;

    void handleClientMessage(Client *client, const std::string &msg);

    void handleJoin(Client* client, const std::vector<std::string>& params);
    void handlePart(Client* client, const std::vector<std::string>& params);

    void handleKick(Client* client, const std::vector<std::string>& params);
    void handleInvite(Client* client, const std::vector<std::string>& params);

    void handleTopic(Client* client, const std::vector<std::string>& params);

    void handleMode(Client* client, const std::vector<std::string>& params);

    void handleQuit(Client *client, const std::vector<std::string> &params);

    Channel* createChannel(const std::string& name);
    void     removeChannel(const std::string& name);

};

#endif
