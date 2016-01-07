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


#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtCore>
#include <QtSql>

class Settings : public QObject
{
    Q_OBJECT

    bool ok;

protected:
    void createTables();
    void prepareStatements();
    bool configExists(QString name);

public:
    Settings(QObject* parent = 0, QString filename = "");
    ~Settings();

    static Settings* loadSettings(QObject *parent = 0);

    QMap<QString, QString> getLangCodes(QString locale = "");
    bool hasLocale(QString locale);
    void setConfig(QString name, QString value);
    QString getConfig(QString name, QString defaultto = "");
    bool langCodeExists(QString locale, QString langid);
    bool setLangCode(QString locale, QString langid, QString langname);

signals:

public slots:
};

#endif // SETTINGS_H
