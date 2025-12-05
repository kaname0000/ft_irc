#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <poll.h>

class Client;
class Channel;

struct Data
{
    std::vector<Client *> _clients;
    std::vector<Channel *> _channels;
};

class Server
{
private:
    Data data;
    int _listen_fd;
    int _port;
    std::string _password;
    std::vector<struct pollfd> _pfds;
    std::map<int, Client *> _client_map;

    Server();
    Server(const Server &other);
    Server &operator=(const Server &other);

    void initSocket();
    void addPollFd(int fd);
    void removePollFd(size_t index);
    void acceptNewClient();
    void handleClientData(size_t index);

public:
    Server(int port, const std::string &password);
    ~Server();

    int getListenFd() const;
    const std::vector<struct pollfd> &getPollFds() const;

    void run();
    void handleClientMessage(Client *client, const std::string &msg);
};

#endif
