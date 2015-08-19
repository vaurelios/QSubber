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


#include <qwidget.h>
#include <iostream>
#include "globals.h"
#include "utils.h"


/* Globals */
QApplication  *app;
MainWindow    *window;
OSHandling    *osh;
SubDownloader *downh;

int main(int ac, char **argv)
{
    app = new QApplication(ac, argv);
    app->setApplicationName("QSubber");
    app->setApplicationVersion("1.0a");

    osh = new OSHandling();
    downh = new SubDownloader();

    window = new MainWindow();
    window->show();

    osh->LogIn("", "");

    std::cout << QSubber::calculate_hash_for_file("breakdance.avi").toUtf8().data() << std::endl;

    return app->exec();
}
