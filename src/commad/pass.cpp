#include "../../includes/class_hpp/Server.hpp"
#include "../../includes/class_hpp/Client.hpp"
#include "../../includes/class_hpp/Channel.hpp"
#include "../../includes/class_hpp/Operation.hpp"
#include "../../includes/utils.hpp"

// 初期接続時に呼ばれるからここで実装する必要ない
// PASS password

void pass(Client *client, Operation &operation, Server *server)
{

    std::vector<std::string> param = operation.getParameter();

    // disallow mult parameter or no parameters

    // std::cout << "pass func\n";
    if (param.size() != 1)
    {
        client->sendMessage(":server.example.com 461 PASS :Not enough parameters");
        std::cout << "error\n";
        return;
    }

    if (param[0] != server->getPassword())
    {
        client->sendMessage(":server.example.com 464 * :Password incorrect");
        std::cout << "error\n";
        return;
    }
    else
    {
        client->setAuthenticated(true);
        // std::cout << "correct pass\n";
        return;
    }
}
