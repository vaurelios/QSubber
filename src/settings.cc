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


#include <QSqlError>
#include <QSqlRecord>
#include "settings.h"
#include "globals.h"
#include "utils.h"

/* Global instance */
Settings *settings;

Settings::Settings(QObject *parent, QString filename) : QObject(parent)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filename);
    ok = db.open();

    if (ok)
    {
        createTables();
    }
    else {
        qDebug() << "QSubber: failed to open a connection to SQLite3 file!";
        qDebug() << "QSubber: SQLite3 Error: " << db.lastError().text();
    }
}

Settings::~Settings()
{
    QSqlDatabase::database().close();
}

Settings* Settings::loadSettings(QObject* parent)
{
    QString filename = QStandardPaths::locate(QStandardPaths::ConfigLocation, "qsubber.db");

    if (filename.isEmpty())
        filename = QSubber::getConfigFilename();

    return new Settings(parent, filename);
}

void Settings::createTables()
{
    if (!ok) return;

    QSqlQuery query;

    query.exec("CREATE TABLE IF NOT EXISTS config(name TEXT NOT NULL, value TEXT);");
    query.exec("CREATE TABLE IF NOT EXISTS langcodes(locale TEXT NOT NULL, sublangid TEXT NOT NULL, langname TEXT NOT NULL);");
}

bool Settings::configExists(QString name)
{
    if (!ok) return false;

    QSqlTableModel model;

    model.setTable("config");
    model.setFilter(QString("name='%1'").arg(name));
    model.select();

    return model.rowCount() > 0 ? true : false;
}

QString Settings::getConfig(QString name, QString defaultto)
{
    if (!ok || !configExists(name)) return defaultto;

    QSqlTableModel model;

    model.setTable("config");
    model.setFilter(QString("name='%1'").arg(name));
    model.select();

    return model.record(0).value("value").toString();
}

void Settings::setConfig(QString name, QString value)
{
    if (!ok) return;

    QSqlTableModel model;
    model.setTable("config");

    if (!configExists(name))
    {
        QSqlRecord record;

        record.setValue("name", name);
        record.setValue("value", value);

        model.insertRecord(-1, record);
    }
    else {
        model.setFilter(QString("name='%1'").arg(name));
        model.select();

        QSqlRecord record = model.record(0);

        record.setValue("value", value);

        model.setRecord(0, record);
    }

    model.submitAll();
}

bool Settings::hasLocale(QString locale)
{
    if (!ok) return false;

    QSqlTableModel model;

    model.setTable("langcodes");
    model.setFilter(QString("locale='%1'").arg(locale));
    model.select();

    return model.rowCount() > 0 ? true : false;
}

QMap<QString, QString> Settings::getLangCodes(QString locale)
{
    QMap<QString, QString> codes;

    if (!ok) return codes;

    QSqlTableModel model;
    model.setTable("langcodes");

    locale = locale.isEmpty() ? getConfig("current_locale", "en") : locale;

    model.setFilter(QString("locale='%1'").arg(locale));
    model.select();


    for (int i = 0; i < model.rowCount(); ++i)
    {
        QString key   = model.record(i).value("sublangid").toString();
        QString value = model.record(i).value("langname").toString();

        codes[key] = value;
    }

    return codes;
}

bool Settings::langCodeExists(QString locale, QString langid)
{
    if (!ok) return false;

    QSqlTableModel model;
    model.setTable("langcodes");

    model.clear();
    model.setFilter(QString("locale='%1' AND sublangid='%2'").arg(locale).arg(langid));
    model.select();

    return model.rowCount() > 0 ? true : false;
}

bool Settings::setLangCode(QString locale, QString langid, QString langname)
{
    if (!ok) return false;

    QSqlTableModel model;
    model.setTable("langcodes");

    if (!langCodeExists(locale, langid))
    {
        QSqlRecord record;

        record.setValue("locale", locale);
        record.setValue("sublangid", langid);
        record.setValue("langname", langname);

        model.insertRecord(-1, record);
    }
    else {
        model.setFilter(QString("locale='%1' AND sublangid='%2'").arg(locale).arg(langid));
        model.select();

        QSqlRecord record = model.record(0);

        record.setValue("langname", langname);

        model.setRecord(0, record);
    }

    return model.submitAll();
}
