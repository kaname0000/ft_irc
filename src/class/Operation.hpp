#ifndef OPERATION_HPP
#define OPERATION_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

class Server;
class Client;
class Operation;

// COMMAND parameter1 parameter2 ... :trailing_parameter

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

#endif
