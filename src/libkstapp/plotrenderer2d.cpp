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


void PlotRenderer2D::setProjectedRange(const QRectF &range) {
  _projectedRange = range;

  _range =  mapFromProjection(range);

  refreshRange();
}


void PlotRenderer2D::setRange(const QRectF &range) {
  _range = range;

  _projectedRange = mapToProjection(range);

  refreshRange();
}


void PlotRenderer2D::setRelationList(const KstRelationList &relationList) {
  _relationList = relationList;
}


KstRelationList PlotRenderer2D::relationList() const {
  return _relationList;
}


QRectF PlotRenderer2D::projectedRange() {
  return _projectedRange;
}


QRectF PlotRenderer2D::range() {
  return _range;
}


QRectF PlotRenderer2D::mapToProjection(const QRectF &rect) {
    return QRectF(mapToProjection(rect.topLeft()), mapToProjection(rect.bottomRight()));
}


QRectF PlotRenderer2D::mapFromProjection(const QRectF &rect) {
    return QRectF(mapFromProjection(rect.topLeft()), mapFromProjection(rect.bottomRight()));
}


void PlotRenderer2D::refreshRange() {
}

}

// vim: ts=2 sw=2 et
