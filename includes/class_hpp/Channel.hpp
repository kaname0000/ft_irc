/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:24:14 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:24:28 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>
#include <set>
#include <vector>

class Client;

class Channel
{
private:
    std::string _name;
    std::string _topic;
    std::string _key;
    int _limit;
    std::map<char, bool> _mode_flags;

    std::map<int, Client *> _members;
    std::map<int, Client *> _operators;
    std::set<int> _invited_fds;

    Channel(const Channel &other);
    Channel &operator=(const Channel &other);
    Channel();

public:
    Channel(const std::string &name);
    ~Channel();

    const std::string& getName() const;
    const std::string& getTopic() const;
    const std::string& getKey() const;
    int getLimit() const;
    int getMemberCount() const;

    void setTopic(const std::string& topic);
    void setKey(const std::string& key);
    void setLimit(int limit);

    void addMember(Client *client);
    void removeMember(int fd);
    bool isMember(int fd) const;

    void addOperator(Client *client);
    void removeOperator(int fd);
    bool isOperator(int fd) const;

    void addInvite(int fd);
    bool isInvited(int fd) const;
    void removeInvite(int fd);

    bool setMode(char mode, bool enable);
    bool getMode(char mode) const;

    void broadcast(const std::string &msg, int exclude_fd = -1);
    void sendNamesReply(Client* client) const;
};

#endif
