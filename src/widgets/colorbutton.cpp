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

#include "colorbutton.h"

#include <QPainter>
#include <QColorDialog>

namespace Kst {

ColorButton::ColorButton(QWidget *parent)
  : QToolButton(parent), _color(Qt::white) {
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  connect (this, SIGNAL(clicked()), this, SLOT(chooseColor()));
}


ColorButton::ColorButton(const QColor &color, QWidget *parent)
  : QToolButton(parent), _color(color) {
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  connect (this, SIGNAL(clicked()), this, SLOT(chooseColor()));
}


ColorButton::~ColorButton() {
}


QColor ColorButton::color() const {
  return _color;
}


void ColorButton::setColor(const QColor &color) {
  _color = color;
  update();
  emit changed(color);
}


void ColorButton::paintEvent(QPaintEvent *event)
{
  QToolButton::paintEvent(event);
  if (!isEnabled())
      return;

  QPainter painter(this);
  QBrush brush(_color);
  qDrawShadePanel(&painter, rect().x() + 2, rect().y() + 2, rect().width() - 4, rect().height() - 4,
                  palette(), /*sunken*/ isDown(), /*lineWidth*/ 1, /*fill*/ &brush);
}


void ColorButton::chooseColor() {

  bool ok;
  QRgb rgba = QColorDialog::getRgba(_color.rgba(), &ok, parentWidget());
  if (ok) {
    QColor color;
    color.setRgba(rgba);
    setColor(color);
  }
}

}

#include "colorbutton.moc"

// vim: ts=2 sw=2 et
