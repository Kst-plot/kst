/*******************************************************************************
 *                                                                             *
 *   copyright : (C) 2007 The University of Toronto                            *
 *   copyright : (C) 2010 C. Barth Netterfield <netterfield@astro.utoronto.ca> *
 *                                                                             *
 *   This program is free software; you can redistribute it and/or modify      *
 *   it under the terms of the GNU General Public License as published by      *
 *   the Free Software Foundation; either version 2 of the License, or         *
 *   (at your option) any later version.                                       *
 *                                                                             *
 *******************************************************************************/

#ifndef _PALETTE_H
#define _PALETTE_H

#include <QColor>
#include <QHash>
#include "kstmath_export.h"

namespace Kst {

typedef QHash<int, QColor> PaletteData;

const unsigned int maxColorTableSize = 2048;

class KSTMATH_EXPORT Palette 
{
  public:
    /**
     * @returns string list of avalible palettes
     */
    static QStringList getPaletteList();

    Palette();
    Palette(const QString &paletteName);

    virtual ~Palette();

    void changePaletteName(const QString &paletteName);
    QString paletteName() const {return _paletteName;}
    int colorCount() const {return _count;}

    /**
     * Returns the color corresponding to colorId.
     * It truncates to max or min _color if colorId is out
     * of bounds, so bounds checking is not necessary in the
     * calling function.
     * @returns the QColor
     */
    QColor color(const int colorId) const{
      if (colorId<0) {
        return _colors[0];
      } else if (colorId>=_count) {
        return _colors[_count-1];
      } else {
        return _colors[colorId];
      }
    }
    /**
     * Returns the rgb value for the color corresponding to colorId.
     * It truncates to max or min _rgb if colorId is out
     * of bounds, so bounds checking is not necessary in the
     * calling function.
     * @returns the QColor.rgb().
     */
    QRgb rgb(const int colorId) const{
      if (colorId<0) {
        return _rgb[0];
      } else if (colorId>=_count) {
        return _rgb[_count-1];
      } else {
        return _rgb[colorId];
      }
    }

private:
    QColor *_colors;
    QRgb *_rgb;
    QString _paletteName;
    int _count;
};

}
#endif

// vim: ts=2 sw=2 et
