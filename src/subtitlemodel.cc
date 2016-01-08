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

namespace QSubber
{
    SubtitleModel::SubtitleModel(QVariantList subtitles)
        : QAbstractListModel()
    {
        for (int i = 0; i < subtitles.size(); ++i)
        {
            SubData* subdata = new SubData(subtitles.at(i).toMap());

            subs.append(subdata);
        }
    }

    SubtitleModel::~SubtitleModel()
    {
        qDeleteAll(subs);
    }

    int SubtitleModel::rowCount(const QModelIndex& parent) const
    {
        if (parent.isValid()) return 0;

        return subs.size();
    }

    int SubtitleModel::columnCount(const QModelIndex& parent) const
    {
        if (parent.isValid()) return 0;

        return 2;
    }

    Qt::ItemFlags SubtitleModel::flags(const QModelIndex& index) const
    {
        if (index.isValid()) return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

        return Qt::ItemIsEnabled;
    }

    QVariant SubtitleModel::data(const QModelIndex& index, int role) const
    {
        if (role != Qt::DisplayRole) return QVariant();

        if (subs.size() > index.row() && index.column() < 2)
        {
            if (index.column() == 0)
                return subs.at(index.row())->getFilename();
            else if (index.column() == 1)
                return subs.at(index.row())->getSize();
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

    SubData* SubtitleModel::getSubData(const QModelIndex& index) const
    {
        return subs.at(index.row());
    }
}
