#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <sqlite3.h>

class Settings : public QObject
{
    Q_OBJECT

    sqlite3      *db;
    sqlite3_stmt *stmt;
    sqlite3_stmt *stmt_update_config;
    sqlite3_stmt *stmt_insert_config;
    sqlite3_stmt *stmt_insert_lang;
    sqlite3_stmt *stmt_get_config_value;
    sqlite3_stmt *stmt_get_lang_codes;

protected:
    void createTables();
    void prepareStatements();

public:
    explicit Settings(QString filename);
    static Settings* loadSettings();

    QMap<QString, QString> getLangCodes(QString locale);
    QString getConfig(QString name);

signals:

public slots:
};

#endif // SETTINGS_H
