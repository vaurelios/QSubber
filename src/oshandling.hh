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

#include "config.h"
#include "subdata.hh"
#include "xmlrpc.hh"

#include <QtCore>

#ifndef OSHANDLING_H
#define OSHANDLING_H

using namespace std;

namespace QSubber
{
    class OSHandling : public QObject
    {
        Q_OBJECT

        XmlRPC rpc;
        QString token;

        void queueCall(QString name=QString(), QVariantList args=QVariantList());
        void postLogIn(XmlReply& reply);
        void postSearch(XmlReply& reply);
        void postSubLanguages(XmlReply& reply);

    public:
        OSHandling(QObject* parent=0);
        bool isLoggedIn();
        void LogIn(QString username, QString password);
        void HashSearch(QString hash);
        void FullSearch(QString movie_name, QString movie_season, QString movie_episode);
        void fetchSubLanguages(QString locale = QString());
        void doSearch(QString hash = QString(), QString movie_name = QString(),
                      QString movie_season = QString(), QString movie_episode = QString());

    private slots:
        void finishedCall(XmlReply reply);
    };
}

#endif // OSHANDLING_H
