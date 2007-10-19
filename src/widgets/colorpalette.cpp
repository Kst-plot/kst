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

#include "colorpalette.h"

#include <QPixmap>
#include <QPainter>

#include "palette.h"

namespace Kst {

ColorPalette::ColorPalette(QWidget *parent)
    : QWidget(parent) {
  setupUi(this);

  connect(_palette, SIGNAL(highlighted(const QString&)), this, SLOT(updatePalette(const QString&)));
  connect(_palette, SIGNAL(activated(const QString&)), this, SLOT(updatePalette(const QString&)));

  refresh();
  updatePalette();
}


ColorPalette::~ColorPalette() {}


void ColorPalette::updatePalette(const QString &palette) {
  QString paletteName = palette;
  if (!paletteName.isEmpty()) {
    paletteName = selectedPalette();
  }

 Palette*  newPalette = new Palette(palette);
  int numberOfColors = newPalette->colorCount();
  int height = _palette->height();
  int width = 7 * height;
  int size = 1;
  int step = 1;

  if (numberOfColors > 0) {
    size = width / numberOfColors;
    if (size == 0) {
      size = 1;
      step = numberOfColors / width;
    }
  }

  int pos = 0;
  QPixmap pix(width, height);
  QPainter p(&pix);

  p.fillRect(p.window(), QColor("white"));
  for (int i=0; i<numberOfColors; i+=step) {
    p.fillRect(pos*size, 0, size, _palette->height(), QBrush(newPalette->color(i)));
    ++pos;
  }
  _paletteDisplay->setPixmap(pix);

  delete newPalette;
}



QString ColorPalette::selectedPalette() {
   return _palette->currentText();
}


void ColorPalette::refresh( const QString & palette ) {
  _palette->clear();

  QStringList paletteList = Palette::getPaletteList();
  paletteList.sort();
  _palette->addItems(paletteList);

  if (!palette.isEmpty()) {
    int i;
    for (i = 0; i < _palette->count(); ++i) {
      if (_palette->itemText(i) == palette) {
        break;
      }
    }
    if (i == _palette->count()) {
      i = 0;
    }
    _palette->setCurrentIndex(i);
  }
}



int ColorPalette::currentPaletteIndex() {
   return _palette->currentIndex();
}

}

// vim: ts=2 sw=2 et
