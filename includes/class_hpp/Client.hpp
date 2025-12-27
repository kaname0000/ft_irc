/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:24:17 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:24:30 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
    std::string _hostname;
    std::string _servername;
    std::string _realname;
    bool _auth_status;
    bool _is_registered_nickname;
    bool _is_registered_username;
    bool _is_registered;
    std::string _receive_buffer;
    std::string _send_buffer;

    Client();
    Client(const Client &other);
    Client &operator=(const Client &other);

public:
    Client(int fd);
    ~Client();

    int getFd() const;
    const std::string &getNickname() const;
    const std::string &getUsername() const;
    const std::string &getHostname() const;
    std::string getClientdata() const;
    bool isAuthenticated() const;
    bool isRegisteredNickname() const;
    bool isRegisteredUsername() const;
    bool isRegistered() const;

    void setNickname(const std::string &nick);
    void setUsername(const std::string &user);
    void setHostname(const std::string &host);
    void setServername(const std::string &server);
    void setRealname(const std::string &real);
    void setAuthenticated(bool status);
    void setRegisteredNickname(bool status);
    void setRegisteredUsername(bool status);
    void setRegistered(bool status);
    void sendMessage(const std::string &msg);
    bool hasPendingSend() const;
    void queueMessage(const std::string &msg);
    bool flushSend();

    void appendReceiveBuffer(const std::string &data);
    std::string extractCommand();
};

#endif
