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
#include <algorithm>

Server::Server(int port, const std::string &password)
    : data(), _listen_fd(-1), _port(port), _password(password), _pfds(), _client_map()
{
    initSocket();
}

Server::~Server()
{
    if (_listen_fd != -1)
        close(_listen_fd);
    std::map<int, Client *>::iterator it = _client_map.begin();
    for (; it != _client_map.end(); ++it)
    {
        if (it->first != -1)
            close(it->first);
        delete it->second;
    }
}

int Server::getListenFd() const { return _listen_fd; }
const std::vector<struct pollfd> &Server::getPollFds() const { return _pfds; }

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

void Server::addPollFd(int fd)
{
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    _pfds.push_back(pfd);
}

void Server::removePollFd(size_t index)
{
    if (index >= _pfds.size())
        return;
    std::vector<struct pollfd>::iterator it = _pfds.begin() + index;
    _pfds.erase(it);
}

void Server::acceptNewClient()
{
    while (true)
    {
        struct sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);
        int client_fd = accept(_listen_fd, reinterpret_cast<struct sockaddr *>(&addr), &addrlen);
        if (client_fd == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            throw std::runtime_error(std::string("accept() failed: ") + std::strerror(errno));
        }

        int flags = fcntl(client_fd, F_GETFL, 0);
        if (flags == -1 || fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            close(client_fd);
            throw std::runtime_error(std::string("fcntl() failed on client: ") + std::strerror(errno));
        }

        Client *client = new Client(client_fd);
        _client_map[client_fd] = client;
        data._clients.push_back(client);
        addPollFd(client_fd);
        std::cout << "Accepted new client fd=" << client_fd << std::endl;
    }
}

void Server::handleClientData(size_t index)
{
    if (index >= _pfds.size())
        return;

    int fd = _pfds[index].fd;
    std::map<int, Client *>::iterator it = _client_map.find(fd);
    if (it == _client_map.end())
        return;

    Client *client = it->second;
    char buf[4096];
    bool close_client = false;
    while (true)
    {
        ssize_t n = recv(fd, buf, sizeof(buf), 0);
        if (n > 0)
        {
            client->appendReceiveBuffer(std::string(buf, n));
            std::string cmd;
            while (!(cmd = client->extractCommand()).empty())
                handleClientMessage(client, cmd);
            continue;
        }
        if (n == 0)
        {
            std::cout << "Client disconnected fd=" << fd << std::endl;
            close_client = true;
            break;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            break;
        std::cerr << "recv() failed on fd=" << fd << " : " << std::strerror(errno) << std::endl;
        close_client = true;
        break;
    }

    if (_pfds[index].revents & (POLLHUP | POLLERR))
        close_client = true;

    if (close_client)
    {
        data._clients.erase(std::remove(data._clients.begin(), data._clients.end(), client), data._clients.end());
        _client_map.erase(fd);
        close(fd);
        delete client;
        removePollFd(index);
    }
}

void Server::run()
{
    while (true)
    {
        int ret = poll(&_pfds[0], _pfds.size(), -1);
        if (ret < 0)
        {
            if (errno == EINTR)
                continue;
            throw std::runtime_error(std::string("poll() failed: ") + std::strerror(errno));
        }

        for (size_t i = 0; i < _pfds.size(); ++i)
        {
            if (!(_pfds[i].revents & POLLIN))
                continue;
            if (_pfds[i].fd == _listen_fd)
                acceptNewClient();
            else
                handleClientData(i);
        }
    }
}

void Server::handleClientMessage(Client *client, const std::string &msg)
{
    Operation operation(msg);

    CommandFunc command = operation.getCommandFunc();
    if (command)
        command(client, operation, this->data);
}
