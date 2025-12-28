/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: okaname <okaname@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:24:59 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/28 16:45:58 by okaname          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/class_hpp/Client.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>

Client::Client(int fd)
    : _fd(fd),
      _nickname(""), _username(""), _auth_status(false), _is_registered_nickname(false), _is_registered_username(false), _is_registered(false), _receive_buffer(""), _send_buffer("")
{
}

Client::~Client()
{
}

int Client::getFd() const { return _fd; }
const std::string &Client::getNickname() const { return _nickname; }
const std::string &Client::getUsername() const { return _username; }
const std::string &Client::getHostname() const { return _hostname; }
std::string Client::getClientdata() const { return (":" + _nickname + "!" + _username + "@" + _hostname); }

bool Client::isAuthenticated() const { return _auth_status; }
bool Client::isRegisteredNickname() const { return _is_registered_nickname; }
bool Client::isRegisteredUsername() const { return _is_registered_username; }
bool Client::isRegistered() const { return _is_registered; }

void Client::setNickname(const std::string &nick) { _nickname = nick; }
void Client::setUsername(const std::string &user) { _username = user; }
void Client::setHostname(const std::string &host) { _hostname = host; }
void Client::setServername(const std::string &server) { _servername = server; }
void Client::setRealname(const std::string &real) { _realname = real; }
void Client::setAuthenticated(bool status) { _auth_status = status; }
void Client::setRegisteredNickname(bool status) { _is_registered_nickname = status; }
void Client::setRegisteredUsername(bool status) { _is_registered_username = status; }
void Client::setRegistered(bool status) { _is_registered = status; }

void Client::appendReceiveBuffer(const std::string &data)
{
    _receive_buffer += data;
}

std::string Client::extractCommand()
{
    std::string::size_type pos = _receive_buffer.find("\n");
    if (pos == std::string::npos)
        return "";

    std::string cmd = _receive_buffer.substr(0, pos);
    if (!cmd.empty() && cmd[cmd.size() - 1] == '\r')
        cmd.erase(cmd.size() - 1);

    _receive_buffer.erase(0, pos + 1);
    std::cout << cmd << std::endl;
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
        ssize_t n = ::send(_fd, _send_buffer.c_str(), _send_buffer.size(), MSG_NOSIGNAL);
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
}
