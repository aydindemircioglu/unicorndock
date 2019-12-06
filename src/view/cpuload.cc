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

#include "cpuload.h"

#include <algorithm>

#include <QColor>
#include <QDate>
#include <QFont>
#include <QIcon>
#include <QTime>
#include <QTimer>

#include <KLocalizedString>

#include "dock_panel.h"
#include "program.h"
#include <utils/draw_utils.h>
#include <utils/font_utils.h>

namespace ksmoothdock {

constexpr float CpuLoad::kWhRatio;
constexpr float CpuLoad::kDelta;

CpuLoad::CpuLoad(DockPanel* parent, MultiDockModel* model,
             Qt::Orientation orientation, int minSize, int maxSize)
    : IconlessDockItem(parent, "" /* label */, orientation, minSize, maxSize,
                       kWhRatio),
      model_(model),
      calendar_(parent) {
  createMenu();
  loadConfig();

  QTimer* timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
  timer->start(1000);  // update the time every second.
}

void CpuLoad::draw(QPainter *painter,  int position, int maxPosition)  {
  const QString timeFormat = model_->use24HourClock() ? "hh:mm" : "hh:mm AP";
  const QString time = QTime::currentTime().toString(timeFormat);
  // The reference time used to calculate the font size.
  const QString referenceTime = QTime(8, 8).toString(timeFormat);

  painter->setFont(adjustFontSize(getWidth(), getHeight(), referenceTime,
                                  model_->clockFontScaleFactor()));
  painter->setRenderHint(QPainter::TextAntialiasing);

  if (size_ > minSize_) {
    drawBorderedText(left_, top_, getWidth(), getHeight(), Qt::AlignCenter,
                     time, 2 /* borderWidth */, Qt::black, Qt::white, painter);
  } else {
    painter->setPen(Qt::white);
    painter->drawText(left_, top_, getWidth(), getHeight(), Qt::AlignCenter,
                      time);
  }
}

void CpuLoad::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    calendar_.toggleCalendar();
  } else if (e->button() == Qt::RightButton) {
    // In case other docks have changed the config.
    loadConfig();
    menu_.popup(e->globalPos());
  }
}

QString CpuLoad::getLabel() const {
  return QDate::currentDate().toString(Qt::SystemLocaleLongDate);
}

void CpuLoad::updateTime() {
  parent_->update();
}

void CpuLoad::setDateAndTime() {
  Program::launch("kcmshell5 clock");
}

void CpuLoad::setFontScaleFactor(float fontScaleFactor) {
  largeFontAction_->setChecked(
      fontScaleFactor > kLargeClockFontScaleFactor - kDelta);
  mediumFontAction_->setChecked(
      fontScaleFactor > kMediumClockFontScaleFactor - kDelta &&
      fontScaleFactor < kMediumClockFontScaleFactor + kDelta);
  smallFontAction_->setChecked(
      fontScaleFactor < kSmallClockFontScaleFactor + kDelta);
}

void CpuLoad::setLargeFont() {
  setFontScaleFactor(kLargeClockFontScaleFactor);
  saveConfig();
}

void CpuLoad::setMediumFont() {
  setFontScaleFactor(kMediumClockFontScaleFactor);
  saveConfig();
}

void CpuLoad::setSmallFont() {
  setFontScaleFactor(kSmallClockFontScaleFactor);
  saveConfig();
}

void CpuLoad::createMenu() {
  menu_.addAction(QIcon::fromTheme("preferences-system-time"),
                  i18n("Date and Time &Settings"),
                  this,
                  SLOT(setDateAndTime()));

  use24HourClockAction_ = menu_.addAction(
      i18n("Use 24-hour Clock"), this,
      [this] {
        saveConfig();
      });
  use24HourClockAction_->setCheckable(true);

  QMenu* fontSize = menu_.addMenu(i18n("Font Size"));
  largeFontAction_ = fontSize->addAction(i18n("Large Font"),
                                         this,
                                         SLOT(setLargeFont()));
  largeFontAction_->setCheckable(true);
  mediumFontAction_ = fontSize->addAction(i18n("Medium Font"),
                                          this,
                                          SLOT(setMediumFont()));
  mediumFontAction_->setCheckable(true);
  smallFontAction_ = fontSize->addAction(i18n("Small Font"),
                                         this,
                                         SLOT(setSmallFont()));
  smallFontAction_->setCheckable(true);

  menu_.addSeparator();
  parent_->addPanelSettings(&menu_);
}

void CpuLoad::loadConfig() {
  use24HourClockAction_->setChecked(model_->use24HourClock());
  setFontScaleFactor(model_->clockFontScaleFactor());
}

void CpuLoad::saveConfig() {
  model_->setUse24HourClock(use24HourClockAction_->isChecked());
  model_->setClockFontScaleFactor(fontScaleFactor());
  model_->saveAppearanceConfig(true /* repaintOnly */);
}

}  // namespace ksmoothdock
