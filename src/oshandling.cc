#include "oshandling.h"

OSHandling::OSHandling()
{

}

void OSHandling::doLogIn(string &username, string &password)
{
    //client.call(RPC_URL, "LogIn", "ssss", &result, username, password, "eng", USER_AGENT);
}

void OSHandling::LogIn(string username, string password)
{
    thread thLogin(&OSHandling::doLogIn, this, std::ref(username), std::ref(password));
}
