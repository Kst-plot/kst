/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
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
#include "builtinpalettes.h"

namespace Kst {


QStringList Palette::getPaletteList() { 
  QStringList paletteList;

  paletteList.append(KstGrayscaleName);
  paletteList.append(RedTempName);
  paletteList.append(SpectrumName);
  paletteList.append(EosAName);
  paletteList.append(EosBName);
  paletteList.append(KstColorsName);

  //TODO: support loading palettes from disk.

  return paletteList;
}

Palette::Palette(): _colors(0), _count(0) {
  changePaletteName(DefaultPalette);
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
  } else if (_paletteName == RedTempName) {
    for (int i = 0; i < RedTempCount; i++) {
      _colors[i] = QColor(RedTemp[i][0], RedTemp[i][1], RedTemp[i][2]);
      _rgb[i] = _colors[i].rgb();
    }
    _count = RedTempCount;
  } else if (_paletteName == SpectrumName) {
    for (int i = 0; i < SpectrumCount; i++) {
      _colors[i] = QColor(Spectrum[i][0], Spectrum[i][1], Spectrum[i][2]);
      _rgb[i] = _colors[i].rgb();
    }
    _count = SpectrumCount;
  } else if (_paletteName == EosAName) {
    for (int i = 0; i < EosACount; i++) {
      _colors[i] = QColor(EosA[i][0], EosA[i][1], EosA[i][2]);
      _rgb[i] = _colors[i].rgb();
    }
    _count = EosACount;
  } else if (_paletteName == EosBName) {
    for (int i = 0; i < EosBCount; i++) {
      _colors[i] = QColor(EosB[i][0], EosB[i][1], EosB[i][2]);
      _rgb[i] = _colors[i].rgb();
    }
    _count = EosBCount;  
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
