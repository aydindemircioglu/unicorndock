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

#ifndef KSMOOTHDOCK_CPULOAD_H_
#define KSMOOTHDOCK_CPULOAD_H_

#include "iconless_dock_item.h"

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QString>

#include "calendar.h"
#include <model/multi_dock_model.h>

namespace ksmoothdock {

// A digital clock.
class CpuLoad : public QObject, public IconlessDockItem {
  Q_OBJECT

 public:
  CpuLoad(DockPanel* parent, MultiDockModel* model, Qt::Orientation orientation,
        int minSize, int maxSize);
  virtual ~CpuLoad() = default;

  void draw(QPainter* painter, int position, int maxPosition) override;
  void mousePressEvent(QMouseEvent* e) override;
  void loadConfig() override;
  QString getLabel() const override;
  bool beforeTask(const QString& command) override { return false; }

 public slots:
  void updateTime();

  void setDateAndTime();

  void setFontScaleFactor(float fontScaleFactor);
  void setLargeFont();
  void setMediumFont();
  void setSmallFont();

 private:
  static constexpr float kWhRatio = 2.8;
  static constexpr float kDelta = 0.01;

  float fontScaleFactor() {
    return largeFontAction_->isChecked()
        ? kLargeClockFontScaleFactor
        : mediumFontAction_->isChecked() ? kMediumClockFontScaleFactor
                                         : kSmallClockFontScaleFactor;
  }

  // Creates the context menu.
  void createMenu();

  void saveConfig();

  MultiDockModel* model_;

  Calendar calendar_;

  // Context menu.
  QMenu menu_;

  QAction* use24HourClockAction_;
  QAction* largeFontAction_;
  QAction* mediumFontAction_;
  QAction* smallFontAction_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_CLOCK_H_
