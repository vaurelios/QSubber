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

#include "xmlrpc.hh"

namespace QSubber
{
    HttpTransport::HttpTransport(QString ua)
        : manager(this)
    {
        useragent = ua;
    }

    QString HttpTransport::getUserAgent()
    {
        return useragent;
    }

    void HttpTransport::setUserAgent(QString ua)
    {
        useragent = ua;
    }

    void HttpTransport::get(QUrl url)
    {
        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, useragent);

        reply = manager.get(request);

        connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransport::on_reply_error);
        connect(reply, &QNetworkReply::downloadProgress, this, &HttpTransport::on_reply_downloadProgress);
    }

    void HttpTransport::post(QUrl url, QByteArray data)
    {
        QNetworkRequest request;
        request.setUrl(url);
        request.setHeader(QNetworkRequest::UserAgentHeader, useragent);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");

        reply = manager.post(request, data);

        connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error),
            this, &HttpTransport::on_reply_error);
        connect(reply, &QNetworkReply::readyRead, this, &HttpTransport::on_reply_readyRead);
        connect(reply, &QNetworkReply::downloadProgress, this, &HttpTransport::on_reply_downloadProgress);
    }

    void HttpTransport::on_reply_error(QNetworkReply::NetworkError code)
    {
        qWarning() << "HttpTransport Error:" << reply->errorString();

        reply->deleteLater();
    }

    void HttpTransport::on_reply_readyRead()
    {
        emit dataReady(reply->readAll());
    }

    void HttpTransport::on_reply_downloadProgress(qint64 total, qint64 done)
    {
        qDebug() << QString("Progress: %1 of %2 bytes...").arg(done).arg(total);
    }

    void HttpTransport::on_manager_finished(QNetworkReply* reply)
    {
        reply->deleteLater();
    }

    XmlReply::XmlReply() {}

    QVariant XmlReply::retrieveValue(QVariant cur, QStringList key)
    {
        bool isNumber = false;
        int number = key.first().toInt(&isNumber);

        if (isNumber)
        {
            if (number >= cur.toList().size())
            {
                qWarning() << "Invalid index in path:" << number;

                return QVariant();
            }

            if (key.size() == 1) return cur.toList().at(number);

            key.removeFirst();
            return retrieveValue(cur.toList().at(number), key);
        }
        else {
            if (!cur.toMap().contains(key.first()))
            {
                qWarning() << "Invalid key in path:" << key.first().toUtf8().data();

                return QVariant();
            }

            QVariant val = cur.toMap().value(key.first());

            if (key.size() == 1) return val;

            key.removeFirst();
            return retrieveValue(val, key);
        }
    }

    QVariant XmlReply::get(QString key)
    {
        QStringList dots = key.split(".");

        return retrieveValue(data, dots);
    }

    void XmlReply::set(QString key, QVariant value)
    {
        Q_ASSERT(!value.isNull());

        QStringList dots = key.split(".");

        if (dots.size() == 1)
        {
            data[key] = value;
            return;
        }

        QVariantMap parent = data;
        for (int i = 0; i < dots.size(); ++i)
        {
            if (i != dots.size() && parent.value(dots.at(i)).type() != QVariant::Map)
            {
                parent[dots.at(i)] = QVariantMap();
                continue;
            }

            if (i == dots.size())
            {
                parent[dots.at(i)] = value;
            }

            parent = qvariant_cast<QVariantMap>(parent.value(dots.at(i)));
        }
    }

    XmlRPC::XmlRPC(QString url, QString ua)
        : transport(ua)
    {
        this->url = url;

        connect(&transport, &HttpTransport::dataReady, this, &XmlRPC::proccessReply);
    }

    void XmlRPC::writeArray(QDomElement& value, const QVariantList& list)
    {
        QDomDocument doc = value.ownerDocument();

        QDomElement array = doc.createElement("array");
        value.appendChild(array);
        QDomElement data = doc.createElement("data");
        array.appendChild(data);

        for (int i = 0; i < list.size(); ++i)
        {
            QDomElement item = doc.createElement("value");
            data.appendChild(item);

            if (list.at(i).type() == QVariant::Map)
                writeStruct(item, list.at(i).toMap());
            if (list.at(i).type() == QVariant::List)
                writeArray(item, list.at(i).toList());
            if (list.at(i).type() == QVariant::String)
            {
                QDomElement str = doc.createElement("string");
                item.appendChild(str);
                str.appendChild(doc.createTextNode(list.at(i).toString()));
            }
        }
    }

    void XmlRPC::writeStruct(QDomElement& value, const QVariantMap& map)
    {
        QDomDocument doc = value.ownerDocument();

        QDomElement stru = doc.createElement("struct");
        value.appendChild(stru);

        QVariantMap::const_iterator i = map.constBegin();
        for (; i != map.constEnd(); ++i)
        {
            QDomElement member = doc.createElement("member");
            stru.appendChild(member);

            QDomElement name = doc.createElement("name");
            member.appendChild(name);
            name.appendChild(doc.createTextNode(i.key()));

            QDomElement item = doc.createElement("value");
            member.appendChild(item);

            if (i.value().type() == QVariant::Map)
                writeStruct(item, i.value().toMap());
            if (i.value().type() == QVariant::List)
                writeArray(item, i.value().toList());
            if (i.value().type() == QVariant::String)
            {
                QDomElement str = doc.createElement("string");
                item.appendChild(str);
                str.appendChild(doc.createTextNode(i.value().toString()));
            }
            if (i.value().type() == QVariant::Double)
            {
                QDomElement dbl = doc.createElement("double");
                item.appendChild(dbl);
                dbl.appendChild(doc.createTextNode(i.value().toString()));
            }
        }
    }

    void XmlRPC::Call(QString method, QVariantList args)
    {
        QDomDocument doc("xml");

        QDomElement mCall = doc.createElement("methodCall");
        doc.appendChild(mCall);

        QDomElement mName = doc.createElement("methodName");
        mName.appendChild(doc.createTextNode(method));
        mCall.appendChild(mName);

        QDomElement params = doc.createElement("params");
        mCall.appendChild(params);

        for (int i = 0; i < args.size(); ++i)
        {
            QDomElement param = doc.createElement("param");
            params.appendChild(param);
            QDomElement value = doc.createElement("value");
            param.appendChild(value);

            QVariant arg = args.at(i);

            if (arg.type() == QVariant::String)
            {
                QDomElement str = doc.createElement("string");
                value.appendChild(str);
                str.appendChild(doc.createTextNode(arg.toString()));
            }
            if (arg.type() == QVariant::Map)
                writeStruct(value, arg.toMap());
            if (arg.type() == QVariant::List)
                writeArray(value, arg.toList());
        }

        QByteArray xml = doc.toByteArray();

        transport.post(url, xml);
    }

    void XmlRPC::readParams(const QDomElement& params, XmlReply& reply)
    {
        QVariantList lParams;

        QDomElement param = params.firstChildElement("param");

        while (!param.isNull())
        {
            QDomElement content = param.firstChildElement("value").firstChildElement();

            if (content.tagName() == "string") lParams.append(readString(content));
            if (content.tagName() == "array")  lParams.append(readArray(content));
            if (content.tagName() == "struct") lParams.append(readStruct(content));

            param = param.nextSiblingElement("param");
        }

        reply.set("params", lParams);
    }

    QString XmlRPC::readString(const QDomElement& content)
    {
        return content.text();
    }

    double XmlRPC::readDouble(const QDomElement& content)
    {
        bool ok = false;
        double ret = content.text().toDouble(&ok);

        if (!ok) qWarning() << "Failed to convert value from string to double!";

        return ret;
    }

    QVariantList XmlRPC::readArray(const QDomElement& content, QVariantList parent)
    {
        QDomElement data = content.firstChildElement("data");

        QDomElement value = data.firstChildElement("value");
        while (!value.isNull())
        {
            QDomElement elem = value.firstChildElement();

            if (elem.tagName() == "string")
                parent.append(readString(elem));
            else if (elem.tagName() == "double")
                parent.append(readDouble(elem));
            else if (elem.tagName() == "array")
                parent.insert(parent.size(), readArray(elem));
            else if (elem.tagName() == "struct")
                parent.append(readStruct(elem));

            value = value.nextSiblingElement("value");
        }

        return parent;
    }

    QVariantMap XmlRPC::readStruct(const QDomElement& content, QVariantMap parent)
    {
        QDomElement member = content.firstChildElement("member");

        while (!member.isNull())
        {
            QString key = member.firstChildElement("name").text();
            QDomElement value = member.firstChildElement("value").firstChildElement();

            if (value.tagName() == "string")
                parent[key] = readString(value);
            if (value.tagName() == "double")
                parent[key] = readDouble(value);
            if (value.tagName() == "struct")
                parent[key] = readStruct(value);
            if (value.tagName() == "array")
                parent[key] = readArray(value);

            member = member.nextSiblingElement("member");
        }

        return parent;
    }

    void XmlRPC::proccessReply(QByteArray data)
    {
        XmlReply reply;

        QDomDocument doc;
        doc.setContent(data);

        QDomElement root = doc.documentElement();
        QDomElement params = root.firstChildElement();

        if (params.tagName() == "params")
            readParams(params, reply);

        emit readyReply(reply);
    }
}
