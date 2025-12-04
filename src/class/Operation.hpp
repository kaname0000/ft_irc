#ifndef OPERATION_HPP
#define OPERATION_HPP

#include <iostream>
#include <vector>
#include <sstream>
#include <string>

struct Data;
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

typedef void (*CommandFunc)(Client *, Operation &, Data &);
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

void nick(Client *, Operation &, Data &);
void user(Client *, Operation &, Data &);
void join(Client *, Operation &, Data &);
void privmsg(Client *, Operation &, Data &);
void part(Client *, Operation &, Data &);
void quit(Client *, Operation &, Data &);
void kick(Client *, Operation &, Data &);
void invite(Client *, Operation &, Data &);
void topic(Client *, Operation &, Data &);
void mode(Client *, Operation &, Data &);

#endif
