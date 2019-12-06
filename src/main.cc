/*
 * This file is part of KSmoothDock.
 * Copyright (C) 2017 Viet Dang (dangvd@gmail.com)
 *
 * KSmoothDock is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * KSmoothDock is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with KSmoothDock.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QDir>
#include <QIcon>
#include <QSettings>

#include <KAboutData>
#include <KDBusService>
#include <KLocalizedString>

#include <model/multi_dock_model.h>
#include <view/multi_dock_view.h>

int main(int argc, char** argv) {
  QApplication app(argc, argv);
  KDBusService service(KDBusService::Unique);

  KAboutData about(
      "unicorndock",
      "unicornDock",
      "6.1",
      i18n("A unicorn desktop panel for linux"),
      KAboutLicense::GPL_V3,
      i18n("Copyright (C) 2019 Viet Dang (dangvd@gmail.com), unicornification by Aydin Demircioglu (unicorn@cloned.de)"),
      "",
      "https://github.com/aydindemircioglu/unicorndock");
  KAboutData::setApplicationData(about);
  QApplication::setWindowIcon(QIcon::fromTheme("user-desktop"));

  QSettings::setDefaultFormat(QSettings::IniFormat);
  //QApplication::setApplicationName("unicorndock");
  QApplication::setOrganizationName("unicorndock");
  //QApplication::setOrganizationDomain("");

  ksmoothdock::MultiDockModel model(QDir::homePath() + "/.unicorndock");
  ksmoothdock::MultiDockView view(&model);
  view.show();
  return app.exec();
}
