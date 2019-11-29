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

#ifndef KSMOOTHDOCK_APPLICATION_MENU_H_
#define KSMOOTHDOCK_APPLICATION_MENU_H_

#include "icon_based_dock_item.h"

#include <QEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPoint>
#include <QProxyStyle>
#include <QSize>
#include <QString>

#include <model/application_menu_config.h>
#include <model/multi_dock_model.h>

namespace ksmoothdock {

constexpr int kApplicationMenuIconSize = 32;

class ApplicationMenuStyle : public QProxyStyle {
 public:
  int pixelMetric(PixelMetric metric, const QStyleOption *option = Q_NULLPTR,
                  const QWidget *widget = Q_NULLPTR) const override;
};


// The application menu item on the dock.
//
// Left-clicking the item shows a cascading popup menu that contains entries
// for all applications organized by categories. The menu uses a custom style
// e.g. bigger icon size and the same translucent effect as the dock's.
//
// Supports drag-and-drop as a drag source.
// What it means is that you can drag an application entry from the menu
// to other widgets/applications. It doesn't support drag-and-drop within the
// menu itself.
class ApplicationMenu : public QObject, public IconBasedDockItem {
  Q_OBJECT

 public:
  ApplicationMenu(
      DockPanel* parent,
      MultiDockModel* model,
      Qt::Orientation orientation,
      int minSize,
      int maxSize);
  virtual ~ApplicationMenu() = default;

  void draw(QPainter* painter, int position, int maxPosition)  override;
  void mousePressEvent(QMouseEvent* e) override;
  void loadConfig() override;

  QSize getMenuSize() { return menu_.sizeHint(); }

public slots:
 void reloadMenu();

protected:
  // Intercepts sub-menus's show events to adjust their position to improve
  // visibility.
  bool eventFilter(QObject* object, QEvent* event) override;

 private:
  QString getStyleSheet();

  QIcon loadIcon(const QString& icon);

  // Builds the menu from the application entries;
  void buildMenu();
  void addToMenu(const std::vector<Category>& categories);
  void addEntry(const ApplicationEntry& entry, QMenu* menu);

  void createContextMenu();

  MultiDockModel* model_;

  // The cascading popup menu that contains all application entries.
  QMenu menu_;
  bool showingMenu_;

  ApplicationMenuStyle style_;

  // Drag support.

  // Starting mouse position, used for minimum drag distance check.
  QPoint startMousePos_;
  // The desktop file associated with the application entry being dragged.
  QString draggedEntry_;

  // Context (right-click) menu.
  QMenu contextMenu_;
};

}  // namespace ksmoothdock

#endif // KSMOOTHDOCK_APPLICATION_MENU_H_
