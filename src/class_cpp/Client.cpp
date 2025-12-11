#include "../../includes/class_hpp/Client.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>

Client::Client(int fd)
    : _fd(fd),
      _nickname(""), _username(""), _auth_status(false), _is_registered(false), _receive_buffer(""), _send_buffer("")
{
}

Client::~Client()
{
}

int Client::getFd() const { return _fd; }
const std::string &Client::getNickname() const { return _nickname; }
bool Client::isAuthenticated() const { return _auth_status; }

void Client::setNickname(const std::string &nick) { _nickname = nick; }
void Client::setUsername(const std::string &user) { _username = user; }
void Client::setHostname(const std::string &host) { _hostname = host; }
void Client::setServername(const std::string &server) { _servername = server; }
void Client::setRealname(const std::string &real) { _realname = real; }
void Client::setAuthenticated(bool status) { _auth_status = status; }
void Client::setRegistered(bool status) { _is_registered = status; }

void Client::appendReceiveBuffer(const std::string &data)
{
    _receive_buffer += data;
}

std::string Client::extractCommand()
{
    std::string::size_type pos = _receive_buffer.find("\r\n");
    if (pos == std::string::npos)
    {
        // std::cout << "npos\n";
        return "";
    }
    std::string cmd = _receive_buffer.substr(0, pos);
    _receive_buffer.erase(0, pos + 2);
    return cmd;
}

bool Client::hasPendingSend() const
{
    return !_send_buffer.empty();
}

void Client::queueMessage(const std::string &msg)
{
    std::string out = msg;
    if (out.size() < 2 || out.substr(out.size() - 2) != "\r\n")
        out += "\r\n";

    _send_buffer += out;
}

bool Client::flushSend()
{
    while (!_send_buffer.empty())
    {
        ssize_t n = ::send(_fd, _send_buffer.c_str(), _send_buffer.size(), 0);
        if (n > 0)
        {
            _send_buffer.erase(0, static_cast<size_t>(n));
            continue;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return false;
        std::cerr << "send() failed on fd=" << _fd << " : " << std::strerror(errno) << std::endl;
        _send_buffer.clear();
        return false;
    }
    return true;
}

void Client::sendMessage(const std::string &msg)
{
    queueMessage(msg);
    flushSend();
}
