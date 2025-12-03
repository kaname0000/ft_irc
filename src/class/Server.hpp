#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <iostream>

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

public:
    Server();
    ~Server();

    void handleClientMessage(Client *client, const std::string &msg);
};

#endif
