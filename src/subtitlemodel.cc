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

#include "subtitlemodel.hh"

#include "subdata.hh"

namespace QSubber
{
    SubtitleModel::SubtitleModel(QVariantList subtitles)
        : QAbstractListModel()
    {
        QVariantList names;
        QVariantList sizes;
        QVariantList refs;

        for (int i = 0; i < subtitles.size(); ++i)
        {
            SubData* subdata = new SubData(subtitles.at(i).toMap());

            names.insert(i, subdata->getFilename());
            sizes.insert(i, subdata->getSize());
            refs.insert(i, QVariant::fromValue(static_cast<void *>(subdata)));
        }

        cols.insert(0, names); // name
        cols.insert(1, sizes); // size
        cols.insert(2, refs); // pointer to data
    }

    SubtitleModel::~SubtitleModel()
    {
        for (int i = 0; i < cols.at(2).size(); ++i)
        {
            SubData* itemData = static_cast<SubData *>(cols.at(2).at(i).value<void *>());

            delete itemData;
        }
    }

    int SubtitleModel::rowCount(const QModelIndex& parent) const
    {
        return cols.at(0).size();
    }

    int SubtitleModel::columnCount(const QModelIndex& parent) const
    {
        return cols.size() - 1;
    }

    Qt::ItemFlags SubtitleModel::flags(const QModelIndex& index) const
    {
        if (index.isValid()) return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

        return Qt::ItemIsEnabled;
    }

    QVariant SubtitleModel::data(const QModelIndex& index, int role) const
    {
        if (role != Qt::DisplayRole) return QVariant();

        qDebug() << "Data for:" << index.column() << "," << index.row();

        if (cols.size() > index.column())
        {
            if (cols.at(index.column()).size() > index.row())
            {
                return cols.at(index.column()).at(index.row());
            }
        }

        return QVariant();
    }

    QVariant SubtitleModel::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (orientation != Qt::Horizontal) return QVariant();

        if (role == Qt::DisplayRole)
        {
            if (section == 0)
            {
                return tr("Name");
            }

            if (section == 1)
            {
                return tr("Size");
            }
        }

        return QVariant();
    }
}
