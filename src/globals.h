#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include <map>
#include <QObject>
#include "mainwindow.h"
#include "oshandling.h"
#include "subdownloader.h"

using namespace std;

typedef map<QString, QString> qsdict;
typedef vector<qsdict>        vqsdict;

extern QApplication       *app;
extern QCommandLineParser *parser;
extern MainWindow         *window;
extern OSHandling         *osh;
extern SubDownloader      *downh;
extern vqsdict             sublist;

#endif // GLOBALS

