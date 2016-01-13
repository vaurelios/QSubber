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

#include "settingsdialog.hh"

namespace QSubber
{
    SettingsDialog::SettingsDialog()
    {
        Application* app = static_cast<Application*>(qApp);

        ui.setupUi(this);

        /* Load values */
        ui.userEdit->setText(app->settings->getConfig("auth_user"));
        ui.passEdit->setText(app->settings->getConfig("auth_pass"));

        /* Signals / Slots */
        connect(this, &QDialog::accepted, this, &SettingsDialog::accepted);
        connect(ui.userEdit, &QLineEdit::textChanged, this, &SettingsDialog::auth_user_changed);
        connect(ui.passEdit, &QLineEdit::textChanged, this, &SettingsDialog::auth_pass_changed);
    }

    void SettingsDialog::accepted()
    {
        Application* app = static_cast<Application*>(qApp);

        QHashIterator<QString, QString> i(values);
        while (i.hasNext())
        {
            i.next();
            app->settings->setConfig(i.key(), i.value());
        }
    }

    void SettingsDialog::auth_user_changed()
    {
        values["auth_user"] = ui.userEdit->text();
    }

    void SettingsDialog::auth_pass_changed()
    {
        values["auth_pass"] = ui.passEdit->text();
    }
}
