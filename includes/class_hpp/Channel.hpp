
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

    void addMember(Client *client);
    void removeMember(int fd);

    bool isMember(int fd) const;
    bool isOperator(int fd) const;
    void addOperator(Client *client);

    bool isInvited(int fd) const;
    void removeInvite(int fd);

    bool setMode(char mode, bool enable);
    bool getMode(char mode) const;

    void broadcast(const std::string &msg, int exclude_fd = -1);
    void sendNamesReply(Client* client) const; // RPL_NAMEREPLY (353) を送信
};

#endif
