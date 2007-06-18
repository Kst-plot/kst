/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 Theodore Kisner <tsk@humanityforward.org>        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "plotrenderer2d.h"

namespace Kst {

PlotRenderer2D::PlotRenderer2D(const QString &name) {
  _name = name;
}


PlotRenderer2D::~PlotRenderer2D() {
}


void PlotRenderer2D::setType(RenderType2D type) {
  _type = type;
}


RenderType2D PlotRenderer2D::type() {
  return _type;
}


void PlotRenderer2D::setRangeXY(const QRectF& range) {
  _xyRange = range;
  QPointF topLeft;
  QPointF bottomRight;
  projectPointInv(range.topLeft(), &topLeft);
  projectPointInv(range.bottomRight(), &bottomRight);
  _uvRange.setTopLeft(topLeft);
  _uvRange.setBottomRight(bottomRight);
  refreshRange();
}


void PlotRenderer2D::setRangeUV(const QRectF& range) {
  _uvRange = range;
  QPointF topLeft;
  QPointF bottomRight;
  projectPoint(range.topLeft(), &topLeft);
  projectPoint(range.bottomRight(), &bottomRight);
  _xyRange.setTopLeft(topLeft);
  _xyRange.setBottomRight(bottomRight);
  refreshRange();
}


void PlotRenderer2D::rangeXY(QRectF *range) {
  (*range) = _xyRange;
}


void PlotRenderer2D::rangeUV(QRectF *range) {
  (*range) = _uvRange;
}


void PlotRenderer2D::refreshRange() {
}


void PlotRenderer2D::projectPath(QPainterPath *path) {
}


void PlotRenderer2D::projectPoint(const QPointF& pold, QPointF *pnew) {
  (*pnew) = pold;
}


void PlotRenderer2D::projectPointInv(const QPointF& pold, QPointF *pnew) {
  (*pnew) = pold;
}

}

// vim: ts=2 sw=2 et
