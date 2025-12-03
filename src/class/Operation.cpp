#include "Operation.hpp"

CommandFunc COMMANDFUNC[] = {
    nick,
    user,
    join,
    privmsg,
    part,
    quit,
    kick,
    invite,
    topic,
    mode,
};

static COMMAND stringToCommand(const std::string &token)
{
    if (token == "NICK")
        return NICK;
    if (token == "USER")
        return USER;
    if (token == "JOIN")
        return JOIN;
    if (token == "PRIVMSG")
        return PRIVMSG;
    if (token == "PART")
        return PART;
    if (token == "QUIT")
        return QUIT;
    if (token == "KICK")
        return KICK;
    if (token == "INVITE")
        return INVITE;
    if (token == "TOPIC")
        return TOPIC;
    if (token == "MODE")
        return MODE;
    return UNKNOWN;
}

Operation::Operation(const std::string &message)
    : _command(), _parameter(), _trailing_parameter("")
{
    std::stringstream ss(message);
    std::string token;

    if (!(ss >> token))
        return;
    _command = stringToCommand(token);
    while (ss >> token)
    {
        if (!token.empty() && token[0] == ':')
        {
            _trailing_parameter = token.substr(1);
            std::string rest;
            std::getline(ss, rest);
            if (!rest.empty() && rest[0] == ' ')
                rest.erase(0, 1);
            _trailing_parameter += rest;
            return;
        }
        _parameter.push_back(token);
    }
}

Operation::~Operation()
{
}

COMMAND Operation::getCommand() const { return _command; }
const std::vector<std::string> &Operation::getParameter() const { return _parameter; }
const std::string &Operation::getTrailing() const { return _trailing_parameter; }

CommandFunc Operation::getCommandFunc() const
{
    if (_command >= NICK && _command <= MODE)
        return COMMANDFUNC[_command];
    return NULL;
}
