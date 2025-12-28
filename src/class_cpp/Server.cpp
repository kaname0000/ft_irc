/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: okaname <okaname@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:25:11 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/28 16:43:19 by okaname          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"

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

    for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        delete it->second;
    }

    for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
        delete it->second;
    }
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

    if (fcntl(_listen_fd, F_SETFL, O_NONBLOCK) == -1)
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

void Server::updatePollEvents(size_t index, short events)
{
    if (index < _pfds.size())
        _pfds[index].events = events;
}

void Server::refreshPollEvents()
{
    for (size_t i = 0; i < _pfds.size(); ++i)
    {
        if (_pfds[i].fd == _listen_fd)
        {
            _pfds[i].events = POLLIN;
            continue;
        }
        std::map<int, Client *>::iterator it = _clients.find(_pfds[i].fd);
        short events = POLLIN;
        if (it != _clients.end() && it->second->hasPendingSend())
            events |= POLLOUT;
        _pfds[i].events = events;
    }
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

        if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
        {
            close(client_fd);
            throw std::runtime_error(std::string("fcntl() failed on client: ") + std::strerror(errno));
        }

        Client *client = new Client(client_fd);
        _clients[client_fd] = client;
        addPollFd(client_fd);
        std::cout << "Accepted new client fd=" << client_fd << std::endl;
    }
}

bool Server::handleClientData(size_t index)
{
    if (index >= _pfds.size())
        return false;

    int fd = _pfds[index].fd;
    std::map<int, Client *>::iterator it = _clients.find(fd);
    if (it == _clients.end())
        return false;

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
        Operation empty_op("");
        quit(client, empty_op, this);

        _clients.erase(fd);
        close(fd);
        delete client;
        removePollFd(index);
        return true;
    }
    return false;
}

void Server::run()
{
    while (g_running)
    {
        refreshPollEvents();
        int ret = poll(&_pfds[0], _pfds.size(), -1);
        if (ret < 0)
        {
            if (errno == EINTR)
                continue;
            throw std::runtime_error(std::string("poll() failed: ") + std::strerror(errno));
        }

        for (size_t i = 0; i < _pfds.size(); ++i)
        {
            if (_pfds[i].fd == _listen_fd)
            {
                if (_pfds[i].revents & POLLIN)
                    acceptNewClient();
                continue;
            }
            if (!_pfds[i].revents)
                continue;

            bool removed = false;
            if (_pfds[i].revents & (POLLIN | POLLERR | POLLHUP))
            {
                if (handleClientData(i))
                {
                    removed = true;
                    --i;
                }
            }
            if (!removed && (_pfds[i].revents & POLLOUT))
            {
                std::map<int, Client *>::iterator it = _clients.find(_pfds[i].fd);
                if (it != _clients.end() && it->second->flushSend())
                    updatePollEvents(i, POLLIN);
            }
        }
    }
}

void Server::handleClientMessage(Client *client, const std::string &msg)
{
    if (msg.empty()) return;

    Operation operation(msg);

    std::cout << "From " << client->getFd() << " " << msg << std::endl;
    CommandFunc command = operation.getCommandFunc();

    if (command) {
        command(client, operation, this);
    } else {
        client->sendMessage(std::string("421 * :Unknown command"));
    }
}

Channel* Server::createChannel(const std::string& name) {
    Channel* new_chan = new Channel(name);
    _channels[name] = new_chan;
    std::cout << "Channel created: " << name << std::endl;
    return new_chan;
}

void Server::removeChannel(const std::string& name) {
    std::map<std::string, Channel*>::iterator it = _channels.find(name);
    if (it != _channels.end()) {
        std::cout << "Channel deleted: " << name << std::endl;
        delete it->second;
        _channels.erase(it);
    }
}
