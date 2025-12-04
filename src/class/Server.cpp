#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Operation.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <stdexcept>

Server::Server(int port, const std::string &password)
    : _clients(), _channels(), _listen_fd(-1), _port(port), _password(password), _pfds()
{
    initSocket();
}

Server::~Server()
{
    if (_listen_fd != -1)
        close(_listen_fd);
}

int Server::getListenFd() const { return _listen_fd; }
std::string Server::getPassword() const { return _password; }
const std::vector<struct pollfd> &Server::getPollFds() const { return _pfds; }

std::map<int, Client *> Server::getClients() const { return _clients; }
Client *Server::getClient(const int fd) const
{

    std::map<int, Client *>::const_iterator it = _clients.find(fd);
    if (it != _clients.end())
    {
        return it->second;
    }

    return NULL;
}

Client *Server::getClient(const std::string &target) const
{
    for (std::map<int, Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
    {

        Client *client_ptr = it->second;

        if (client_ptr != NULL && client_ptr->getNickname() == target)
        {
            return client_ptr;
        }
    }

    return NULL;
}

std::map<std::string, Channel *> Server::getChannels() const { return _channels; }
Channel *Server::getChannel(const std::string &target) const
{
    std::map<std::string, Channel *>::const_iterator it = _channels.find(target);
    if (it != _channels.end())
    {
        return it->second;
    }

    return NULL;
}

void Server::initSocket()
{
    _listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_listen_fd == -1)
        throw std::runtime_error(std::string("socket() failed: ") + std::strerror(errno));

    int opt = 1;
    if (setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        close(_listen_fd);
        throw std::runtime_error(std::string("setsockopt() failed: ") + std::strerror(errno));
    }

    int flags = fcntl(_listen_fd, F_GETFL, 0);
    if (flags == -1 || fcntl(_listen_fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        close(_listen_fd);
        throw std::runtime_error(std::string("fcntl() failed: ") + std::strerror(errno));
    }

    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(_port);

    if (bind(_listen_fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1)
    {
        close(_listen_fd);
        throw std::runtime_error(std::string("bind() failed: ") + std::strerror(errno));
    }

    if (listen(_listen_fd, SOMAXCONN) == -1)
    {
        close(_listen_fd);
        throw std::runtime_error(std::string("listen() failed: ") + std::strerror(errno));
    }

    struct pollfd pfd;
    pfd.fd = _listen_fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pfds.push_back(pfd);
}

void Server::handleClientMessage(Client *client, const std::string &msg)
{
    Operation operation(msg);

    CommandFunc command = operation.getCommandFunc();
    if (command)
        command(client, operation, this);
}
