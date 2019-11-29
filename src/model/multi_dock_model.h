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

#ifndef KSMOOTHDOCK_MULTI_DOCK_MODEL_H_
#define KSMOOTHDOCK_MULTI_DOCK_MODEL_H_

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QColor>
#include <QDir>
#include <QObject>
#include <QString>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>

#include "application_menu_config.h"
#include "config_helper.h"
#include <utils/command_utils.h>

namespace ksmoothdock {

enum class PanelPosition { Top, Bottom, Left, Right };

enum class PanelVisibility { AlwaysVisible, AutoHide, WindowsCanCover,
                             WindowsGoBelow, WindowsCanCover_Quiet };

constexpr int kDefaultMinSize = 128;
constexpr int kDefaultMaxSize = 512;
constexpr float kDefaultSpacingFactor = 0.5;
constexpr int kDefaultTooltipFontSize = 20;
constexpr float kDefaultBackgroundAlpha = 0.42;
constexpr char kDefaultBackgroundColor[] = "#638abd";
constexpr bool kDefaultShowBorder = true;
constexpr char kDefaultBorderColor[] = "#b1c4de";
constexpr float kLargeClockFontScaleFactor = 1.0;
constexpr float kMediumClockFontScaleFactor = 0.8;
constexpr float kSmallClockFontScaleFactor = 0.6;

constexpr PanelVisibility kDefaultVisibility = PanelVisibility::AlwaysVisible;
constexpr bool kDefaultAutoHide = false;
constexpr bool kDefaultShowApplicationMenu = true;
constexpr bool kDefaultShowPager = false;
constexpr bool kDefaultShowTaskManager = true;
constexpr bool kDefaultShowClock = false;

constexpr char kDefaultApplicationMenuName[] = "Applications";
constexpr char kDefaultApplicationMenuIcon[] = "start-here-kde";
constexpr bool kDefaultApplicationMenuStrut = true;
constexpr bool kDefaultShowDesktopNumber = true;
constexpr bool kDefaultCurrentDesktopTasksOnly = true;
constexpr bool kDefaultCurrentScreenTasksOnly = false;
constexpr bool kDefaultUse24HourClock = true;
constexpr float kDefaultClockFontScaleFactor = kLargeClockFontScaleFactor;

struct LauncherConfig {
  QString name;
  QString icon;
  QString command;
  QString taskCommand;

  LauncherConfig() = default;
  LauncherConfig(const QString& name2, const QString& icon2,
                 const QString& command2)
      : name(name2), icon(icon2), command(command2),
        taskCommand(getTaskCommand(command)) {}
  LauncherConfig(const QString& desktopFile);

  // Saves to file in desktop file format.
  void saveToFile(const QString& filePath) const;
};

// The model.
class MultiDockModel : public QObject {
  Q_OBJECT

 public:
  MultiDockModel(const QString& configDir);
  ~MultiDockModel() = default;

  MultiDockModel(const MultiDockModel&) = delete;
  MultiDockModel& operator=(const MultiDockModel&) = delete;

  // Returns the number of docks.
  int dockCount() const { return dockConfigs_.size(); }

  // Adds a new dock in the specified position and screen.
  void addDock(PanelPosition position, int screen, bool showApplicationMenu,
               bool showPager, bool showTaskManager, bool showClock);

  void addDock() {
    addDock(PanelPosition::Bottom, 0, true, true, true, true);
  }

  // Clones an existing dock in the specified position and screen.
  void cloneDock(int srcDockId, PanelPosition position, int screen);

  // Removes a dock.
  void removeDock(int dockId);

  int minIconSize() const {
    return appearanceProperty(kGeneralCategory, kMinimumIconSize,
                              kDefaultMinSize);
  }

  void setMinIconSize(int value) {
    setAppearanceProperty(kGeneralCategory, kMinimumIconSize, value);
  }

  int maxIconSize() const {
    return appearanceProperty(kGeneralCategory, kMaximumIconSize,
                              kDefaultMaxSize);
  }

  void setMaxIconSize(int value) {
    setAppearanceProperty(kGeneralCategory, kMaximumIconSize, value);
  }

  float spacingFactor() const {
    return appearanceProperty(kGeneralCategory, kSpacingFactor, kDefaultSpacingFactor);
  }

  void setSpacingFactor(float value) {
    setAppearanceProperty(kGeneralCategory, kSpacingFactor, value);
  }

  QColor backgroundColor() const {
    QColor defaultBackgroundColor(kDefaultBackgroundColor);
    defaultBackgroundColor.setAlphaF(kDefaultBackgroundAlpha);
    return appearanceProperty(kGeneralCategory, kBackgroundColor,
                              defaultBackgroundColor);
  }

