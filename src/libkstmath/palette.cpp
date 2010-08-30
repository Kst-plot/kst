/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// application specific includes
#include "palette.h"
#include "settings.h"
#include <QVector>
#include <qapplication.h>
#include <math_kst.h>

namespace Kst {


// Default palette.
static const char *const KstColors[] = { "red",
                                      "blue",
                                      "green",
                                      "black",
                                      "magenta",
                                      "steelblue",
                                      "#501010",
                                      "#105010"
                                      };
static const int KstColorsCount = sizeof(KstColors) / sizeof(char*);
static const QString KstColorsName = "Kst Colors";

static const int KstGrayscaleCount = 255;
static const QString KstGrayscaleName = "Kst Grayscale";

QStringList Palette::getPaletteList() { 
  QStringList paletteList;

  paletteList.append(KstGrayscaleName);
  paletteList.append(KstColorsName);

  //TODO: support loading palettes from disk.

  return paletteList;
}

Palette::Palette(): _colors(0), _count(0) {
  changePaletteName(KstColorsName);
}


Palette::Palette(const QString &paletteName): _colors(0), _count(0) {
  changePaletteName(paletteName);
}

Palette::~Palette() {
  delete[] _colors;
  delete[] _rgb;
  _colors = 0;
  _count = 0;
}

void Palette::changePaletteName(const QString &paletteName) {

  if (_count==0) {
    _colors = new QColor[2048];
    _rgb = new QRgb[2048];
  }

  if (paletteName.isEmpty()) {
    _paletteName = KstColorsName;
  } else {
    _paletteName = paletteName;
  }

  if (_paletteName == KstColorsName) {
    for (int i = 0; i < KstColorsCount; i++) {
      _colors[i] = QColor(KstColors[i]);
      _rgb[i] = _colors[i].rgb();
    }
    _count = KstColorsCount;
  } else {
    for (int i = 0; i < KstGrayscaleCount; i++) {
      _colors[i] = QColor(i, i, i);
      _rgb[i] = _colors[i].rgb();
    }
    _count = KstGrayscaleCount;
  }
}

}
// vim: ts=2 sw=2 et
