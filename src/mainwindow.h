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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QWidget*            centralWidget;
    QStandardItemModel* tvmodel;

    QMenu*       fileMenu;
    QMenu*       helpMenu;
    QPushButton* browseButton;
    QComboBox*   langCombo;
    QLabel*      mediaLabel;
    QLabel*      nameLabel;
    QLabel*      seasonLabel;
    QLabel*      epLabel;
    QLineEdit*   mediaEdit;
    QLineEdit*   nameEdit;
    QLineEdit*   seasonEdit;
    QLineEdit*   epEdit;
    QTreeView*   subTreeView;
    QPushButton* downButton;
    QPushButton* searchHSHButton;
    QPushButton* searchNameButton;
    QStatusBar*  statusbar;

public:
    explicit MainWindow(QWidget *parent = 0);

signals:

public slots:
    // thread conn slots
    void update_status(QString status, int timeout);
    void sublist_updated();
    void clear_list();

    // UI LineEdits
    void mediaChanged(QString text);
    void langChanged(int index);

    // UI buttons
    void browser_button_clicked();
    void down_button();
    void hash_search_button();
    void full_search_button();

    // --- file
    void preferences();
    void quit();
    // --- help
    void about();
};

#endif // MAINWINDOW_H
