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

#include "mainwindow.hh"

#include "application.hh"
#include "settingsdialog.hh"
#include "subdownloader.hh"
#include "subtitlemodel.hh"
#include "utils.hh"

namespace QSubber
{
    MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
    {
        QSubber::Application* app = static_cast<QSubber::Application*>(qApp);

        ui.setupUi(this);

        QString current_lang = app->settings->getConfig("current_lang", "eng");

        QObject::connect(app, &Application::sublist_updated, this, &MainWindow::sublist_updated);

        /* command line arguments */
        QStringList pargs = app->positionalArgs();
        if (pargs.count() > 0)
            ui.mediaEdit->setText(pargs.at(0));

        /* fill language combobox */
        QMap<QString, QString> langs = app->settings->getLangCodes();

        QMapIterator<QString, QString> i(langs);
        while (i.hasNext())
        { i.next();
            ui.langCombo->addItem(i.value(), i.key());
        }

        // set default value
        int langdefault = ui.langCombo->findData(current_lang);
        ui.langCombo->setCurrentIndex(langdefault);
    }

    /* Slots */
    // conn
    void MainWindow::updateStatus(QString status, int timeout)
    {
         statusBar()->showMessage(status, timeout);
    }

    void MainWindow::sublist_updated()
    {
        SubtitleModel* model;

        model = static_cast<SubtitleModel*>(ui.subtitlesView->model());

        if (model) delete model;

        QVariantList data = static_cast<Application*>(qApp)->getSubList();

        model = new SubtitleModel(data);

        ui.subtitlesView->setModel(model);

        ui.subtitlesView->header()->setStretchLastSection(false);
        ui.subtitlesView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
        ui.subtitlesView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    }

    // UI slots
    void MainWindow::on_langCombo_currentIndexChanged(int index)
    {
        Application* app = static_cast<Application*>(qApp);

        app->settings->setConfig("current_lang", ui.langCombo->itemData(index).toString());
    }

    void MainWindow::on_mediaEdit_textChanged(QString text)
    {
        QFileInfo info(text);
        QString base = info.completeBaseName();

        QVector<QRegExp> exps;

        exps.append(QRegExp("([a-zA-Z0-9. ]+)[ -_.]+[Ss]([0-9]{0,2})[Ee]([0-9]{0,2})"));
        exps.append(QRegExp("([a-zA-Z0-9. ]+)[ -_.]+([0-9]+)[Xx]([0-9]+)"));
        exps.append(QRegExp("([a-zA-Z0-9. ]+)[ -_.]+([0-9]{1,2})([0-9]{2})"));

        QVectorIterator<QRegExp> it(exps);
        while (it.hasNext())
        {
            QRegExp exp = it.next();

            if (exp.indexIn(base) != -1)
            {
                QStringList texts = exp.capturedTexts();
                QString name = texts.at(1);
                name.replace(".", " ");

                ui.nameEdit->setText(name);
                ui.seasonEdit->setText(texts.at(2));
                ui.episodeEdit->setText(texts.at(3));

                break;
            }
        }
    }

    void MainWindow::on_subtitlesView_doubleClicked()
    {
        on_downloadButton_clicked();
    }

    void MainWindow::on_browseButton_clicked()
    {
        QString dir = QDir::homePath();

        if (!ui.mediaEdit->text().isEmpty()) {
            dir = QDir(ui.mediaEdit->text()).path();
        }

        QString filename = QFileDialog::getOpenFileName(this,
                                                        "Open Media",
                                                        dir,
                                                        "Media Files (*.mp4 *.avi *.mkv *.flv *.3gp *.wmv)");

        ui.mediaEdit->setText(filename);
    }

    void MainWindow::on_downloadButton_clicked()
    {
        static SubDownloader downh(this);

        QModelIndex index = ui.subtitlesView->currentIndex();

        if (index.isValid())
        {
            SubtitleModel* model = static_cast<SubtitleModel*>(ui.subtitlesView->model());

            SubData* subdata = model->getSubData(index);

            QString filename = subdata->getFilename();
            QString suburl = subdata->getURL();
            suburl.chop(3); // we don't want the gzipped one...

            qDebug() << filename << ":" << suburl;

            QStringList destfile;
            QString media = ui.mediaEdit->text();
            if(media.isEmpty()) {
                destfile << QDir::currentPath();
                destfile << "/" << filename;
            } else {
                QFileInfo info(media);

                destfile << info.path();
                destfile << "/" << info.completeBaseName() << "." << filename.right(3);
            }

            downh.Download(QUrl(suburl), destfile.join(""), subdata->getByteSize());
        }
    }

    void MainWindow::on_aSearchButton_clicked()
    {
        Application* app = static_cast<Application*>(qApp);

        QString media = ui.mediaEdit->text();

        QVariantMap params;
        params["moviehash"]     = calculate_hash_for_file(media.toUtf8().data());
        params["moviebytesize"] = (double) QFileInfo(media).size();

        app->osh.Search(params);
    }

    void MainWindow::on_hSearchButton_clicked()
    {
        Application* app = static_cast<Application*>(qApp);

        QString media = ui.mediaEdit->text();

        QVariantMap params;
        params["moviehash"] = calculate_hash_for_file(media.toUtf8().data());

        app->osh.Search(params);
    }

    void MainWindow::on_nSearchButton_clicked()
    {
        Application* app = static_cast<Application*>(qApp);

        QVariantMap params;
        params["query"]   = ui.nameEdit->text();
        params["season"]  = ui.seasonEdit->text();
        params["episode"] = ui.episodeEdit->text();

        app->osh.Search(params);
    }

    void MainWindow::on_sSearchButton_clicked()
    {
        Application* app = static_cast<Application*>(qApp);

        QString media = ui.mediaEdit->text();

        QVariantMap params;
        params["moviebytesize"] = (double) QFileInfo(media).size();

        app->osh.Search(params);
    }

    void MainWindow::on_action_Preferences_triggered()
    {
        SettingsDialog* dialog = new SettingsDialog();
        dialog->exec();

        delete dialog;
    }

    void MainWindow::on_action_Quit_triggered()
    {
        qApp->quit();
    }

    void MainWindow::on_action_About_triggered()
    {

    }
}
