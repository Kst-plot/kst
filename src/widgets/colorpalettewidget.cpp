/***************************************************************************
                   colorpalettewidget.cpp
                             -------------------
    begin                : 02/27/07
    copyright            : (C) 2007 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "colorpalettewidget.h"

#include <QPixmap>
#include <QPainter>

#include <kst_export.h>

ColorPaletteWidget::ColorPaletteWidget(QWidget *parent)
    : QWidget(parent) {
  setupUi(this);

  connect(_palette, SIGNAL(highlighted(const QString&)), this, SLOT(updatePalette(const QString&)));
  connect(_palette, SIGNAL(activated(const QString&)), this, SLOT(updatePalette(const QString&)));
}


ColorPaletteWidget::~ColorPaletteWidget() {}


void ColorPaletteWidget::init() {
  refresh();
}


void ColorPaletteWidget::updatePalette( const QString &palette) {
  KPalette* newPal = 0L;
  QColor color;
  int nrColors = 0;
  int height = _palette->height();
  int width = 7 * height;
  QPixmap pix(width, height);
  QPainter p(&pix);
  int size = 1;
  int step = 1;
  int pos = 0;
  int i;

  if (!palette.isEmpty()) {
    newPal = new KPalette(palette);
    nrColors = newPal->nrColors();
  }

  if (nrColors > 0) {
    size = width / nrColors;
    if (size == 0) {
      size = 1;
      step = nrColors / width;
    }
  }

  p.fillRect(p.window(), QColor("white"));
  for (i=0; i<nrColors; i+=step) {
    color = newPal->color(i);
    p.fillRect(pos*size, 0, size, height, QBrush(color));
    ++pos;
  }
  _paletteDisplay->setPixmap(pix);

  delete newPal;
}



QString ColorPaletteWidget::selectedPalette() {
  return _palette->currentText();
}


void ColorPaletteWidget::refresh() {
  _palette->clear();
  QStringList palList = KPalette::getPaletteList();
  palList.sort();
  _palette->addItems(palList);

  if (palList.contains("Kst Spectrum 1021")) {
    _palette->setItemText(_palette->currentIndex(), "Kst Spectrum 1021");
  } else if (palList.contains("Kst Grayscale 256")) {
    _palette->setItemText(_palette->currentIndex(), "Kst Grayscale 256");
  }
}


void ColorPaletteWidget::refresh( const QString & palette ) {
  QStringList palList = KPalette::getPaletteList();
  int i;

  _palette->clear();
  palList.sort();
  _palette->addItems(palList);

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



int ColorPaletteWidget::currentPaletteIndex() {
  return _palette->currentIndex();
}

#include "colorpalettewidget.moc"

// vim: ts=2 sw=2 et
