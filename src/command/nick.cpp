#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"
#include "../../includes/utils.hpp"

// TODO: 各コマンドの実装は後で埋める。現在はサーバー起動確認用のスタブ。
void join(Client *, Operation &, Server *) {}
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

    if (param.size() != 1) {
        client->sendMessage(":server 431 " + client->getNickname() + " :No nickname given");
        return;
    }

    std::string newNickname = param[0];
    std::string oldNickname = client->getNickname();

    if (!isValidName(newNickname, "-_[]\\`^{}", 9)) {
        client->sendMessage(":server 432 " + oldNickname + " " + newNickname + " :Erroneous nickname");
        return;
    }

    std::string msg = ":" + client->getNickname() + " NICK :" + newNickname;
    client->sendMessage(msg);

    client->setNickname(newNickname);
    client->setRegisteredNickname(true);

}
