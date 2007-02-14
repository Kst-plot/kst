/***************************************************************************
                               kstviewbezier.cpp
                             -------------------
    begin                : Jun 14, 2005
    copyright            : (C) 2005 The University of Toronto
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

#include "kstviewbezier.h"
#include <qpainter.h>

KstViewBezier::KstViewBezier()
: KstViewObject("Bezier"), _width(1) {
  setTransparent(true);
  _capStyle = Qt::FlatCap; 
  _penStyle = Qt::SolidLine; 
  _points.resize(4);
  _points[0] = _points[1] = _points[2] = _points[3] = QPoint(0, 0);
}


KstViewBezier::KstViewBezier(const QDomElement& e)
: KstViewObject(e) {
  _width = 1;
  _capStyle = Qt::FlatCap; 
  _penStyle = Qt::SolidLine; 
  _points.resize(4);
  _points[0] = _points[1] = _points[2] = _points[3] = QPoint(0, 0);
  setTransparent(true);
}


KstViewBezier::~KstViewBezier() {
}


void KstViewBezier::paint(KstPainter& p, const QRegion& bounds) {
  KstViewObject::paint(p, bounds);
  QPen pen(_foregroundColor, _width);
  pen.setCapStyle(_capStyle);
  pen.setStyle(_penStyle);
  p.setPen(pen);
  p.drawCubicBezier(_points);
}


void KstViewBezier::save(QTextStream& ts, const QString& indent) {
  KstViewObject::save(ts, indent);
}


void KstViewBezier::setPointA(const QPoint& pt) {
  if (_points[0] != pt) {
    _points[0] = pt;
    setDirty();
  }
}


QPoint KstViewBezier::pointA() const {
  return _points[0];
}


void KstViewBezier::setPointB(const QPoint& pt) {
  if (_points[1] != pt) {
    _points[1] = pt;
    setDirty();
  }
}


QPoint KstViewBezier::pointB() const {
  return _points[1];
}


void KstViewBezier::setPointC(const QPoint& pt) {
  if (_points[2] != pt) {
    _points[2] = pt;
    setDirty();
  }
}


QPoint KstViewBezier::pointC() const {
  return _points[2];
}


void KstViewBezier::setPointD(const QPoint& pt) {
  if (_points[3] != pt) {
    _points[3] = pt;
    setDirty();
  }
}


QPoint KstViewBezier::pointD() const {
  return _points[3];
}


void KstViewBezier::setWidth(int width) {
  if (_width != width) {
    _width = width;
    setDirty();
  }
}


int KstViewBezier::width() const {
  return _width;
}


void KstViewBezier::setCapStyle(Qt::PenCapStyle style) {
  if (_capStyle != style) {
    _capStyle = style;
    setDirty();
  }
}


Qt::PenCapStyle KstViewBezier::capStyle() const {
  return _capStyle;
}


void KstViewBezier::setPenStyle(Qt::PenStyle style) {
  if (_penStyle != style) {
    _penStyle = style;
    setDirty();
  }
}


Qt::PenStyle KstViewBezier::penStyle() const {
  return _penStyle;
}


void KstViewBezier::resize(const QSize& sz) {
  const QSize oldSize(size());
  const QPoint pos(position());
  const double dxF = double(sz.width() - oldSize.width()) / oldSize.width();
  const double dyF = double(sz.height() - oldSize.height()) / oldSize.height();
  _points[0].setX(int(_points[0].x() * (1 + dxF) - pos.x() * dxF));
  _points[0].setY(int(_points[0].y() * (1 + dyF) - pos.y() * dyF));
  _points[1].setX(int(_points[1].x() * (1 + dxF) - pos.x() * dxF));
  _points[1].setY(int(_points[1].y() * (1 + dyF) - pos.y() * dyF));
  _points[2].setX(int(_points[2].x() * (1 + dxF) - pos.x() * dxF));
  _points[2].setY(int(_points[2].y() * (1 + dyF) - pos.y() * dyF));
  _points[3].setX(int(_points[3].x() * (1 + dxF) - pos.x() * dxF));
  _points[3].setY(int(_points[3].y() * (1 + dyF) - pos.y() * dyF));
  KstViewObject::resize(sz);
}


void KstViewBezier::move(const QPoint& pt) {
  const QPoint offset(pt - position());
  _points.translate(offset.x(), offset.y());
  KstViewObject::move(pt);
}


#include "kstviewbezier.moc"
// vim: ts=2 sw=2 et
