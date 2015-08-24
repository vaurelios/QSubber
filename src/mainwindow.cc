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
    centralWidget = new QWidget();
    tvmodel       = new QStandardItemModel();

    /* Widgets */
    browseButton     = new QPushButton("...");
    langCombo        = new QComboBox();
    mediaLabel       = new QLabel("Media:");
    nameLabel        = new QLabel("Name:");
    seasonLabel      = new QLabel("Season:");
    epLabel          = new QLabel("Episode:");
    mediaEdit        = new QLineEdit();
    nameEdit         = new QLineEdit();
    seasonEdit       = new QLineEdit();
    epEdit           = new QLineEdit();
    subTreeView      = new QTreeView();
    downButton       = new QPushButton("&Download selected");
    searchHSHButton  = new QPushButton("Search by &Hash");
    searchNameButton = new QPushButton("Search by &Name");

    /* Layouts */
    QGridLayout* mainLayout   = new QGridLayout();
    QGridLayout* searchLayout = new QGridLayout();
    QHBoxLayout* tvLayout     = new QHBoxLayout();
    QHBoxLayout *btnLayout    = new QHBoxLayout();

    // --- file
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Preferences", this, SLOT(preferences()), QKeySequence("Ctrl+P"));
    fileMenu->addSeparator();
    fileMenu->addAction("&Quit", this, SLOT(quit()), QKeySequence::Quit);

    // --- help
    helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About", this, SLOT(about()));

    searchLayout->addWidget(mediaLabel,   0, 0);
    searchLayout->addWidget(mediaEdit,    0, 1);
    searchLayout->addWidget(browseButton, 0, 2);
    searchLayout->addWidget(nameLabel,    1, 0);
    searchLayout->addWidget(nameEdit,     1, 1);
    searchLayout->addWidget(langCombo,    1, 2);
    searchLayout->addWidget(seasonLabel,  2, 0);
    searchLayout->addWidget(seasonEdit,   2, 1);
    searchLayout->addWidget(epLabel,      3, 0);
    searchLayout->addWidget(epEdit,       3, 1);

    tvmodel->setHorizontalHeaderLabels(QStringList() << "Name" << "Size");

    tvLayout->addWidget(subTreeView);
    subTreeView->setModel(tvmodel);
    subTreeView->header()->setSectionsMovable(false);
    subTreeView->header()->setSectionsClickable(false);
    subTreeView->header()->setStretchLastSection(false);
    subTreeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    subTreeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    btnLayout->addWidget(downButton);
    btnLayout->addWidget(searchHSHButton);
    btnLayout->addWidget(searchNameButton);

    mainLayout->addLayout(searchLayout, 0, 0);
    mainLayout->addLayout(tvLayout, 1, 0);
    mainLayout->addLayout(btnLayout, 3, 0);

    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
    setWindowTitle("QSubber");

    // connect signals
    QObject::connect(mediaEdit, &QLineEdit::textChanged, this, &MainWindow::mediaChanged);
    QObject::connect(langCombo, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &MainWindow::langChanged);
    QObject::connect(browseButton, &QPushButton::clicked, this, &MainWindow::browser_button_clicked);
    QObject::connect(downButton, &QPushButton::clicked, this, &MainWindow::down_button);
    QObject::connect(searchHSHButton, &QPushButton::clicked, this, &MainWindow::hash_search_button);
    QObject::connect(searchNameButton, &QPushButton::clicked, this, &MainWindow::full_search_button);
    QObject::connect(osh, &OSHandling::update_status, this, &MainWindow::update_status);
    QObject::connect(osh, &OSHandling::sublist_updated, this, &MainWindow::sublist_updated);
    QObject::connect(osh, &OSHandling::clear_list, this, &MainWindow::clear_list);
    QObject::connect(downh, &SubDownloader::update_status, this, &MainWindow::update_status);

    /* command line arguments */
    QStringList pargs = parser->positionalArguments();
    if (pargs.count() > 0)
        mediaEdit->setText(pargs.at(0));

    /* fill language combobox */
    QMap<QString, QString> langs = settings->getLangCodes();

    QMapIterator<QString, QString> i(langs);
    while (i.hasNext()) { i.next();
        langCombo->addItem(i.value(), i.key());
    }

    // set default value
    int langdefault = langCombo->findData(settings->getConfig("current_lang", "eng"));
    langCombo->setCurrentIndex(langdefault);
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

        tvmodel->appendRow(row);
    }
}

void MainWindow::clear_list() {
    tvmodel->removeRows(0, tvmodel->rowCount());
}

// UI slots
void MainWindow::langChanged(int index) {
    settings->setConfig("current_lang", langCombo->itemData(index).toString());
}

void MainWindow::mediaChanged(QString text) {
    QFileInfo info(text);
    QString base = info.completeBaseName();

    QRegExp sxey("([a-zA-Z0-9. ]+)[ -_.]+[Ss]([0-9]{0,2})[Ee]([0-9]{0,2})");
    QRegExp xxy("([a-zA-Z0-9. ]+)[ -_.]+([0-9]+)[Xx]([0-9]+)");

    if (sxey.indexIn(base) != -1) {
        QStringList texts = sxey.capturedTexts();
        QString name = texts.at(1);
        name.replace(".", " ");

        nameEdit->setText(name);
        seasonEdit->setText(texts.at(2));
        epEdit->setText(texts.at(3));
    }

    if(xxy.indexIn(base) != -1) {
        QStringList texts = xxy.capturedTexts();
        QString name = texts.at(1);
        name.replace(".", " ");

        nameEdit->setText(name);
        seasonEdit->setText(texts.at(2));
        epEdit->setText(texts.at(3));
    }
}

void MainWindow::browser_button_clicked() {
    QString dir = QDir::homePath();

    if (!mediaEdit->text().isEmpty()) {
        dir = QDir(mediaEdit->text()).path();
    }

    QString filename = QFileDialog::getOpenFileName(this,
                                                    "Open Media",
                                                    dir,
                                                    "Media Files (*.mp4 *.avi *.mkv *.flv *.3gp *.wmv)");

    mediaEdit->setText(filename);
}

void MainWindow::down_button() {
    QModelIndex index = subTreeView->currentIndex();

    if (index.isValid()) {
        QStandardItem *item = tvmodel->itemFromIndex(index);
        qsdict *subdata;

        subdata = (qsdict *) item->data().value<void *>();

        QString filename = subdata->at("SubFileName");
        QString suburl = subdata->at("SubDownloadLink");
        suburl.chop(3); // we don't want the gzipped one...

        qDebug() << filename << ":" << suburl;

        QStringList destfile;
        QString media = mediaEdit->text();
        if(media.isEmpty()) {
            destfile << QDir::currentPath();
            destfile << "/" << filename;
        } else {
            QFileInfo info(media);

            destfile << info.path();
            destfile << "/" << info.completeBaseName() << "." << filename.right(3);
        }
        downh->Download(QUrl(suburl), destfile.join(""), subdata->at("SubSize").toLongLong());
    }
}

void MainWindow::hash_search_button() {
    QString media = mediaEdit->text();

    QString hash = QSubber::calculate_hash_for_file(media.toUtf8().data());

    osh->HashSearch(hash);
}

void MainWindow::full_search_button() {
    osh->FullSearch(nameEdit->text(), seasonEdit->text(), epEdit->text());
}

void MainWindow::preferences()
{
    ConfigDialog* dialog = new ConfigDialog();
    dialog->exec();
}

void MainWindow::quit()
{
    app->quit();
}

void MainWindow::about()
{

}
