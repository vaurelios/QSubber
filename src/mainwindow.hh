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

#include "ui_mainwindow.h"

#include <QtWidgets>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

namespace QSubber
{
    class MainWindow : public QMainWindow
    {
        Q_OBJECT

        QStandardItemModel* tvmodel;

    public:
        explicit MainWindow(QWidget *parent = 0);

        void updateStatus(QString status, int timeout);

    signals:

    public slots:
        void sublist_updated();

    private slots:
        void on_mediaEdit_textChanged(QString text);
        void on_langCombo_currentIndexChanged(int index);

        void on_browseButton_clicked();
        void on_downloadButton_clicked();
        void on_aSearchButton_clicked();
        void on_hSearchButton_clicked();
        void on_nSearchButton_clicked();
        void on_sSearchButton_clicked();

        void on_action_Preferences_triggered();
        void on_action_Quit_triggered();
        void on_action_About_triggered();

    private:
        Ui::MainWindow ui;
    };
}

#endif // MAINWINDOW_H
