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


#ifndef SUBDOWNLOADER_H
#define SUBDOWNLOADER_H

#include <QObject>
#include <QtNetwork>

class SubDownloader : public QObject
{
    Q_OBJECT

    QNetworkAccessManager manager;
    QNetworkRequest request;
    QNetworkReply *reply;

    qint64 currentSize;
    QString currentFile;

public:
    SubDownloader(QObject *parent = 0);
    void Download(QUrl url, QString destfile, qint64 filesize = 0);

signals:
    void update_status(QString status, int timeout=0);

public slots:
    void replyError(QNetworkReply::NetworkError err);
    void downloadProgress(qint64 br, qint64 bt);
    void replyFinished();
};

#endif // SUBDOWNLOADER_H
