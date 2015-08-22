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


#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include <map>
#include <QObject>
#include "mainwindow.h"
#include "oshandling.h"
#include "subdownloader.h"
#include "settings.h"

using namespace std;

typedef map<QString, QString> qsdict;
typedef vector<qsdict>        vqsdict;

extern QApplication       *app;
extern QCommandLineParser *parser;
extern Settings           *settings;
extern MainWindow         *window;
extern OSHandling         *osh;
extern SubDownloader      *downh;
extern vqsdict             sublist;

#endif // GLOBALS

