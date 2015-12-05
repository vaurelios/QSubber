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


#include "globals.h"
#include "utils.h"
#include "mainwindow.h"
#include "configdialog.h"
#include <libgen.h>
#include <iostream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    QString current_lang = settings->getConfig("current_lang", "eng");

    tvmodel = new QStandardItemModel;
    tvmodel->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Size"));
    ui.subsTreeView->setModel(tvmodel);

    ui.subsTreeView->header()->setSectionsMovable(false);
    ui.subsTreeView->header()->setSectionsClickable(false);
    ui.subsTreeView->header()->setStretchLastSection(false);
    ui.subsTreeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui.subsTreeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    /* Thread communication signals */
    QObject::connect(osh, &OSHandling::update_status, this, &MainWindow::update_status);
    QObject::connect(osh, &OSHandling::sublist_updated, this, &MainWindow::sublist_updated);
    QObject::connect(osh, &OSHandling::clear_list, this, &MainWindow::clear_list);
    QObject::connect(downh, &SubDownloader::update_status, this, &MainWindow::update_status);

    /* command line arguments */
    QStringList pargs = parser->positionalArguments();
    if (pargs.count() > 0)
        ui.mediaEdit->setText(pargs.at(0));

    /* fill language combobox */
    QMap<QString, QString> langs = settings->getLangCodes();

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
void MainWindow::update_status(QString status, int timeout)
{
     statusBar()->showMessage(status, timeout);
}

void MainWindow::sublist_updated()
{
    for (int i = 0; i < sublist.size(); ++i)
    {
        SubData* subdata = sublist.at(i);

        QStandardItem *filename = new QStandardItem(subdata->getFilename());
        QStandardItem *filesize = new QStandardItem(subdata->getSize());
        filename->setEditable(false);
        filesize->setEditable(false);

        // a pointer to sublist element
        filename->setData(qVariantFromValue((void *) subdata));

        QList<QStandardItem *> row;
        row << filename << filesize;

        tvmodel->appendRow(row);
    }
}

void MainWindow::clear_list()
{
    tvmodel->removeRows(0, tvmodel->rowCount());

    for (int i = 0; i < sublist.size(); ++i)
    {
        delete sublist.at(i);
    }
    sublist.clear();
}

// UI slots
void MainWindow::on_langCombo_currentIndexChanged(int index)
{
    settings->setConfig("current_lang", ui.langCombo->itemData(index).toString());
}

void MainWindow::on_mediaEdit_textChanged(QString text)
{
    QFileInfo info(text);
    QString base = info.completeBaseName();

    QRegExp sxey("([a-zA-Z0-9. ]+)[ -_.]+[Ss]([0-9]{0,2})[Ee]([0-9]{0,2})");
    QRegExp xxy("([a-zA-Z0-9. ]+)[ -_.]+([0-9]+)[Xx]([0-9]+)");

    if (sxey.indexIn(base) != -1)
    {
        QStringList texts = sxey.capturedTexts();
        QString name = texts.at(1);
        name.replace(".", " ");

        ui.nameEdit->setText(name);
        ui.seasonEdit->setText(texts.at(2));
        ui.episodeEdit->setText(texts.at(3));
    }

    if(xxy.indexIn(base) != -1)
    {
        QStringList texts = xxy.capturedTexts();
        QString name = texts.at(1);
        name.replace(".", " ");

        ui.nameEdit->setText(name);
        ui.seasonEdit->setText(texts.at(2));
        ui.episodeEdit->setText(texts.at(3));
    }
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
    QModelIndex index = ui.subsTreeView->currentIndex();

    if (index.isValid()) {
        QStandardItem *item = tvmodel->itemFromIndex(index);

        SubData* subdata = (SubData *) item->data().value<void *>();

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
        downh->Download(QUrl(suburl), destfile.join(""), subdata->getSize());
    }
}

void MainWindow::on_hSearchButton_clicked()
{
    QString media = ui.mediaEdit->text();

    QString hash = QSubber::calculate_hash_for_file(media.toUtf8().data());

    osh->HashSearch(hash);
}

void MainWindow::on_nSearchButton_clicked()
{
    osh->FullSearch(ui.nameEdit->text(), ui.seasonEdit->text(), ui.episodeEdit->text());
}

void MainWindow::on_action_Preferences_triggered()
{
    ConfigDialog* dialog = new ConfigDialog();
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
