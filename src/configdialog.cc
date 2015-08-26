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


#include "configdialog.h"
#include "globals.h"

ConfigDialog::ConfigDialog()
{
    dialogLayout   = new QVBoxLayout();
    userAuthBox    = new QGroupBox("User Authentication");
    userAuthLayout = new QGridLayout();
    buttons        = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    userLabel = new QLabel("User:");
    passLabel = new QLabel("Password:");
    userEdit  = new QLineEdit();
    passEdit  = new QLineEdit();

    userAuthLayout->addWidget(userLabel, 0, 0);
    userAuthLayout->addWidget(userEdit,  0, 1);
    userAuthLayout->addWidget(passLabel, 1, 0);
    userAuthLayout->addWidget(passEdit,  1, 1);

    userAuthBox->setLayout(userAuthLayout);
    dialogLayout->addWidget(userAuthBox);
    dialogLayout->addWidget(buttons);

    setLayout(dialogLayout);

    /* Load values */
    userEdit->setText(settings->getConfig("auth_user"));
    passEdit->setText(settings->getConfig("auth_pass"));

    /* Signals / Slots */
    connect(this, &QDialog::accepted, this, &ConfigDialog::accepted);
    connect(buttons, &QDialogButtonBox::accepted, this, &ConfigDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &ConfigDialog::reject);
    connect(userEdit, &QLineEdit::textChanged, this, &ConfigDialog::auth_user_changed);
    connect(passEdit, &QLineEdit::textChanged, this, &ConfigDialog::auth_pass_changed);
}

void ConfigDialog::accepted() {
    QHashIterator<QString, QString> i(values);
    while(i.hasNext()) {
        i.next();
        settings->setConfig(i.key(), i.value());
    }
}

void ConfigDialog::auth_user_changed() {
    values["auth_user"] = userEdit->text();
}

void ConfigDialog::auth_pass_changed() {
    values["auth_pass"] = passEdit->text();
}
