#include "subdata.h"

SubData::SubData(cstruct data)
{
    QString d_id = QString::fromStdString(value_string(data.at("IDSubtitle")).cvalue());
    QString d_size = QString::fromStdString(value_string(data.at("SubSize")).cvalue());
    QString d_url = QString::fromStdString(value_string(data.at("SubDownloadLink")).cvalue());
    QString d_filename = QString::fromStdString(value_string(data.at("SubFileName")).cvalue());

    id       = d_id.toLongLong();
    size     = d_size.toLongLong();
    url      = d_url;
    filename = d_filename;
}

qint64 SubData::getID() {
    return id;
}

qint64 SubData::getSize() {
    return size;
}

QString SubData::getURL() {
    return url;
}

QString SubData::getFilename() {
    return filename;
}