  void setBackgroundColor(const QColor& value) {
    setAppearanceProperty(kGeneralCategory, kBackgroundColor, value);
  }

  bool showBorder() const {
    return appearanceProperty(kGeneralCategory, kShowBorder,
                              kDefaultShowBorder);
  }

  void setShowBorder(bool value) {
    setAppearanceProperty(kGeneralCategory, kShowBorder, value);
  }

  QColor borderColor() const {
    return appearanceProperty(kGeneralCategory, kBorderColor,
                              QColor(kDefaultBorderColor));
  }

  void setBorderColor(const QColor& value) {
    setAppearanceProperty(kGeneralCategory, kBorderColor, value);
  }

  int tooltipFontSize() const {
    return appearanceProperty(kGeneralCategory, kTooltipFontSize,
                              kDefaultTooltipFontSize);
  }

  void setTooltipFontSize(int value) {
    setAppearanceProperty(kGeneralCategory, kTooltipFontSize, value);
  }

  QString applicationMenuName() const {
    return appearanceProperty(kApplicationMenuCategory, kLabel,
                              i18n(kDefaultApplicationMenuName));
  }

  void setApplicationMenuName(const QString& value) {
    setAppearanceProperty(kApplicationMenuCategory, kLabel, value);
  }

  QString applicationMenuIcon() const {
    return appearanceProperty(kApplicationMenuCategory, kIcon,
                              QString(kDefaultApplicationMenuIcon));
  }

  void setApplicationMenuIcon(const QString& value) {
    setAppearanceProperty(kApplicationMenuCategory, kIcon, value);
  }

  bool applicationMenuStrut() const {
    return appearanceProperty(kApplicationMenuCategory, kStrut,
                              kDefaultApplicationMenuStrut);
  }

  void setApplicationMenuStrut(bool value) {
    setAppearanceProperty(kApplicationMenuCategory, kStrut, value);
  }

  QString wallpaper(int desktop, int screen) const {
    return appearanceProperty(kPagerCategory,
                              ConfigHelper::wallpaperConfigKey(desktop, screen),
                              QString());
  }

  void setWallpaper(int desktop, int screen, const QString& value) {
    setAppearanceProperty(kPagerCategory,
                          ConfigHelper::wallpaperConfigKey(desktop, screen),
                          value);
  }

  // Notifies that the wallpaper for the current desktop for the specified
  // screen has been changed.
  void notifyWallpaperChanged(int screen) {
    emit wallpaperChanged(screen);
  }

  bool showDesktopNumber() const {
    return appearanceProperty(kPagerCategory, kShowDesktopNumber,
                              kDefaultShowDesktopNumber);
  }

  void setShowDesktopNumber(bool value) {
    setAppearanceProperty(kPagerCategory, kShowDesktopNumber, value);
  }

  bool currentDesktopTasksOnly() const {
    return appearanceProperty(kTaskManagerCategory, kCurrentDesktopTasksOnly,
                              kDefaultCurrentDesktopTasksOnly);
  }

  void setCurrentDesktopTasksOnly(bool value) {
    setAppearanceProperty(kTaskManagerCategory, kCurrentDesktopTasksOnly, value);
  }

  bool currentScreenTasksOnly() const {
    return appearanceProperty(kTaskManagerCategory, kCurrentScreenTasksOnly,
                              kDefaultCurrentScreenTasksOnly);
  }

  void setCurrentScreenTasksOnly(bool value) {
    setAppearanceProperty(kTaskManagerCategory, kCurrentScreenTasksOnly, value);
  }

  bool use24HourClock() const {
    return appearanceProperty(kClockCategory, kUse24HourClock,
                              kDefaultUse24HourClock);
  }

  void setUse24HourClock(bool value) {
    setAppearanceProperty(kClockCategory, kUse24HourClock, value);
  }

  float clockFontScaleFactor() const {
    return appearanceProperty(kClockCategory, kFontScaleFactor,
                              kDefaultClockFontScaleFactor);
  }

  void setClockFontScaleFactor(float value) {
    setAppearanceProperty(kClockCategory, kFontScaleFactor, value);
  }

  void saveAppearanceConfig(bool repaintOnly = false) {
    syncAppearanceConfig();
    if (repaintOnly) {
      emit appearanceOutdated();
    } else {
      emit appearanceChanged();
    }
  }

  PanelPosition panelPosition(int dockId) const {
    return static_cast<PanelPosition>(dockProperty(
        dockId, kGeneralCategory, kPosition,
        static_cast<int>(PanelPosition::Bottom)));
  }

  void setPanelPosition(int dockId, PanelPosition value) {
    setDockProperty(dockId, kGeneralCategory, kPosition,
                    static_cast<int>(value));
  }

