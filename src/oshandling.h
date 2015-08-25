/*
 * This file is part of QSubber.
 *
 * QSubber is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QSubber is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QSubber.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef OSHANDLING_H
#define OSHANDLING_H

#include <xmlrpc-c/client_simple.hpp>
#include <thread>
#include <QObject>
#include "config.h"
#include "subdata.h"

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
    void fetchSubLanguages(QString locale = "");

    // --- thread functions
    void doLogIn(const char *username, const char *password);
    void doSearch(std::string hash, std::string movie_name, std::string movie_season, std::string movie_episode);
    void doFetchSubLanguages(QString locale);
};

#endif // OSHANDLING_H
