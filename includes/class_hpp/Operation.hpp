/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Operation.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:24:24 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:24:25 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef OPERATION_HPP
#define OPERATION_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

class Server;
class Client;
class Operation;

enum COMMAND
{
    NICK,
    USER,
    JOIN,
    PRIVMSG,
    PART,
    QUIT,
    KICK,
    INVITE,
    TOPIC,
    MODE,
    PASS,
    PING,
    NOTICE,
    CAP,
    WHO,
    UNKNOWN
};

typedef void (*CommandFunc)(Client *, Operation &, Server *);
extern CommandFunc COMMANDFUNC[];

class Operation
{
private:
    COMMAND _command;
    std::vector<std::string> _parameter;
    std::string _trailing_parameter;

public:
    Operation(const std::string &message);
    ~Operation();

    COMMAND getCommand() const;
    const std::vector<std::string> &getParameter() const;
    const std::string &getTrailing() const;
    CommandFunc getCommandFunc() const;
};

void nick(Client *, Operation &, Server *);
void user(Client *, Operation &, Server *);
void join(Client *, Operation &, Server *);
void privmsg(Client *, Operation &, Server *);
void part(Client *, Operation &, Server *);
void quit(Client *, Operation &, Server *);
void kick(Client *, Operation &, Server *);
void invite(Client *, Operation &, Server *);
void topic(Client *, Operation &, Server *);
void mode(Client *, Operation &, Server *);
void pass(Client *, Operation &, Server *);
void ping(Client *, Operation &, Server *);
void notice(Client *, Operation &, Server *);
void cap(Client *, Operation &, Server *);
void who(Client *, Operation &, Server *);

#endif
