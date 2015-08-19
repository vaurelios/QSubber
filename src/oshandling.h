#ifndef OSHANDLING_H
#define OSHANDLING_H

#include <xmlrpc-c/client_simple.hpp>
#include <thread>
#include <QObject>
#include "config.h"

using namespace std;
using namespace xmlrpc_c;


class OSHandling : public QObject
{
    Q_OBJECT

    typedef map<QString, QString> csdict;
    typedef map<QString, csdict> cdict;

    clientXmlTransport_curl curlTrp;
    client_xml xmlClient;
    carriageParm_curl0 OSCarriageParm;
    QString token;

signals:
    void update_status(QString status, int timeout=0);
    void sublist_updated();
    void clear_list();

public:
    OSHandling();
    bool isLoggedIn();
    void LogIn(const char *username, const char *password);
    void HashSearch(QString hash);
    void FullSearch(QString movie_name, QString movie_season, QString movie_episode);

    // --- thread functions
    void doLogIn(const char *username, const char *password);
    void doSearch(std::string hash, std::string movie_name, std::string movie_season, std::string movie_episode);
};

#endif // OSHANDLING_H
