#ifndef OSHANDLING_H
#define OSHANDLING_H

#include <xmlrpc-c/client_simple.hpp>
#include <thread>

#define USER_AGENT "QSubber v1"
#define RPC_URL "http://api.opensubtitles.org/xml-rpc"

using namespace std;


class OSHandling
{
    xmlrpc_c::clientSimple client;
    xmlrpc_c::value result;

public:
    OSHandling();

    // --- thread functions
    void doLogIn(string &username, string &password);

    void LogIn(string username = string(""), string password = string(""));
};

#endif // OSHANDLING_H
