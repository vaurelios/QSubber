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

#include "oshandling.hh"

#include "application.hh"
#include "utils.hh"

namespace QSubber
{
    OSHandling::OSHandling(QObject *parent) : QObject(parent)
        , rpc(RPC_URL, USER_AGENT)
    {
        connect(&rpc, &XmlRPC::readyReply, this, &OSHandling::finishedCall);
    }

    bool OSHandling::isLoggedIn()
    {
        if (token.isEmpty()) return false;

        return true;
    }

    void OSHandling::queueCall(QString name, QVariantList args)
    {
        typedef struct
        {
            QString name;
            QVariantList args;
            bool working;
        } MethodCall;

        static QQueue<MethodCall> queue;

        if (!name.isNull())
        {
            MethodCall call { name, args, false };
            queue.enqueue(call);

            if (queue.size() == 1) queueCall();
        }
        else {
            if (queue.isEmpty()) return;

            MethodCall* call = &queue.head();

            if (call->working)
            {
                queue.dequeue();
                queueCall();
            }
            else {
                call->working = true;

                rpc.Call(call->name, call->args);

                static_cast<Application*>(qApp)->setCurrentJob(call->name);
            }
        }
    }

    void OSHandling::Search(QVariantMap& params)
    {
        Application* app = static_cast<Application*>(qApp);

        app->updateStatus("Searching...");

        QVariantMap searchData;
        searchData["sublanguageid"] = app->settings->getConfig("current_lang", "eng");
        searchData["moviehash"]     = "";
        searchData["moviebytesize"] = "";
        searchData["imdbid"]        = "";
        searchData["query"]         = "";
        searchData["season"]        = "";
        searchData["episode"]       = "";
        searchData["tag"]           = "";

        QMapIterator<QString, QVariant> param(params);
        while (param.hasNext())
        {
            param.next();

            searchData[param.key()] = param.value();
        }

        QVariantList searches;
        searches.append(searchData);

        QVariantList searchParms;
        searchParms.append(token);
        searchParms.insert(searchParms.size(), searches);

        qDebug() << searchParms;

        queueCall("SearchSubtitles", searchParms);
    }

    void OSHandling::fetchSubLanguages(QString locale)
    {
        Application* app = static_cast<Application*>(qApp);

        if (locale.isEmpty()) locale = app->settings->getConfig("current_locale", "en");

        if (!app->settings->hasLocale(locale))
        {
            QVariantList paramList;

            paramList.append(locale);

            queueCall("GetSubLanguages", paramList);
        }
    }

    void OSHandling::LogIn(QString username, QString password)
    {
        Application* app = static_cast<Application*>(qApp);

        app->updateStatus("Logging in...");

        QVariantList args;

        args << username;
        args << password;
        args << QString("eng");
        args << QString(USER_AGENT);

        queueCall("LogIn", args);
    }

    void OSHandling::postLogIn(XmlReply& reply)
    {
        Application* app = static_cast<Application*>(qApp);

        if (reply.get("params.0.status") == "200 OK")
        {
            qDebug() << "Logged in, token:" << reply.get("params.0.token").toString();

            token = reply.get("params.0.token").toString();

            app->updateStatus("Logged in!!!", 3000);
        }
        else {
            QString status("Failed to login, server error: %0");

            app->updateStatus(status.arg(reply.get("params.0.status").toString()));
        }
    }

    void OSHandling::postSearch(XmlReply& reply)
    {
        Application* app = static_cast<Application*>(qApp);

        if (reply.get("params.0.status") == "200 OK")
        {
            QVariantList data = reply.get("params.0.data").toList();

            if (data.isEmpty())
            {
                app->setSubList(QVariantList());
                app->updateStatus("Searching... done. No Results!", 1500);
                return;
            }

            app->setSubList(data);
        }

        app->updateStatus("Searching... done!", 1500);
    }

    void OSHandling::postSubLanguages(XmlReply& reply)
    {
        Application* app = static_cast<Application*>(qApp);

        QVariantList data = reply.get("params.0.data").toList();

        for (int i = 0; i < data.size(); ++i)
        {
            QVariantMap lang = data.at(i).toMap();
            QString locale = app->settings->getConfig("current_locale", "en");

            app->settings->setLangCode(locale,
                                       lang["SubLanguageID"].toString(),
                                       lang["LanguageName"].toString());
        }
    }

    void OSHandling::finishedCall(XmlReply reply)
    {
        QString job = static_cast<Application*>(qApp)->popCurrentJob();
        queueCall();

        if (job == "LogIn")
        {
            postLogIn(reply);
            qDebug() << reply.get("params.0.status").toString();
        }
        else if (job == "SearchSubtitles")
        {
            postSearch(reply);
        }
        else if (job == "GetSubLanguages")
        {
            postSubLanguages(reply);
        }
    }
}
