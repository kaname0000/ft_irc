#include "../../includes/class/Server.hpp"
#include "../../includes/class/Client.hpp"
#include "../../includes/class/Channel.hpp"
#include "../../includes/class/Operation.hpp"
#include "../../includes/utils.hpp"

// TODO: 各コマンドの実装は後で埋める。現在はサーバー起動確認用のスタブ。
void nick(Client *, Operation &, Server *);
void user(Client *, Operation &, Server *) {}
void join(Client *, Operation &, Server *) {}
void privmsg(Client *, Operation &, Server *) {}
void part(Client *, Operation &, Server *) {}
void quit(Client *, Operation &, Server *) {}
void kick(Client *, Operation &, Server *) {}
void invite(Client *, Operation &, Server *) {}
void topic(Client *, Operation &, Server *) {}
void mode(Client *, Operation &, Server *) {}

// NICK parameter

void nick(Client *client, Operation &operation, Server *server)
{
    (void)server;

    std::vector<std::string> param = operation.getParameter();

    // disallow mult parameter or no parameters
    if (param.size() != 1)
        return;

    std::string newNickname = param[0];

    // check Nickname
    if (!isValidName(newNickname, "-_[]\\`^{}", 9))
        return;

    client->setNickname(operation.getParameter()[0]);
}
