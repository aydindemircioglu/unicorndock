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

#include "icon_based_dock_item.h"

#include <KIconLoader>
#include <iostream>
#include <math.h>

#include <QImage>
#include <QDir>
#include <QSettings>
#include <QString>


namespace ksmoothdock {

const int IconBasedDockItem::kIconLoadSize;

IconBasedDockItem::IconBasedDockItem(DockPanel* parent, const QString& label, Qt::Orientation orientation,
                  const QString& iconName, int minSize, int maxSize)
    : DockItem(parent, label, orientation, minSize, maxSize),
    icons_(maxSize - minSize + 1),
    iconsHeights_ (maxSize - minSize + 1),
    iconsWidths_ (maxSize - minSize + 1) {
  position_ = -1;
  setIconName(iconName);
  QSettings settings;
  std::cout << "Reading settings from "  << settings.fileName().toStdString() << "\n";
}

IconBasedDockItem::IconBasedDockItem(DockPanel* parent, const QString& label,
    Qt::Orientation orientation, const QPixmap& icon,
    int minSize, int maxSize)
    : DockItem(parent, label, orientation, minSize, maxSize),
    icons_(maxSize - minSize + 1),
    iconsHeights_ (maxSize - minSize + 1),
    iconsWidths_ (maxSize - minSize + 1) {
  position_ = -1;
  setIcon(icon);
  QSettings settings;
  std::cout << "Reading settings from "  << settings.fileName().toStdString() << "\n";
}


void IconBasedDockItem::recolorIcon (QImage& img, int position, int maxPosition) {

    // determine new color first
    double relPos = position/(double)maxPosition;
    int palLen = 13;
    int hueMap[] = {240, 220, 190, 130, 80, 60, 40, 20, 0,  -20, -40, -60, -80};
    int hueMin = (int) floor(relPos*(palLen-2));
    int hueMax = hueMin + 1;
    double hueWeight = relPos*(palLen-2) - hueMin;
    double hue = round (hueWeight * hueMap[hueMax] + (1 - hueWeight) * hueMap[hueMin]);
    // 				// hue should go from 240 downto 0--360 downto 300
    // 				double hue = 240 - relPos*300;

    if (hue < 0)
      hue = 360 + hue;


    double satMap[] = {0.82, 0.82, 0.82, 0.84, 0.88, 0.92, 0.94, 0.92, 0.88, 0.86, 0.84, 0.82, 0.82};
    double sat = (hueWeight * satMap[hueMax] + (1 - hueWeight) * satMap[hueMin]);

    double valMap[] =  {0.75, 0.75, 0.78, 0.84, 0.88, 0.92, 0.94, 0.92, 0.88, 0.78, 0.71, 0.66, 0.66};
    double val = (hueWeight * valMap[hueMax] + (1 - hueWeight) * valMap[hueMin]);

    // HsvToRgb(hue, sat, val, out newColorR, out newColorG, out newColorB);
    //int alpha = colour.alpha();
    QColor tmpColor (0,0,0);
    int ihue = int(round(hue));
    int isat = int(round(sat*255));
    int ival = int(round(val*255));
    tmpColor.setHsv(ihue, isat, ival);
    int newColorR = 0;
    int newColorG = 0;
    int newColorB = 0;
    tmpColor.getRgb (&newColorR, &newColorG, &newColorB);

    // QImage finalImage(img);
    for (int y = 0; y < img.height(); ++y) {
      QRgb *line = reinterpret_cast<QRgb*>(img.scanLine(y));
      for (int x = 0; x < img.width(); ++x) {
          QRgb pixelRgb = line[x];

					// check if pixel is whitish by checking the difference in R, G, B, the max is 255+255
          int r = qRed (pixelRgb);
          int g = qGreen (pixelRgb);
          int b = qBlue (pixelRgb);
          int alpha = qAlpha (pixelRgb);
					double whiteAlpha = abs(r-b) + abs(b-g) + abs(r-g);
          whiteAlpha = whiteAlpha/(255 + 255);

					if (whiteAlpha > 0.19) {
            // r = byte(round(newColorR * whiteAlpha + 255*(1.0 - whiteAlpha)));
            // r.. = byte(round(newColorR * whiteAlpha + 255*(1.0 - whiteAlpha)));
            // r.. = byte(round(newColorR * whiteAlpha + 255*(1.0 - whiteAlpha)));
            r = newColorR;
            g = newColorG;
            b = newColorB;
					} else {
						int whitish = (int) round (255 * (double)(r+g+b)/(double)(255*3));
            r = whitish;
            g = whitish;
            b = whitish;
					}

          QColor final(r, g, b, alpha);
          line[x] = final.rgba();
      }
    }

// 						if (whiteAlpha > 0.19) {
// 							pixels[0] = (byte) Math.Round(newColorR * whiteAlpha + 255  * (1.0 - whiteAlpha));
// 							pixels[1] = (byte) Math.Round(newColorG * whiteAlpha + 255 * (1.0 - whiteAlpha));
// 							pixels[2] = (byte) Math.Round(newColorB * whiteAlpha + 255  * (1.0 - whiteAlpha));
// 							pixels[0] = (byte) newColorR;
// 							pixels[1] = (byte) newColorG;
// 							pixels[2] = (byte) newColorB;
// 						}  else {
// 							byte whitish = (byte) Math.Round ( 255 * (double) (pixels[0] + pixels[1] + pixels[2])/(double)(255*3) );
// //							whitish = 255;
// 							pixels[0] = (byte) whitish;
// 							pixels[1] = (byte) whitish;
// 							pixels[2] = (byte) whitish;
// 						}
//
// 						pixels += 4;
//
//
//         colour.setHsv(ihue, isat, ival, alpha);
//
//
//
//         image.setPixel(x, y, colour.rgba());
  // return (pixmap);
}


void IconBasedDockItem::draw(QPainter* painter, int position, int maxPosition) {
  // std::cout << "painting..\n";
  // need the position of us
  // if position changes we do something
  if (position != position_) {
    std::cout << "position change from " << position_ << " to " << position << "\n";
    position_ = position;

    // if new position is odd, we grey it out
    image_ = originalImage_.copy();
    recolorIcon (image_, position_, maxPosition);

    // this will invalidate the cache too
    mipmapIcons (image_);
  }

  // create mipmap if needed
  if (icons_[size_ - minSize_].height() == 0) {
    updateIconCache (size_, minSize_);
  }

  painter->drawPixmap(left_, top_, icons_[size_ - minSize_]);
}


void IconBasedDockItem::updateIconCache (int size, int minSize_) {
  icons_[size - minSize_] = QPixmap::fromImage(
    (orientation_ == Qt::Horizontal)
        ? image_.scaledToHeight(size, Qt::SmoothTransformation)
        : image_.scaledToWidth(size, Qt::SmoothTransformation));
}


void IconBasedDockItem::setIcon(const QPixmap& icon) {
  std::cout << "Setting icon from pixmap!\n";
  generateIcons(icon);
}


void IconBasedDockItem::setIconName(const QString& iconName) {
  std::cout << "# label is: " << label_.toStdString() << "\n";

  if (!iconName.isEmpty()) {
    iconName_ = iconName;

    // overrides, maybe load these from a .json or something
    QString pngName (iconName);
    QSettings settings;
    settings.beginGroup("iconoverrides");
    // QStringList childKeys = settings.childGroups();
    QStringList childKeys = settings.childKeys();
    for (const auto& key:childKeys) {
      // do override
      if (label_ == key) {
        pngName = settings.value(key).toString();
      }
      // std::cout << "XXX: " << key.toStdString() << "\n";
      // std::cout << "XXX: " << settings.value(key).toString().toStdString() << "\n";
    }

    std::string newIconPath = "/home/aydin/.icons/Moka/stash/" + pngName.toStdString() + ".png";
    QString qstr = QString::fromStdString(newIconPath);
    std::cout << "Loading from " << newIconPath << " here.\n";
    QPixmap icon;
    icon.load (qstr);
    std::cout << "Icon has size " << icon.height() << "x" << icon.width() << ".\n";
    if (icon.height() == 0) {
        // load stub
        std::string newIconPath = "/home/aydin/.icons/Moka/stash/kchmviewer.png";
        QString qstr = QString::fromStdString(newIconPath);
        icon.load(qstr);
        std::cout << "Reloaded con has size " << icon.height() << "x" << icon.width() << ".\n";
    }
    setIcon(icon);
  }
}

const QPixmap& IconBasedDockItem::getIcon(int size) const {
  if (size < minSize_) {
    size = minSize_;
  } else if (size > maxSize_) {
    size = maxSize_;
  }
  std::cout << "THROW EXCEPTION: getting icons here!" << "\n";
  return icons_[size - minSize_];
}

void IconBasedDockItem::generateIcons(const QPixmap& icon) {
  originalImage_ = icon.toImage(); // Convert to QImage for fast scaling.
  mipmapIcons (originalImage_);
}

void IconBasedDockItem::mipmapIcons (const QImage& image) {
  for (int size = minSize_; size <= maxSize_; ++size) {
    icons_[size - minSize_] = QPixmap::fromImage(
        (orientation_ == Qt::Horizontal)
            ? image.scaledToHeight(size, Qt::SmoothTransformation)
            : image.scaledToWidth(size, Qt::SmoothTransformation));
    iconsHeights_[size - minSize_] = icons_[size - minSize_].height();
    iconsWidths_[size - minSize_] = icons_[size - minSize_].width();
    // free image again
    icons_[size - minSize_] = QPixmap();
  }
  // create mipmaps only for sizes etc
  return;
}



int IconBasedDockItem::getIconWidth (int size) const {
  if (size < minSize_) {
    size = minSize_;
  } else if (size > maxSize_) {
    size = maxSize_;
  }
  return iconsWidths_[size - minSize_];
}

int IconBasedDockItem::getIconHeight (int size) const {
  if (size < minSize_) {
    size = minSize_;
  } else if (size > maxSize_) {
    size = maxSize_;
  }
  return iconsHeights_[size - minSize_];
}



}  // namespace ksmoothdock
