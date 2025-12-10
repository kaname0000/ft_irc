#include "../../includes/class/Server.hpp"
#include "../../includes/class/Client.hpp"
#include "../../includes/class/Channel.hpp"
#include "../../includes/class/Operation.hpp"

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
    std::map<int, Client *>::iterator it = _clients.begin();
    for (; it != _clients.end(); ++it)
    {
        if (it->first != -1)
            close(it->first);
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
        _clients[client_fd] = client;
        addPollFd(client_fd);
        std::cout << "Accepted new client fd=" << client_fd << std::endl;
    }
}

void Server::handleClientData(size_t index)
{
    if (index >= _pfds.size())
        return;

    int fd = _pfds[index].fd;
    std::map<int, Client *>::iterator it = _clients.find(fd);
    if (it == _clients.end())
        return;

    Client *client = it->second;
    char buf[4096];
    bool close_client = false;

    while (true)
    {
        ssize_t n = recv(client->getFd(), buf, sizeof(buf) - 1, 0);
        if (n <= 0)
        {
            return;
        }
        buf[n] = '\0';
        std::string line(buf);
        if (!client->isAuthenticated())
        {
            if (line.substr(0, 5) == "PASS ")
            {
                std::string received_pass = line.substr(5);
                received_pass.erase(received_pass.find_first_of("\r\n")); // 改行削除
                if (received_pass == _password)
                {
                    client->setAuthenticated(true);
                    std::cout << "Client authenticated fd=" << client->getFd() << std::endl;
                }
                else
                {
                    std::cout << "Client failed PASS\n";
                }
            }
            else
                continue;
        }
    }

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
        _clients.erase(fd);
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

    std::cout << operation.getCommand();
    CommandFunc command = operation.getCommandFunc();
    if (command)
        command(client, operation, this);
}

void Server::handleJoin(Client* client, const std::vector<std::string>& params) {
    if (!client->isRegistered()) {
        client->sendReply("451 * :You have not registered");
        return;
    }
    if (params.empty() || params[0].empty()) {
        client->sendReply("461 JOIN :Not enough parameters");
        return;
    }

    std::string channel_name = params[0];
    std::string channel_key = (params.size() > 1) ? params[1] : "";

    if (!isValidChannelName(channel_name)) {
        client->sendReply("403 " + channel_name + " :No such channel");
        return;
    }

    Channel* channel = getChannel(channel_name);

    if (channel == NULL) {
        try {
            channel = createChannel(channel_name);
            channel->addOperator(client);
            channel->setMode('t', true);
        } catch (const std::exception& e) {
            std::cerr << "Channel creation failed: " << e.what() << std::endl;
            return;
        }
    } else {
        if (channel->isMember(client->getFd())) {
            return;
        }
        if (channel->getMode('k') && channel->getKey() != channel_key) {
            client->sendReply("475 " + client->getNickname() + " " + channel_name + " :Cannot join channel (+k)");
            return;
        }
        if (channel->getMode('l') && channel->getMemberCount() >= channel->getLimit()) {
            client->sendReply("471 " + client->getNickname() + " " + channel_name + " :Cannot join channel (+l)");
            return;
        }
        if (channel->getMode('i') && !channel->isInvited(client->getFd())) {
             client->sendReply("473 " + client->getNickname() + " " + channel_name + " :Cannot join channel (+i)");
             return;
        }
        if (channel->isInvited(client->getFd())) {
             channel->removeInvite(client->getFd());
        }
    }
    channel->addMember(client);
    std::string join_msg = ":" + client->getNickname() + "!" + client->getUsername() + "@hostname JOIN :" + channel_name;
    channel->broadcast(join_msg);
    if (!channel->getTopic().empty()) {
        client->sendReply("332 " + client->getNickname() + " " + channel_name + " :" + channel->getTopic());
    } else {
        client->sendReply("331 " + client->getNickname() + " " + channel_name + " :No topic is set");
    }
    channel->sendNamesReply(client);
}
