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


#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include "application.hh"

#include <QtWidgets>

class ConfigDialog : public QDialog
{
    Q_OBJECT

    QHash<QString, QString> values;

    QSubber::Application* app;

    QVBoxLayout*      dialogLayout;
    QGroupBox*        userAuthBox;
    QDialogButtonBox* buttons;
    QGridLayout*      userAuthLayout;

    QLabel*    userLabel;
    QLabel*    passLabel;
    QLineEdit* userEdit;
    QLineEdit* passEdit;


public:
    ConfigDialog();

signals:

public slots:
    void accepted();
    void auth_user_changed();
    void auth_pass_changed();
};

#endif // CONFIGDIALOG_H
