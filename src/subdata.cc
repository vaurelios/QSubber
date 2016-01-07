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

#include "subdata.hh"

namespace QSubber
{
    SubData::SubData(QVariantMap data)
    {
        QString d_id       = data.value("IDSubtitle").toString();
        QString d_size     = data.value("SubSize").toString();
        QString d_url      = data.value("SubDownloadLink").toString();
        QString d_filename = data.value("SubFileName").toString();

        id       = d_id.toLongLong();
        size     = d_size;
        url      = d_url;
        filename = d_filename;
    }

    qint64 SubData::getID()
    {
        return id;
    }

    qint64 SubData::getByteSize()
    {
        return size.toLongLong();
    }

    QString SubData::getSize()
    {
        qint64 ll_size = size.toLongLong();

        if (ll_size > 1024 && ll_size < (1024 * 1024))
            return QString("%0 kB").arg(ll_size / 1024);

        return size;
    }

    QString SubData::getURL()
    {
        return url;
    }

    QString SubData::getFilename()
    {
        return filename;
    }
}
