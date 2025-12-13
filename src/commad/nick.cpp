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

    // disallow mult parameter or no parameters
    if (param.size() != 1)
    {
        client->sendMessage(":server 432 * :Erroneous nickname");
        std::cout << "nickname too params\n";
        return;
    }

    std::string newNickname = param[0];

    // check Nickname
    if (!isValidName(newNickname, "-_[]\\`^{}", 9))
    {
        client->sendMessage(":server 432 * :Erroneous nickname");
        std::cout << "nickname invlaid char\n";
        return;
    }

    client->sendMessage(":" + client->getNickname() + " NICK :" + param[0]);
    client->setNickname(param[0]);
    // std::cout << "set new nickname\n";
    client->setRegisteredNickname(true);
}
