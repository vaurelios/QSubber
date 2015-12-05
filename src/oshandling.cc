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


#include "oshandling.h"
#include "globals.h"
#include "utils.h"
#include <iostream>
#include <cassert>
#include <cstring>


/* Global subtitles list */
QList<SubData*> sublist;

OSHandling::OSHandling(QObject *parent) : QObject(parent)
    , curlTrp(clientXmlTransport_curl::constrOpt().timeout(60000).user_agent("QSubber/1.0"))
    , xmlClient(&curlTrp)
    , OSCarriageParm(RPC_URL)
{
    // grow up characters limit
    xmlrpc_limit_set(XMLRPC_XML_SIZE_LIMIT_ID, 10e6);
}

bool OSHandling::isLoggedIn()
{
    if (token.isEmpty()) return false;

    return true;
}

void OSHandling::LogIn(const char *username, const char *password)
{
    std::thread thLogin(&OSHandling::doLogIn, this, username, password);
    thLogin.detach();
}

void OSHandling::HashSearch(QString hash)
{
    std::thread thSearch(&OSHandling::doSearch, this, hash.toUtf8().data(), "", "", "");
    thSearch.detach();
}

void OSHandling::FullSearch(QString movie_name, QString movie_season, QString movie_episode) {
    std::thread thSearch(&OSHandling::doSearch, this, "",
                         movie_name.toUtf8().data(),
                         movie_season.toUtf8().data(),
                         movie_episode.toUtf8().data());
    thSearch.detach();
}

void OSHandling::fetchSubLanguages(QString locale) {
    if (locale.isEmpty()) locale = settings->getConfig("current_locale", "en");

    if (!settings->hasLocale(locale))
        doFetchSubLanguages(locale);
}

void OSHandling::doLogIn(const char *username, const char *password)
{
    emit update_status("Logging in...");

    try {
        paramList loginParms;
        loginParms.add(value_string(username));
        loginParms.add(value_string(password));
        loginParms.add(value_string("eng"));
        loginParms.add(value_string(USER_AGENT));

        rpcPtr login("LogIn", loginParms);

        login->call(&xmlClient, &OSCarriageParm);
        assert(login->isFinished());

        value_struct retval  = login->getResult();
        value_string rtoken  = retval.cvalue().at("token");
        value_string rstatus = retval.cvalue().at("status");

        if (rstatus.cvalue().compare("200 OK") == 0) {
            std::cout << "Token: " << rtoken.cvalue() << std::endl;
            std::cout << "Status: " << rstatus.cvalue() << std::endl;

            token = rtoken.cvalue().c_str();

            emit update_status("Logged in!!!", 3000);
        }
        else {
            QString status("Failed to login, server error: ");
            status.append(rstatus.cvalue().c_str());

            emit update_status(status);
        }
    } catch (girerr::error const e) {
        QString status("Failed to login: ");
        status.append(e.what());

        emit update_status(status);
    }
}

void OSHandling::doSearch(std::string hash, std::string movie_name, std::string movie_season, std::string movie_episode) {
    emit update_status("Searching...");

    cstruct searchData;
    searchData["sublanguageid"] = value_string("pob");
    searchData["moviehash"]     = value_string(hash);
    searchData["query"]         = value_string(movie_name);
    searchData["season"]        = value_string(movie_season);
    searchData["episode"]       = value_string(movie_episode);

    carray searches;
    searches.push_back(value_struct(searchData));

    paramList searchParms;
    searchParms.add(value_string(token.toUtf8().data()));
    searchParms.add(value_array(searches));

    rpcPtr search("SearchSubtitles", searchParms);

    search->call(&xmlClient, &OSCarriageParm);
    assert(search->isFinished());

    value_struct retval = search->getResult();
    value_string status = retval.cvalue().at("status");

    emit clear_list();

    if (retval.cvalue().at("data").type() != value::TYPE_ARRAY) {
        emit update_status("Searching... done. No Results!", 1500);
        return;
    }

    value_array data = retval.cvalue().at("data");

    for(const auto &p : data.cvalue()) {
        SubData* subdata = new SubData(value_struct(p).cvalue());

        sublist << subdata;
    }

    emit update_status("Searching... done!", 1500);
    emit sublist_updated();
}

void OSHandling::doFetchSubLanguages(QString locale) {
    paramList getParms;
    getParms.add(value_string(locale.toUtf8().data()));

    rpcPtr get("GetSubLanguages", getParms);

    get->call(&xmlClient, &OSCarriageParm);
    assert(get->isFinished());

    value_struct retval = get->getResult();
    value_array  data   = retval.cvalue().at("data");

    for(const auto &p : data.cvalue()) {
        QMap<QString, QString> lang = QSubber::cstructToQMap(value_struct(p).cvalue());

        settings->setLangCode(locale, lang["SubLanguageID"], lang["LanguageName"]);
    }
}
