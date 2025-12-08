#include "../../includes/class/Client.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>

Client::Client(int fd)
    : _fd(fd), _nickname(""), _username(""), _auth_status(false), _is_registered(false), _receive_buffer("")
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
        return "";
    std::string cmd = _receive_buffer.substr(0, pos);
    _receive_buffer.erase(0, pos + 2);
    return cmd;
}

void Client::sendMessage(const std::string &msg)
{
    std::string out = msg;
    if (out.size() < 2 || out.substr(out.size() - 2) != "\r\n")
        out += "\r\n";

    size_t sent = 0;
    while (sent < out.size())
    {
        ssize_t n = ::send(_fd, out.c_str() + sent, out.size() - sent, 0);
        if (n > 0)
        {
            sent += static_cast<size_t>(n);
            continue;
        }
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // ノンブロッキングで送れなかった分は現状破棄。送信キューを実装する場合はここで蓄積する。
            break;
        }
        std::cerr << "send() failed on fd=" << _fd << " : " << std::strerror(errno) << std::endl;
        break;
    }
}
