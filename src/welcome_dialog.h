/*
 * This file is part of KSmoothDock.
 * Copyright (C) 2018 Viet Dang (dangvd@gmail.com)
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

#ifndef KSMOOTHDOCK_WELCOME_DIALOG_H_
#define KSMOOTHDOCK_WELCOME_DIALOG_H_

#include <QDialog>

#include "multi_dock_model.h"

namespace Ui {
  class WelcomeDialog;
}

namespace ksmoothdock {

class WelcomeDialog : public QDialog {
  Q_OBJECT

 public:
  explicit WelcomeDialog(MultiDockModel* model);
  ~WelcomeDialog();

 public slots:
  void accept() override;

 private:
  Ui::WelcomeDialog *ui;

  MultiDockModel* model_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_WELCOME_DIALOG_H_
