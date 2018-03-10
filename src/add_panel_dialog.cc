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

#include "add_panel_dialog.h"
#include "ui_add_panel_dialog.h"

#include <QApplication>
#include <QDesktopWidget>

namespace ksmoothdock {

AddPanelDialog::AddPanelDialog(MultiDockModel* model)
    : QDialog(nullptr),
      ui(new Ui::AddPanelDialog),
      model_(model) {
  ui->setupUi(this);

  const int numScreens = QApplication::desktop()->screenCount();
  for (int i = 1; i <= numScreens; ++i) {
    ui->screen->addItem(QString::number(i));
  }
  ui->screen->setCurrentIndex(0);

  const bool isMultiScreen = numScreens > 1;
  ui->screenLabel->setVisible(isMultiScreen);
  ui->screen->setVisible(isMultiScreen);
}

AddPanelDialog::~AddPanelDialog() {
  delete ui;
}

void AddPanelDialog::accept() {
  QDialog::accept();
  model_->addDock(static_cast<PanelPosition>(ui->position->currentIndex()),
                  ui->screen->currentIndex());
}

}  // namespace ksmoothdock
