/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Operation.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sykawai <sykawai@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/27 15:25:04 by sykawai           #+#    #+#             */
/*   Updated: 2025/12/27 15:25:05 by sykawai          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/class_hpp/Operation.hpp"

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
    pass,
    ping,
    notice,
    cap,
    who,
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
    if (token == "PASS")
        return PASS;
    if (token == "PING")
        return PING;
    if(token == "NOTICE")
        return NOTICE;
    if (token == "CAP")
        return CAP;
    if (token == "WHO" || token == "WHOIS")
        return WHO;
    return UNKNOWN;
}

Operation::Operation(const std::string &message)
    : _command(UNKNOWN), _parameter(), _trailing_parameter("")
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
            if (!rest.empty())
                _trailing_parameter += " " + rest;
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
    if (_command >= NICK && _command <= WHO)
        return COMMANDFUNC[_command];
    return NULL;
}
