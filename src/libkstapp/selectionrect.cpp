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

#include "selectionrect.h"

#include <QDebug>

SelectionRect::SelectionRect() {
  reset();
}


SelectionRect::~SelectionRect() {
}


bool SelectionRect::isValid() const {
  return _validFrom && _validTo && _from != _to;
}


void SelectionRect::setFrom(const QPointF& point) {
  _from = point;
  _validFrom = true;
}


void SelectionRect::setTo(const QPointF& point) {
  _to = point;
  _validTo = true;
}


void SelectionRect::reset() {
  _from = _to = QPointF();
  _validFrom = _validTo = false;
}


QRectF SelectionRect::rect() const {
  if (!isValid())
    return QRectF();

  return QRectF(_from, _to).normalized();
}


// vim: ts=2 sw=2 et
