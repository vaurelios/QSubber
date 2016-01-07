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

#ifndef SUBTITLEMODEL_H
#define SUBTITLEMODEL_H

namespace QSubber
{
    class SubtitleModel : public QAbstractListModel
    {
        Q_OBJECT

        QList<QVariantList> cols;

    public:
        explicit SubtitleModel(QVariantList subtitles);
        ~SubtitleModel();

        //QModelIndex	index(int row, int column, const QModelIndex& parent=QModelIndex()) const Q_DECL_OVERRIDE;
        //QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;
        int rowCount(const QModelIndex& parent=QModelIndex()) const Q_DECL_OVERRIDE;
        int columnCount(const QModelIndex& parent=QModelIndex()) const Q_DECL_OVERRIDE;
        Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
        QVariant data(const QModelIndex& index, int role=Qt::DisplayRole) const Q_DECL_OVERRIDE;
        QVariant headerData(int section, Qt::Orientation orientation, int role=Qt::DisplayRole) const Q_DECL_OVERRIDE;
    };
}

#endif /* SUBTITLEMODEL_H */
