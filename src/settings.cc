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


#include "settings.h"
#include "globals.h"
#include "utils.h"

/* Global instance */
Settings *settings;

Settings::Settings(QString filename) : QObject(0)
{
    int ret;

    ret = sqlite3_open_v2(filename.toUtf8().data(), &db,
                          SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);

    if (ret == SQLITE_OK) {
        createTables();
    } else {
        qDebug() << "QSubber: failed to open a connection to SQLite3 file!";
        qDebug() << "QSubber: SQLite3 Error: " << sqlite3_errmsg(db);
    }
}

Settings* Settings::loadSettings() {
    QString filename = QStandardPaths::locate(QStandardPaths::ConfigLocation, "qsubber.db");

    if (filename.isEmpty())
        filename = QSubber::getConfigFilename();

    return new Settings(filename);
}

void Settings::createTables() {
    sqlite3_prepare_v2(db, "CREATE TABLE IF NOT EXISTS config("
                           "ID INTEGER PRIMARY KEY,"
                           "name TEXT NOT NULL,"
                           "value TEXT);",
                       -1, &stmt, NULL);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db, "CREATE TABLE IF NOT EXISTS langcodes("
                           "ID INTEGER PRYMARY KEY,"
                           "locale TEXT NOT NULL,"
                           "sublandid TEXT NOT NULL,"
                           "langname TEXT NOT NULL);",
                       -1, &stmt, NULL);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Settings::prepareStatements() {
    /* Inserts/updates */
    sqlite3_prepare_v2(db, "INSERT INTO config(name, value)"
                           "VALUES(?1, ?2);",
                       -1, &stmt_insert_config, NULL);
    sqlite3_prepare_v2(db, "UPDATE config"
                           "SET value = ?2"
                           "WHERE name = ?1;",
                       -1, &stmt_update_config, NULL);
    sqlite3_prepare_v2(db, "INSERT INTO langcodes(locale, sublandid, langname)"
                           "VALUES(?1, ?2, ?3);",
                       -1, &stmt_insert_lang, NULL);

    /* queries */
    sqlite3_prepare_v2(db, "SELECT value FROM config"
                           "WHERE name = ?1;",
                       -1, &stmt_get_config_value, NULL);
    sqlite3_prepare_v2(db, "SELECT sublangid, langname FROM langcodes"
                           "WHERE locale = ?1"
                           "ORDER BY sublangid ASC;",
                       -1, &stmt_get_lang_codes, NULL);
}

QString Settings::getConfig(QString name, QString defaultto) {
    sqlite3_bind_text(stmt_get_config_value, 1, name.toUtf8().data(), -1, NULL);

    int ret = sqlite3_step(stmt_get_config_value);

    if (ret == SQLITE_ROW) {
        const unsigned char* value = sqlite3_column_text(stmt_get_config_value, 0);

        sqlite3_reset(stmt);

        return QSubber::getStringFromUnsignedChar(value);
    }
    sqlite3_reset(stmt);

    return defaultto;
}

QMap<QString, QString> Settings::getLangCodes(QString locale) {
    QMap<QString, QString> codes;
    int ret;

    sqlite3_bind_text(stmt_get_lang_codes, 1, locale.toUtf8().data(), -1, NULL);

    while (SQLITE_ROW == (ret = sqlite3_step(stmt_get_lang_codes))) {
        const unsigned char* key = sqlite3_column_text(stmt_get_lang_codes, 0);
        const unsigned char* value = sqlite3_column_text(stmt_get_lang_codes, 1);

        codes[QSubber::getStringFromUnsignedChar(key)] = QSubber::getStringFromUnsignedChar(value);

        ret = sqlite3_step(stmt_get_lang_codes);
    }
    sqlite3_reset(stmt);

    return codes;
}

bool Settings::addLangCode(QString locale, QString langid, QString langname) {
    sqlite3_bind_text(stmt_insert_lang, 1, locale.toUtf8().data(), -1, NULL);
    sqlite3_bind_text(stmt_insert_lang, 2, langid.toUtf8().data(), -1, NULL);
    sqlite3_bind_text(stmt_insert_lang, 3, langname.toUtf8().data(), -1, NULL);

    int ret = sqlite3_step(stmt_insert_lang);
    sqlite3_reset(stmt_insert_lang);

    if (ret == SQLITE_DONE) return true;

    return false;
}
