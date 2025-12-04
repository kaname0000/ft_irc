#include "Client.hpp"

const std::string &Client::getNickname() const { return _nickname; }

void Client::setNickname(const std::string &nick) { _nickname = nick; }