  int screen(int dockId) const {
    return dockProperty(dockId, kGeneralCategory, kScreen, 0);
  }

  void setScreen(int dockId, int value) {
    setDockProperty(dockId, kGeneralCategory, kScreen, value);
  }

  PanelVisibility visibility(int dockId) const {
    if (autoHide(dockId)) {  // for backward compatibility.
      return PanelVisibility::AutoHide;
    }
    return static_cast<PanelVisibility>(dockProperty(
        dockId, kGeneralCategory, kVisibility,
        static_cast<int>(kDefaultVisibility)));
  }

  void setVisibility(int dockId, PanelVisibility value) {
    setDockProperty(dockId, kGeneralCategory, kVisibility,
                    static_cast<int>(value));
    // For backward compatibility.
    setAutoHide(dockId, value == PanelVisibility::AutoHide);
  }

  bool autoHide(int dockId) const {
    return dockProperty(dockId, kGeneralCategory, kAutoHide, kDefaultAutoHide);
  }

  void setAutoHide(int dockId, bool value) {
    setDockProperty(dockId, kGeneralCategory, kAutoHide, value);
  }

  bool showApplicationMenu(int dockId) const {
    return dockProperty(dockId, kGeneralCategory, kShowApplicationMenu,
                        kDefaultShowApplicationMenu);
  }

  void setShowApplicationMenu(int dockId, bool value) {
    setDockProperty(dockId, kGeneralCategory, kShowApplicationMenu, value);
  }

  bool showPager(int dockId) const {
    return dockProperty(dockId, kGeneralCategory, kShowPager,
                        kDefaultShowPager);
  }

  void setShowPager(int dockId, bool value) {
    setDockProperty(dockId, kGeneralCategory, kShowPager, value);
  }

  bool showTaskManager(int dockId) const {
    return dockProperty(dockId, kGeneralCategory, kShowTaskManager,
                        kDefaultShowTaskManager);
  }

  void setShowTaskManager(int dockId, bool value) {
    setDockProperty(dockId, kGeneralCategory, kShowTaskManager, value);
  }

  bool showClock(int dockId) const {
    return dockProperty(dockId, kGeneralCategory, kShowClock,
                        kDefaultShowClock);
  }

  void setShowClock(int dockId, bool value) {
    setDockProperty(dockId, kGeneralCategory, kShowClock, value);
  }

  void saveDockConfig(int dockId) {
    syncDockConfig(dockId);
    // No need to emit signal here.
  }

  QString dockLaunchersPath(int dockId) const {
    return std::get<2>(dockConfigs_.at(dockId));
  }

  const std::vector<LauncherConfig> dockLauncherConfigs(int dockId) const {
    return std::get<3>(dockConfigs_.at(dockId));
  }

  void setDockLauncherConfigs(
      int dockId, const std::vector<LauncherConfig>& launcherConfigs) {
    std::get<3>(dockConfigs_[dockId]) = launcherConfigs;
  }

  void saveDockLauncherConfigs(int dockId) {
    syncDockLaunchersConfig(dockId);
    emit dockLaunchersChanged(dockId);
  }

  void addLauncher(int dockId, const LauncherConfig& launcher) {
    auto& launchers = std::get<3>(dockConfigs_[dockId]);
    unsigned int i = 0;
    for (; i < launchers.size() && launchers[i].taskCommand < launcher.taskCommand; ++i) {}
    launchers.insert(launchers.begin() + i, launcher);
    syncDockLaunchersConfig(dockId);
  }

  void removeLauncher(int dockId, const QString& command) {
    auto& launchers = std::get<3>(dockConfigs_[dockId]);
    for (unsigned i = 0; i < launchers.size(); ++i) {
      if (launchers[i].command == command) {
        launchers.erase(launchers.begin() + i);
        syncDockLaunchersConfig(dockId);
        return;
      }
    }
  }

  // Whether any dock has a pager.
  bool hasPager() const;

  const std::vector<Category>& applicationMenuCategories() const {
    return applicationMenuConfig_.categories();
  }

  const ApplicationEntry* findApplication(const QString& command) const {
    return applicationMenuConfig_.findApplication(command);
  }

 signals:
  // Minor appearance changes that require view update (repaint).
  void appearanceOutdated();
  // Major appearance changes that require view reload.
  void appearanceChanged();
  void dockAdded(int dockId);
  void dockLaunchersChanged(int dockId);
  // Wallpaper for the current desktop for screen <screen> has been changed.
  // Will require calling Plasma D-Bus to update the wallpaper.
  void wallpaperChanged(int screen);
  void applicationMenuConfigChanged();

