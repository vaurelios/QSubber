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
#include "subdownloader.hh"

namespace QSubber
{
    SubDownloader::SubDownloader(QObject *parent) : QObject(parent)
      , manager(this)
    {
        request.setRawHeader("User-Agent", USER_AGENT);
    }

    void SubDownloader::Download(QUrl url, QString destfile, qint64 filesize)
    {
        request.setUrl(url);
        this->currentSize = filesize;
        this->currentFile = destfile;

        reply = manager.get(request);

        QObject::connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
                         this, &SubDownloader::replyError);
        QObject::connect(reply, &QNetworkReply::downloadProgress, this, &SubDownloader::downloadProgress);
        QObject::connect(reply, &QNetworkReply::finished, this, &SubDownloader::replyFinished);
    }

    /* Slots */
    void SubDownloader::replyError(QNetworkReply::NetworkError err)
    {
        qDebug() << "SubDownloader Error:" << reply->errorString();

        reply->deleteLater();
    }

    void SubDownloader::downloadProgress(qint64 br, qint64 bt)
    {
        Application* app = static_cast<Application*>(qApp);
        int timeout = 0;

        if (bt != Q_INT64_C(-1))
            this->currentSize = bt;

        QString status;
        if (this->currentSize > Q_INT64_C(0)) {
            qint64 progress = (br / this->currentSize) * 100;
            if(progress == Q_INT64_C(100)) timeout = 1500;

            status = QString("Downloading... %0%").arg(progress);
        }
        else {
            status = QString("Downloading... %0 bytes").arg(br);
        }

        app->updateStatus(status, timeout);
    }

    void SubDownloader::replyFinished()
    {
        QByteArray subfile = reply->readAll();

        QFile outfile(this->currentFile);
        if(outfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            outfile.write(subfile);
            outfile.close();
        }
        else {
            qDebug() << "Failed to open " << this->currentFile << ":" << outfile.error();
        }
    }
}
