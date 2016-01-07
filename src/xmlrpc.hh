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

#include <QtCore>
#include <QDomDocument>
#include <QDomElement>
#include <QtNetwork>

#ifndef QSUBBER_XMLRPC
#define QSUBBER_XMLRPC

namespace QSubber
{
    class HttpTransport : public QObject
    {
        Q_OBJECT

        QNetworkAccessManager manager;
        QNetworkReply* reply;
        QString useragent;

    public:
        HttpTransport(QString useragent = QString());

        QString getUserAgent();
        void    setUserAgent(QString useragent);

        void get(QUrl url);
        void post(QUrl url, QByteArray data);

    signals:
        void dataReady(QByteArray data);

    private slots:
        void on_reply_error();
        void on_reply_readyRead();
        void on_reply_downloadProgress(qint64 total, qint64 done);
    };

    class XmlReply
    {
        QVariantMap data;

        QVariant retrieveValue(QVariant cur, QStringList key);

    public:
        XmlReply();

        QVariant get(QString key);
        void set(QString key, QVariant value);
    };

    class XmlRPC : public QObject
    {
        Q_OBJECT

        QString url;
        HttpTransport transport;

        void         writeArray(QDomElement& value, const QVariantList& list);
        void         writeStruct(QDomElement& value, const QVariantMap& map);
        void         readParams(const QDomElement& params, XmlReply& reply);
        QString      readString(const QDomElement& content);
        double       readDouble(const QDomElement& content);
        QVariantList readArray(const QDomElement& content, QVariantList parent = QVariantList());
        QVariantMap  readStruct(const QDomElement& content, QVariantMap parent = QVariantMap());

    public:
        XmlRPC(QString url, QString useragent = QString());

        void Call(QString method, QVariantList args);

    signals:
        void readyReply(XmlReply reply);

    private slots:
        void proccessReply(QByteArray data);
    };
}

#endif /* QSUBBER_XMLRPC */
