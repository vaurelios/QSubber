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

#include "httptransport.hh"

namespace QSubber
{
HttpTransport::HttpTransport(QString ua)
  : manager(this)
{
  useragent = ua;

  connect(&this->manager, &QNetworkAccessManager::finished, this, &HttpTransport::on_manager_finished);
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

  auto reply = manager.get(request);

  connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred), this,
          &HttpTransport::on_reply_error);
  connect(reply, &QNetworkReply::downloadProgress, this, &HttpTransport::on_reply_downloadProgress);
}

void HttpTransport::post(QUrl url, QByteArray data)
{
  QNetworkRequest request;
  request.setUrl(url);
  request.setHeader(QNetworkRequest::UserAgentHeader, useragent);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

  auto reply = manager.post(request, data);

  connect(reply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred), this,
          &HttpTransport::on_reply_error);
  connect(reply, &QNetworkReply::downloadProgress, this, &HttpTransport::on_reply_downloadProgress);
}

void HttpTransport::on_reply_error(QNetworkReply::NetworkError code)
{
  auto reply = dynamic_cast<QNetworkReply*>(sender());

  qWarning() << "HttpTransport Error (" << code << "):" << reply->errorString();

  reply->deleteLater();
}

void HttpTransport::on_reply_readyRead()
{
  auto reply = dynamic_cast<QNetworkReply*>(sender());

  emit dataReady(reply->url(), reply->readAll());
}

void HttpTransport::on_reply_downloadProgress(qint64 total, qint64 done)
{
  qDebug() << QString("Progress: %1 of %2 bytes...").arg(done).arg(total);
}

void HttpTransport::on_manager_finished(QNetworkReply* reply)
{
  qDebug() << "Finished";

  emit replyFinished(reply->url(), reply->readAll());

  reply->deleteLater();
}
}
