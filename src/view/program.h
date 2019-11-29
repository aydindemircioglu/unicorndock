/*
 * This file is part of KSmoothDock.
 * Copyright (C) 2019 Viet Dang (dangvd@gmail.com)
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

#ifndef KSMOOTHDOCK_PROGRAM_H_
#define KSMOOTHDOCK_PROGRAM_H_

#include <vector>

#include <QAction>
#include <QMenu>
#include <QPixmap>
#include <QTimer>

#include <KWindowSystem>

#include "icon_based_dock_item.h"

#include <model/multi_dock_model.h>
#include <utils/command_utils.h>

namespace ksmoothdock {

struct ProgramTask {
  WId wId;
  QString name;  // e.g. home -- Dolphin
  bool demandsAttention;

  ProgramTask(WId wId2, QString name2, bool demandsAttention2)
    : wId(wId2), name(name2), demandsAttention(demandsAttention2) {}
};

class Program : public QObject, public IconBasedDockItem {
  Q_OBJECT

 public:
  Program(DockPanel* parent, MultiDockModel* model, const QString& label,
      Qt::Orientation orientation, const QString& iconName, int minSize,
      int maxSize, const QString& command, const QString& taskCommand, bool pinned);

  ~Program() override = default;

  void setLaunching(bool launching) { launching_ = launching; }

  void draw(QPainter* painter, int position, int maxPosition)  override;

  void mousePressEvent(QMouseEvent* e) override;

  QString getLabel() const override;

  bool addTask(const TaskInfo& task) override;

  bool updateTask(const TaskInfo& task) override;

  bool removeTask(WId wId) override;

  bool hasTask(WId wId) override;

  bool beforeTask(const QString& command) override;

  bool shouldBeRemoved() override { return taskCount() == 0 && !pinned_; }

  int taskCount() const { return static_cast<int>(tasks_.size()); }

  bool active() const { return getActiveTask() >= 0; }

  int getActiveTask() const {
    for (int i = 0; i < static_cast<int>(tasks_.size()); ++i) {
      if (KWindowSystem::activeWindow() == tasks_[i].wId) {
        return i;
      }
    }
    return -1;
  }

  bool pinned() { return pinned_; }
  void pinUnpin();

  void launch();
  static void launch(const QString& command);
  static void lockScreen() { launch(kLockScreenCommand); }

 private:
  void createMenu();

  void setDemandsAttention(bool value);
  void updateDemandsAttention();

  MultiDockModel* model_;
  QString name_;
  QString command_;
  QString taskCommand_;
  bool launching_;
  bool pinned_;
  std::vector<ProgramTask> tasks_;

  // Context (right-click) menu.
  QMenu menu_;
  QAction* pinAction_;

  // Demands attention logic.
  bool demandsAttention_;
  QTimer animationTimer_;
  bool attentionStrong_;

  friend class DockPanel;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_PROGRAM_H_
