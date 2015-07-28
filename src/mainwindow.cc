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


#include "mainwindow.h"

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
    fileMenu->addAction("&Preferences", this, SLOT(preferences(bool)), QKeySequence("Ctrl+P"));
    fileMenu->addSeparator();
    fileMenu->addAction("&Quit", this, SLOT(quit(bool)), QKeySequence::Quit);

    // --- help
    helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About", this, SLOT(about(bool)));

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

    //sublistModel = None;
    QHBoxLayout *lvLayout = new QHBoxLayout();
    lvLayout->addWidget(&subListView);

    //self.downButton.clicked.connect(self.download_sub)
    //searchHSHButton.clicked.connect(self.search_by_hash)
    //searchNameButton.clicked.connect(self.detailed_search)

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(&downButton);
    btnLayout->addWidget(&searchHSHButton);
    btnLayout->addWidget(&searchNameButton);

    QHBoxLayout *statusLayout = new QHBoxLayout();
    statusLayout->addWidget(&statusBar);

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->addLayout(searchLayout, 0, 0);
    mainLayout->addLayout(lvLayout, 1, 0);
    mainLayout->addLayout(btnLayout, 3, 0);
    mainLayout->addLayout(statusLayout, 4, 0);

    centralWidget.setLayout(mainLayout);
    setCentralWidget(&centralWidget);
    setWindowTitle("QSubber");
}

void MainWindow::preferences(bool checked)
{

}

void MainWindow::quit(bool checked)
{

}

void MainWindow::about(bool checked)
{

}
