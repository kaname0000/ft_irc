
#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <vector>
#include <string>

class Client;
class Operation;
class Server;

void nick(Client *client, Operation &operation, Server *server);
void user(Client *client, Operation &operation, Server *server);
void pass(Client *client, Operation &operation, Server *server);
void join(Client *client, Operation &operation, Server *server);
void part(Client *client, Operation &operation, Server *server);
void privmsg(Client *client, Operation &operation, Server *server);
void notice(Client *client, Operation &operation, Server *server);
void quit(Client *client, Operation &operation, Server *server);
void kick(Client *client, Operation &operation, Server *server);
void invite(Client *client, Operation &operation, Server *server);
void topic(Client *client, Operation &operation, Server *server);
void mode(Client *client, Operation &operation, Server *server);

#endif
