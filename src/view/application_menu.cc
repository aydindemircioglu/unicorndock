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

#include "application_menu.h"

#include <algorithm>

#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QStringBuilder>
#include <QUrl>
#include <iostream>

#include <KDesktopFile>
#include <KIconLoader>
#include <KLocalizedString>
#include <KWindowSystem>

#include "dock_panel.h"
#include "program.h"
#include <utils/draw_utils.h>

namespace ksmoothdock {

int ApplicationMenuStyle::pixelMetric(
    PixelMetric metric, const QStyleOption *option, const QWidget *widget)
    const {
  if (metric == QStyle::PM_SmallIconSize) {
    return kApplicationMenuIconSize;
  }
  return QProxyStyle::pixelMetric(metric, option, widget);
}

ApplicationMenu::ApplicationMenu(
    DockPanel *parent, MultiDockModel* model, Qt::Orientation orientation,
    int minSize, int maxSize)
    : IconBasedDockItem(parent, "" /* label */, orientation, "" /* iconName */,
                        minSize, maxSize),
      model_(model),
      showingMenu_(false) {
  menu_.setStyle(&style_);
  menu_.setStyleSheet(getStyleSheet());

  loadConfig();
  buildMenu();

  createContextMenu();

  connect(&menu_, SIGNAL(aboutToShow()), parent_,
          SLOT(setStrutForApplicationMenu()));
  connect(&menu_, &QMenu::aboutToShow, this,
          [this]() { showingMenu_ = true; } );
  connect(&menu_, SIGNAL(aboutToHide()), parent_, SLOT(setStrut()));
  connect(&menu_, &QMenu::aboutToHide, this,
          [this]() { showingMenu_ = false; } );
  connect(model_, SIGNAL(applicationMenuConfigChanged()),
          this, SLOT(reloadMenu()));
}

void ApplicationMenu::draw(QPainter* painter, int position, int maxPosition)  {
  std::cout << "AppMenu " << position << "\n";
  if (showingMenu_) {
    drawHighlightedIcon(model_->backgroundColor(), left_, top_, getWidth(), getHeight(),
                        minSize_ / 4 - 4, size_ / 8, painter);
  }
  IconBasedDockItem::draw(painter, position, maxPosition);
}

void ApplicationMenu::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::LeftButton) {
    menu_.popup(parent_->applicationMenuPosition(getMenuSize()));
  } else if (e->button() == Qt::RightButton) {
    contextMenu_.popup(e->globalPos());
  }
}

void ApplicationMenu::reloadMenu() {
  menu_.clear();
  buildMenu();
}

bool ApplicationMenu::eventFilter(QObject* object, QEvent* event) {
  QMenu* menu = dynamic_cast<QMenu*>(object);
  if (menu) {
    if (event->type() == QEvent::Show) {
      menu->popup(parent_->applicationSubMenuPosition(getMenuSize(),
                                                         menu->geometry()));
      // Filter this event.
      return true;
    } else if (event->type() == QEvent::MouseButtonPress) {
      QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
      if (mouseEvent && mouseEvent->button() == Qt::LeftButton
          && menu->activeAction()) {
        startMousePos_ = mouseEvent->pos();
        draggedEntry_ = menu->activeAction()->data().toString();
      }
    } else if (event->type() == QEvent::MouseMove) {
      QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
      if (mouseEvent && mouseEvent->buttons() & Qt::LeftButton) {
        const int distance
            = (mouseEvent->pos() - startMousePos_).manhattanLength();
        if (distance >= QApplication::startDragDistance()
            && !draggedEntry_.isEmpty()) {
          // Start drag.
          QMimeData* mimeData = new QMimeData;
          mimeData->setData("text/uri-list",
                            QUrl::fromLocalFile(draggedEntry_).toEncoded());

          QDrag* drag = new QDrag(this);
          drag->setMimeData(mimeData);
          drag->exec(Qt::CopyAction);
        }
      }
    }
  }

  return QObject::eventFilter(object, event);
}

QString ApplicationMenu::getStyleSheet() {
  QColor bgColor = parent_->backgroundColor();
  bgColor.setAlphaF(0.42);
  QColor borderColor = parent_->borderColor();
  return " \
QMenu { \
  background-color: " % bgColor.name(QColor::HexArgb) % ";"
" margin: 1px; \
  padding: 2px; \
  border: 1px transparent; \
  border-radius: 3px; \
} \
\
QMenu::item { \
  font: bold; \
  color: white; \
  background-color: transparent; \
  padding: 4px 45px 4px 45px; \
} \
\
QMenu::item:selected { \
  background-color: " % bgColor.name(QColor::HexArgb) % ";"
" border: 1px solid " % borderColor.name() % ";"
" border-radius: 3px; \
} \
\
QMenu::separator { \
  margin: 5px; \
  height: 1px; \
  background: " % borderColor.name() % ";"
"}";
}

void ApplicationMenu::loadConfig() {
  setLabel(model_->applicationMenuName());
  setIconName(model_->applicationMenuIcon());
}

void ApplicationMenu::buildMenu() {
  addToMenu(model_->applicationMenuCategories());
  menu_.addSeparator();
  addToMenu(ApplicationMenuConfig::kSessionSystemCategories);
  addEntry(ApplicationMenuConfig::kSearchEntry, &menu_);
}

void ApplicationMenu::addToMenu(const std::vector<Category>& categories) {
  for (const auto& category : categories) {
    if (category.entries.empty()) {
      continue;
    }

    QMenu* menu = menu_.addMenu(loadIcon(category.icon), category.displayName);
    menu->setStyle(&style_);
    for (const auto& entry : category.entries) {
      addEntry(entry, menu);
    }
    menu->installEventFilter(this);
  }
}

void ApplicationMenu::addEntry(const ApplicationEntry &entry, QMenu *menu) {
  QAction* action = menu->addAction(loadIcon(entry.icon), entry.name, this,
                  [&entry]() {
                    Program::launch(entry.command);
                  });
  action->setData(entry.desktopFile);
}

QIcon ApplicationMenu::loadIcon(const QString &icon) {
  return QIcon(KIconLoader::global()->loadIcon(
      icon, KIconLoader::NoGroup, kApplicationMenuIconSize));
}

void ApplicationMenu::createContextMenu() {
  contextMenu_.addAction(QIcon::fromTheme("configure"),
                         i18n("Application Menu &Settings"),
                         parent_,
                         [this] { parent_->showApplicationMenuSettingsDialog(); });
  contextMenu_.addSeparator();
  parent_->addPanelSettings(&contextMenu_);
}

}  // namespace ksmoothdock
