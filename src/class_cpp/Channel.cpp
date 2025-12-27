/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:24:54 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:24:55 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Client.hpp"

Channel::Channel(const std::string &name)
    : _name(name), _topic(""), _key(""), _limit(-1)
{
    _mode_flags['i'] = false;
    _mode_flags['t'] = true;
    _mode_flags['k'] = false;
    _mode_flags['o'] = false;
    _mode_flags['l'] = false;
}

Channel::~Channel()
{
    _members.clear();
    _operators.clear();
    _invited_fds.clear();
}

const std::string &Channel::getName() const { return _name; }
const std::string &Channel::getTopic() const { return _topic; }
const std::string &Channel::getKey() const { return _key; }
int Channel::getLimit() const { return _limit; }
int Channel::getMemberCount() const { return _members.size(); }

void Channel::setTopic(const std::string& topic) { _topic = topic; }
void Channel::setKey(const std::string& key) { _key = key; }
void Channel::setLimit(int limit) { _limit = limit; }

void Channel::addMember(Client *client)
{
    if (client && _members.find(client->getFd()) == _members.end())
        _members[client->getFd()] = client;
}
void Channel::removeMember(int fd)
{
    _members.erase(fd);
    _operators.erase(fd);
    _invited_fds.erase(fd);
}
bool Channel::isMember(int fd) const { return _members.count(fd) > 0; }

void Channel::addOperator(Client *client)
{
    if (client && _operators.find(client->getFd()) == _operators.end())
        _operators[client->getFd()] = client;
}
void Channel::removeOperator(int fd) { _operators.erase(fd); }
bool Channel::isOperator(int fd) const { return _operators.count(fd) > 0; }

void Channel::addInvite(int fd) { _invited_fds.insert(fd); }
bool Channel::isInvited(int fd) const { return _invited_fds.count(fd) > 0; }
void Channel::removeInvite(int fd) { _invited_fds.erase(fd); }

bool Channel::getMode(char mode) const
{
    std::map<char, bool>::const_iterator it = _mode_flags.find(mode);
    return (it != _mode_flags.end()) ? it->second : false;
}

bool Channel::setMode(char mode, bool enable)
{
    if (_mode_flags.count(mode))
    {
        _mode_flags[mode] = enable;
        return true;
    }
    return false;
}

void Channel::broadcast(const std::string &msg, int exclude_fd)
{
    for (std::map<int, Client *>::iterator it = _members.begin(); it != _members.end(); ++it)
    {
        Client *member = it->second;
        if (member->getFd() != exclude_fd)
        {
            member->sendMessage(msg);
        }
    }
}

void Channel::sendNamesReply(Client *client) const
{
    std::string names_list;

    for (std::map<int, Client *>::const_iterator it = _members.begin(); it != _members.end(); ++it)
    {
        Client *member = it->second;
        if (isOperator(member->getFd()))
        {
            names_list += "@";
        }
        names_list += member->getNickname();
        names_list += " ";
    }

    if (!names_list.empty())
    {
        names_list.erase(names_list.size() - 1);
    }

    std::string reply_353 = "353 " + client->getNickname() + " = " + _name + " :" + names_list;
    client->sendMessage(reply_353);

    std::string reply_366 = "366 " + client->getNickname() + " " + _name + " :End of /NAMES list";
    client->sendMessage(reply_366);
}
