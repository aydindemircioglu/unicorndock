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

#ifndef KSMOOTHDOCK_DESKTOP_SELECTOR_H_
#define KSMOOTHDOCK_DESKTOP_SELECTOR_H_

#include "icon_based_dock_item.h"

#include <memory>

#include <QAction>
#include <QMenu>
#include <QObject>
#include <QString>

#include <KWindowSystem>

#include <model/multi_dock_model.h>

namespace ksmoothdock {

// Pager icons.
class DesktopSelector : public QObject, public IconBasedDockItem {
  Q_OBJECT

 public:
  DesktopSelector(DockPanel* parent, MultiDockModel* model,
                  Qt::Orientation orientation, int minSize, int maxSize,
                  int desktop, int screen);

  virtual ~DesktopSelector() = default;

  int getWidthForSize(int size) const override {
    return isHorizontal() ? (size * desktopWidth_ / desktopHeight_) : size;
  }

  int getHeightForSize(int size) const override {
    return isHorizontal() ? size : (size * desktopHeight_ / desktopWidth_);
  }

  void draw(QPainter* painter, int position, int maxPosition) override;
  void mousePressEvent(QMouseEvent* e) override;
  void loadConfig() override;

  // Sets the icon but scales the pixmap to the screen's width/height ratio.
  void setIconScaled(const QPixmap& icon);

 private:
  bool isCurrentDesktop() const {
    return KWindowSystem::currentDesktop() == desktop_;
  }

  void createMenu();

  void saveConfig();

  MultiDockModel* model_;

  // The desktop that this desktop selector manages, 1-based.
  int desktop_;
  // The screen that the parent panel is on, 0-based.
  int screen_;
  KConfig* config_;

  // Context (right-click) menu.
  QMenu menu_;

  QAction* showDesktopNumberAction_;

  int desktopWidth_;
  int desktopHeight_;

  bool hasCustomWallpaper_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_DESKTOP_SELECTOR_H_
