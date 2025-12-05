#include "Operation.hpp"
#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"
#include "../utils.hpp"

// 初期接続時に呼ばれるからここで実装する必要ない
// PASS password

void pass(Client *client, Operation &operation, Server *server)
{
    (void)server;

    std::vector<std::string> param = operation.getParameter();

    // disallow mult parameter or no parameters
    if (param.size() != 1)
        return;

    std::string newNickname = param[0];
}