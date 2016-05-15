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

#include "application.hh"

#include "config.h"

namespace QSubber
{
    Application::Application(int &argc, char **argv)
        : QApplication(argc, argv)
        , osh(this)
    {
        setApplicationName("QSubber");
        setApplicationVersion(QSUBBER_VERSION);

        parser = new QCommandLineParser;
        parser->addHelpOption();
        parser->addVersionOption();
        parser->addPositionalArgument("media", "Media file to search subtitle for.");
        parser->process(*this);

        settings = Settings::loadSettings(this);

        window = new MainWindow;
        window->show();

        osh.fetchSubLanguages();
        osh.LogIn(settings->getConfig("auth_user"), settings->getConfig("auth_pass"));
    }

    QStringList Application::positionalArgs()
    {
        return parser->positionalArguments();
    }

    void Application::setCurrentJob(QString job)
    {
        window->setEnabled(false);

        current_job = job;
    }

    QString Application::popCurrentJob()
    {
        window->setEnabled(true);

        QString old_job = current_job;
        current_job.clear();

        return old_job;
    }

    void Application::setSubList(QVariantList sublist)
    {
        this->sublist = sublist;

        emit sublist_updated();
    }

    QVariantList Application::getSubList()
    {
        return sublist;
    }

    void Application::updateStatus(QString status, int timeout)
    {
        window->updateStatus(status, timeout);
    }
}
