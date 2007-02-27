/***************************************************************************
                              kstbackbuffer.cpp
                             -------------------
    begin                : Apr 18, 2004
    copyright            : (C) 2004 The University of Toronto
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

#include "kstbackbuffer.h"

#include "curveappearancewidget.ui.h"
//Added by qt3to4:
#include <QPixmap>

KstBackBuffer::KstBackBuffer()
: _buffer(0, 0) {
}


KstBackBuffer::~KstBackBuffer() {
}


QPixmap& KstBackBuffer::buffer() {
  return _buffer;
}


void KstBackBuffer::paintInto(QPainter& p, const QRect& geom) {
  p.drawPixmap(geom.left(), geom.top(), _buffer, 0, 0, geom.width(), geom.height());
}


void KstBackBuffer::paintInto(QPainter& p, const QRect& geom, const QPoint& from) {
  p.drawPixmap(geom.left(), geom.top(), _buffer, from.x(), from.y(), geom.width(), geom.height());
}


// vim: ts=2 sw=2 et
