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

#include "colorpalette.h"

#include "ui_colorpalette.h"

#include <QPixmap>
#include <QPainter>

#include "palette.h"

namespace Kst {

ColorPalette::ColorPalette(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ColorPalette)
{
  ui->setupUi(this);

  connect(ui->_palette, SIGNAL(highlighted(QString)), this, SLOT(updatePalette(QString)));
  connect(ui->_palette, SIGNAL(currentIndexChanged(QString)), this, SLOT(updatePalette(QString)));
  connect(ui->_palette, SIGNAL(currentIndexChanged(int)), this, SIGNAL(selectionChanged()));

  refresh(DefaultPalette);
  updatePalette(DefaultPalette);
}


ColorPalette::~ColorPalette() {}


void ColorPalette::updatePalette(const QString &palette) {
  QString paletteName = palette;
  if (!paletteName.isEmpty()) {
    paletteName = selectedPalette();
  }

 Palette*  newPalette = new Palette(palette);
  int numberOfColors = newPalette->colorCount();
  int height = ui->_palette->height() ? ui->_palette->height():1;
  int width = 7 * height;

  QPixmap pix(width, height);
  QPainter p(&pix);

  p.fillRect(p.window(), QColor("white"));
  for (int i=0; i<width; i++) {
    int j = i*numberOfColors/width;
    p.fillRect(i, 0, 1, height, QBrush(newPalette->color(j)));
  }

  ui->_paletteDisplay->setPixmap(pix);

  delete newPalette;
}


QString ColorPalette::selectedPalette() {
   return ui->_palette->currentText();
}


void ColorPalette::setPalette(const QString palette) {
  ui-> _palette->setCurrentIndex(ui->_palette->findText(palette));
  updatePalette(palette);
}


bool ColorPalette::selectedPaletteDirty() const {
  return ui->_palette->currentIndex() != -1;
}


void ColorPalette::clearSelection() {
  ui->_palette->setCurrentIndex(-1);

  QPixmap pix(7 *ui->_palette->height(), ui->_palette->height());
  QPainter p(&pix);
  p.fillRect(p.window(), QColor("white"));
  ui->_paletteDisplay->setPixmap(pix);
}


void ColorPalette::refresh( const QString & palette ) {
  ui->_palette->clear();

  QStringList paletteList = Palette::getPaletteList();
  paletteList.sort();
  ui->_palette->addItems(paletteList);

  if (!palette.isEmpty()) {
    int i;
    for (i = 0; i < ui->_palette->count(); ++i) {
      if (ui->_palette->itemText(i) == palette) {
        break;
      }
    }
    if (i == ui->_palette->count()) {
      i = ui->_palette->findText(DefaultPalette);
      if (i<0) {
        i=0;
      }
    }
    ui->_palette->setCurrentIndex(i);
  }
}



int ColorPalette::currentPaletteIndex() {
   return ui->_palette->currentIndex();
}

}

// vim: ts=2 sw=2 et
