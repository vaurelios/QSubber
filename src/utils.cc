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

#include "utils.hh"

#include "config.h"

#include <iostream>
#include <fstream>
#include <algorithm>

namespace QSubber
{
    QString calculate_hash_for_file(const char* filename)
    {
        std::ifstream f;
        uint64_t hash, fsize;

        f.open(filename, std::ios::in|std::ios::binary|std::ios::ate);
        if(!f.is_open()) return NULL;

        f.seekg(0, std::ios::end);
        fsize = f.tellg();
        f.seekg(0, std::ios::beg);

        hash = fsize;
        for (uint64_t tmp = 0, i = 0; i < 65536/sizeof(tmp) && f.read((char*)&tmp, sizeof(tmp)); i++, hash += tmp);
        f.seekg(std::max<uint64_t>((uint64_t)fsize - 65536, 0), std::ios::beg);
        for (uint64_t tmp = 0, i = 0; i < 65536/sizeof(tmp) && f.read((char*)&tmp, sizeof(tmp)); i++, hash += tmp);

        char* tmp = (char *) malloc(256);
        snprintf(tmp, 256, "%lx", hash);

        QString shash(tmp);

        free(tmp);

        return shash;
    }

    QString getConfigFilename()
    {
        QString dirpath = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0);
        dirpath.append('/'); dirpath.append(APP_CONFIG_DIR);

        QDir configDir(dirpath);

        if(!configDir.exists())
            configDir.mkpath(".");

        return configDir.absolutePath().append('/').append(APP_CONFIG_FILENAME);
    }
}
