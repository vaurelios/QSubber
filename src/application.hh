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

#include "mainwindow.hh"
#include "oshandling.hh"
#include "settings.hh"

#include <QtWidgets/QApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QList>
#include <QtCore/QStringList>

#ifndef QSUBBER_APPLICATION
#define QSUBBER_APPLICATION

namespace QSubber
{
    class Application : public QApplication
    {
        Q_OBJECT

        QCommandLineParser* parser;
        QVariantList sublist;
        QString current_job;

        public:
            Application(int &argc, char **argv);

            QStringList positionalArgs();
            void setCurrentJob(QString job);
            QString popCurrentJob();
            void setSubList(QVariantList sublist);
            QVariantList getSubList();
            void updateStatus(QString status, int timeout=0);

            Settings*       settings;
            MainWindow*     window;
            OSHandling      osh;

        signals:
            void sublist_updated();
    };
}

#endif /* QSUBBER_APPLICATION */
