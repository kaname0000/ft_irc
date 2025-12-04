
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <ctime>

class Client
{
private:
    int _fd;
    std::string _nickname;
    std::string _username;
    bool _auth_status;
    bool _is_registered;
    std::string _receive_buffer;

    Client();
    Client(const Client &other);
    Client &operator=(const Client &other);

public:
    Client(int fd);
    ~Client();

    int getFd() const;
    const std::string &getNickname() const;
    bool isAuthenticated() const;

    void setNickname(const std::string &nick);
    void setUsername(const std::string &user);
    void setAuthenticated(bool status);
    void setRegistered(bool status);

    // Buffer Operations(カナメさん用)
    void appendReceiveBuffer(const std::string &data);
    std::string extractCommand();

    void sendMessage(const std::string &);
};

#endif
