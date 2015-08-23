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
#include <libgen.h>
#include <iostream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , browseButton("...")
    , mediaLabel("Media:")
    , nameLabel("Name:")
    , seasonLabel("Season:")
    , epLabel("Episode:")
    , downButton("&Download selected")
    , searchHSHButton("Search by &Hash")
    , searchNameButton("Search by &Name")
{
    // --- file
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Preferences", this, SLOT(preferences()), QKeySequence("Ctrl+P"));
    fileMenu->addSeparator();
    fileMenu->addAction("&Quit", this, SLOT(quit()), QKeySequence::Quit);

    // --- help
    helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About", this, SLOT(about()));

    QGridLayout *searchLayout = new QGridLayout();
    searchLayout->addWidget(&mediaLabel, 0, 0);
    searchLayout->addWidget(&mediaEdit, 0, 1);
    searchLayout->addWidget(&browseButton, 0, 2);
    searchLayout->addWidget(&nameLabel, 1, 0);
    searchLayout->addWidget(&nameEdit, 1, 1);
    searchLayout->addWidget(&langCombo, 1, 2);
    searchLayout->addWidget(&seasonLabel, 2, 0);
    searchLayout->addWidget(&seasonEdit, 2, 1);
    searchLayout->addWidget(&epLabel, 3, 0);
    searchLayout->addWidget(&epEdit, 3, 1);

    QObject::connect(&mediaEdit, &QLineEdit::textChanged, this, &MainWindow::mediaChanged);

    // browse button
    QObject::connect(&browseButton, &QPushButton::clicked, this, &MainWindow::browser_button_clicked);

    // create model
    QStringList header;
    header << "Name" << "Size";
    tvmodel.setHorizontalHeaderLabels(header);
    // the view
    QHBoxLayout *tvLayout = new QHBoxLayout();
    tvLayout->addWidget(&subTreeView);
    subTreeView.setModel(&tvmodel);
    // the view header
    QHeaderView *hcol = subTreeView.header();

    hcol->setSectionsMovable(false);
    hcol->setSectionsClickable(false);
    hcol->setStretchLastSection(false);
    hcol->setSectionResizeMode(0, QHeaderView::Stretch);
    hcol->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    //hcol->resizeSection(1, 10);

    QObject::connect(&downButton, &QPushButton::clicked, this, &MainWindow::down_button);
    QObject::connect(&searchHSHButton, &QPushButton::clicked, this, &MainWindow::hash_search_button);
    QObject::connect(&searchNameButton, &QPushButton::clicked, this, &MainWindow::full_search_button);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(&downButton);
    btnLayout->addWidget(&searchHSHButton);
    btnLayout->addWidget(&searchNameButton);

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addLayout(searchLayout, 0, 0);
    mainLayout->addLayout(tvLayout, 1, 0);
    mainLayout->addLayout(btnLayout, 3, 0);

    centralWidget.setLayout(mainLayout);
    setCentralWidget(&centralWidget);
    setWindowTitle("QSubber");

    statusbar = statusBar();

    // connect conn signals
    QObject::connect(osh, &OSHandling::update_status, this, &MainWindow::update_status);
    QObject::connect(osh, &OSHandling::sublist_updated, this, &MainWindow::sublist_updated);
    QObject::connect(osh, &OSHandling::clear_list, this, &MainWindow::clear_list);
    QObject::connect(downh, &SubDownloader::update_status, this, &MainWindow::update_status);

    /* command line arguments */
    QStringList pargs = parser->positionalArguments();
    if (pargs.count() > 0)
        mediaEdit.setText(pargs.at(0));

    /* fill language combobox */
    QMap<QString, QString> langs = settings->getLangCodes();

    QMapIterator<QString, QString> i(langs);
    while (i.hasNext()) { i.next();
        langCombo.addItem(i.value(), i.key());
    }

    // set default value
    int langdefault = langCombo.findData(settings->getConfig("default-lang", "eng"));
    langCombo.setCurrentIndex(langdefault);
}

/* Slots */
// conn
void MainWindow::update_status(QString status, int timeout) {
     statusBar()->showMessage(status, timeout);
}

void MainWindow::sublist_updated() {
    for(qsdict &p : sublist) {
        QList<QStandardItem *> row;

        QStandardItem *filename = new QStandardItem(p.at("SubFileName"));
        QStandardItem *filesize = new QStandardItem(p.at("SubSize"));
        filename->setEditable(false);
        filesize->setEditable(false);

        // a pointer to sublist element
        qsdict *psub = &p;
        QVariant qpsub = qVariantFromValue((void *) psub);
        filename->setData(qpsub);

        row << filename << filesize;

        tvmodel.appendRow(row);
    }
}

void MainWindow::clear_list() {
    tvmodel.removeRows(0, tvmodel.rowCount());
}

// UI slots
void MainWindow::mediaChanged(QString text) {
    QFileInfo info(text);
    QString base = info.completeBaseName();

    QRegExp sxey("([a-zA-Z0-9. ]+)[ -_.]+[Ss]([0-9]{0,2})[Ee]([0-9]{0,2})");
    QRegExp xxy("([a-zA-Z0-9. ]+)[ -_.]+([0-9]+)[Xx]([0-9]+)");

    if (sxey.indexIn(base) != -1) {
        QStringList texts = sxey.capturedTexts();
        QString name = texts.at(1);
        name.replace(".", " ");

        nameEdit.setText(name);
        seasonEdit.setText(texts.at(2));
        epEdit.setText(texts.at(3));
    }

    if(xxy.indexIn(base) != -1) {
        QStringList texts = xxy.capturedTexts();
        QString name = texts.at(1);
        name.replace(".", " ");

        nameEdit.setText(name);
        seasonEdit.setText(texts.at(2));
        epEdit.setText(texts.at(3));
    }
}

void MainWindow::browser_button_clicked() {
    QString dir = QDir::homePath();

    if (!mediaEdit.text().isEmpty()) {
        dir = QDir(mediaEdit.text()).path();
    }

    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Open Media",
                                                    dir,
                                                    "Media Files (*.mp4 *.avi *.mkv *.flv *.3gp *.wmv)");

    mediaEdit.setText(filename);
}

void MainWindow::down_button() {
    QModelIndex index = subTreeView.currentIndex();

    if (index.isValid()) {
        QStandardItem *item = tvmodel.itemFromIndex(index);
        qsdict *subdata;

        subdata = (qsdict *) item->data().value<void *>();

        QString filename = subdata->at("SubFileName");
        QString suburl = subdata->at("SubDownloadLink");
        suburl.chop(3); // we don't want the gzipped one...

        qDebug() << filename << ":" << suburl;

        QStringList destfile;
        QString media = mediaEdit.text();
        if(media.isEmpty()) {
            destfile << QDir::currentPath();
            destfile << "/" << filename;
        } else {
            QFileInfo info(media);

            destfile << info.path();
            destfile << "/" << info.completeBaseName() << "." << filename.right(3);

            qDebug() << "Destfile: " << destfile.join("");
        }
        downh->Download(QUrl(suburl), destfile.join(""));
    }
}

void MainWindow::hash_search_button() {
    QString media = mediaEdit.text();

    QString hash = QSubber::calculate_hash_for_file(media.toUtf8().data());

    osh->HashSearch(hash);
}

void MainWindow::full_search_button() {
    osh->FullSearch(nameEdit.text(), seasonEdit.text(), epEdit.text());
}

void MainWindow::preferences()
{

}

void MainWindow::quit()
{
    app->quit();
}

void MainWindow::about()
{

}
