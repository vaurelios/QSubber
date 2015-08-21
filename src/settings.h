#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <sqlite3.h>

class Settings : public QObject
{
    Q_OBJECT

    sqlite3      db;
    sqlite3_stmt stmt;

public:
    explicit Settings(QObject *parent = 0);

signals:

public slots:
};

#endif // SETTINGS_H
