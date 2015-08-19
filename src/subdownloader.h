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
    explicit SubDownloader(QObject *parent = 0);
    void Download(QUrl url, QString destfile, qint64 filesize = 0);

signals:
    void update_status(QString status, int timeout=0);

public slots:
    void replyError(QNetworkReply::NetworkError err);
    void downloadProgress(qint64 br, qint64 bt);
    void replyFinished();
};

#endif // SUBDOWNLOADER_H
