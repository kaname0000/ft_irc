
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

    // Member Management
    void addMember(Client *client);
    void removeMember(int fd);
    // ... その他 isMember, isOperator ...

    // Mode Management (MODEコマンド処理)
    bool setMode(char mode, bool enable);
    bool getMode(char mode) const;

    // Broadcasting (メッセージ転送)
    void broadcast(const std::string &msg, int exclude_fd = -1);
};

#endif