 private:
  // Dock config's categories/properties.
  static constexpr char kGeneralCategory[] = "General";
  static constexpr char kAutoHide[] = "autoHide";
  static constexpr char kVisibility[] = "visibility";
  static constexpr char kPosition[] = "position";
  static constexpr char kScreen[] = "screen";
  static constexpr char kShowApplicationMenu[] = "showApplicationMenu";
  static constexpr char kShowClock[] = "showClock";
  static constexpr char kShowPager[] = "showPager";
  static constexpr char kShowTaskManager[] = "showTaskManager";

  // Global appearance config's categories/properties.

  // General category.
  static constexpr char kBackgroundColor[] = "backgroundColor";
  static constexpr char kBorderColor[] = "borderColor";
  static constexpr char kMaximumIconSize[] = "maximumIconSize";
  static constexpr char kMinimumIconSize[] = "minimumIconSize";
  static constexpr char kSpacingFactor[] = "spacingFactor";
  static constexpr char kShowBorder[] = "showBorder";
  static constexpr char kTooltipFontSize[] = "tooltipFontSize";

  static constexpr char kApplicationMenuCategory[] = "Application Menu";
  static constexpr char kIcon[] = "icon";
  static constexpr char kLabel[] = "label";
  static constexpr char kStrut[] = "strut";

  static constexpr char kPagerCategory[] = "Pager";
  static constexpr char kWallpaper[] = "wallpaper";
  static constexpr char kShowDesktopNumber[] = "showDesktopNumber";

  static constexpr char kTaskManagerCategory[] = "TaskManager";
  static constexpr char kCurrentDesktopTasksOnly[] = "currentDesktopTasksOnly";
  static constexpr char kCurrentScreenTasksOnly[] = "currentScreenTasksOnly";

  static constexpr char kClockCategory[] = "Clock";
  static constexpr char kUse24HourClock[] = "use24HourClock";
  static constexpr char kFontScaleFactor[] = "fontScaleFactor";

  template <typename T>
  T appearanceProperty(QString category, QString name, T defaultValue) const {
    KConfigGroup group(&appearanceConfig_, category);
    return group.readEntry(name, defaultValue);
  }

  template <typename T>
  void setAppearanceProperty(QString category, QString name, T value) {
    KConfigGroup group(&appearanceConfig_, category);
    group.writeEntry(name, value);
  }

  template <typename T>
  T dockProperty(int dockId, QString category, QString name, T defaultValue)
      const {
    KConfigGroup group(dockConfig(dockId), category);
    return group.readEntry(name, defaultValue);
  }

  template <typename T>
  void setDockProperty(int dockId, QString category, QString name, T value) {
    KConfigGroup group(dockConfig(dockId), category);
    group.writeEntry(name, value);
  }

  QString dockConfigPath(int dockId) const {
    return std::get<0>(dockConfigs_.at(dockId));
  }

  const KConfig* dockConfig(int dockId) const {
    return std::get<1>(dockConfigs_.at(dockId)).get();
  }

  KConfig* dockConfig(int dockId) {
    return std::get<1>(dockConfigs_[dockId]).get();
  }

  static std::vector<LauncherConfig> loadDockLaunchers(
      const QString& dockLaunchersPath);

  static std::vector<LauncherConfig> createDefaultLaunchers();

  void loadDocks();

  int addDock(const std::tuple<QString, QString>& configs,
              PanelPosition position, int screen);

  void syncAppearanceConfig() {
    appearanceConfig_.sync();
  }

  void syncDockConfig(int dockId) {
    dockConfig(dockId)->sync();
  }

  void syncDockLaunchersConfig(int dockId);

  static void copyEntry(const QString& key, const KConfigGroup& sourceGroup,
                        KConfigGroup* destGroup) {
    destGroup->writeEntry(key, sourceGroup.readEntry(key));
  }

  // Converts the old single-dock config to the new multi-dock config if needed.
  bool convertConfig();

  // Helper(s).
  ConfigHelper configHelper_;

  // Model data.

  // Appearance config.
  KConfig appearanceConfig_;

  // Dock configs, as map from dockIds to tuples of:
  // (dock config file path,
  //  dock config,
  //  launchers dir path,
  //  list of launcher configs)
  std::unordered_map<int,
                     std::tuple<QString,
                                std::unique_ptr<KConfig>,
                                QString,
                                std::vector<LauncherConfig>>> dockConfigs_;

  // ID for the next dock.
  int nextDockId_;

  ApplicationMenuConfig applicationMenuConfig_;
};

}  // namespace ksmoothdock

#endif  // KSMOOTHDOCK_MULTI_DOCK_MODEL_H_
