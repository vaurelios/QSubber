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
        prepareStatements();
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
                           "name TEXT NOT NULL,"
                           "value TEXT);",
                       -1, &stmt, NULL);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    sqlite3_prepare_v2(db, "CREATE TABLE IF NOT EXISTS langcodes("
                           "locale TEXT NOT NULL,"
                           "sublangid TEXT NOT NULL,"
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
    sqlite3_prepare_v2(db, "UPDATE config SET value = ?2 WHERE name = ?1;",
                       -1, &stmt_update_config, NULL);
    sqlite3_prepare_v2(db, "INSERT INTO langcodes(locale, sublangid, langname)"
                           "VALUES(?1, ?2, ?3);",
                       -1, &stmt_insert_lang, NULL);

    /* queries */
    sqlite3_prepare_v2(db, "SELECT value FROM config WHERE name = ?1;",
                       -1, &stmt_get_config_value, NULL);
    sqlite3_prepare_v2(db, "SELECT sublangid, langname FROM langcodes WHERE locale = ?1"
                           "ORDER BY sublangid ASC;",
                       -1, &stmt_get_lang_codes, NULL);
}

bool Settings::configExists(QString name) {
    sqlite3_stmt *stmt_count;

    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM config WHERE name = ?1",
                       -1, &stmt_count, NULL);
    sqlite3_bind_text(stmt_count, 1, name.toUtf8().data(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt_count);

    int count = sqlite3_column_int(stmt_count, 0);

    if ( count > 0)
        return true;

    return false;
}

QString Settings::getConfig(QString name, QString defaultto) {
    if (!configExists(name)) return defaultto;

    sqlite3_bind_text(stmt_get_config_value, 1, name.toUtf8().data(), -1, NULL);

    if (SQLITE_ROW == sqlite3_step(stmt_get_config_value)) {
        const unsigned char* value = sqlite3_column_text(stmt_get_config_value, 0);

        sqlite3_reset(stmt);

        return QSubber::getStringFromUnsignedChar(value);
    }

    return defaultto;
}

void Settings::setConfig(QString name, QString value) {
    if (!configExists(name)) {
        sqlite3_bind_text(stmt_insert_config, 1, name.toUtf8().data(),  -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt_insert_config, 2, value.toUtf8().data(), -1, SQLITE_TRANSIENT);

        sqlite3_step(stmt_insert_config);
        sqlite3_reset(stmt_insert_config);
    } else {
        sqlite3_bind_text(stmt_update_config, 1, name.toUtf8().data(),  -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt_update_config, 2, value.toUtf8().data(), -1, SQLITE_TRANSIENT);

        sqlite3_step(stmt_update_config);
        sqlite3_reset(stmt_update_config);
    }
}

bool Settings::hasLocale(QString locale) {
    sqlite3_stmt *stmt_count;

    sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM langcodes WHERE locale = ?1",
                       -1, &stmt_count, NULL);
    sqlite3_bind_text(stmt_count, 1, locale.toUtf8().data(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt_count);

    int count = sqlite3_column_int(stmt_count, 0);

    if ( count > 0)
        return true;

    return false;
}

QMap<QString, QString> Settings::getLangCodes(QString locale) {
    QMap<QString, QString> codes;

    if (locale.isEmpty()) locale = getConfig("current_locale", "en");

    sqlite3_bind_text(stmt_get_lang_codes, 1, locale.toUtf8().data(), -1, SQLITE_TRANSIENT);

    while (true) {
        if (SQLITE_ROW != sqlite3_step(stmt_get_lang_codes)) break;

        const unsigned char* key = sqlite3_column_text(stmt_get_lang_codes, 0);
        const unsigned char* value = sqlite3_column_text(stmt_get_lang_codes, 1);

        codes[QSubber::getStringFromUnsignedChar(key)] = QSubber::getStringFromUnsignedChar(value);
    }
    sqlite3_reset(stmt);

    return codes;
}

bool Settings::addLangCode(QString locale, QString langid, QString langname) {
    sqlite3_bind_text(stmt_insert_lang, 1, locale.toUtf8().data(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt_insert_lang, 2, langid.toUtf8().data(),   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt_insert_lang, 3, langname.toUtf8().data(), -1, SQLITE_TRANSIENT);

    int ret = sqlite3_step(stmt_insert_lang);
    sqlite3_reset(stmt_insert_lang);

    if (ret == SQLITE_DONE) return true;

    return false;
}
