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

#include "plotrenderitem.h"

namespace Kst {

PlotRenderItem::PlotRenderItem(const QString &name) {
  _name = name;
}


PlotRenderItem::~PlotRenderItem() {
}


void PlotRenderItem::setType(RenderType type) {
  _type = type;
}


RenderType PlotRenderItem::type() {
  return _type;
}


void PlotRenderItem::setPlotRect(const QRectF &plotRect) {
  _plotRect = plotRect;
}


QRectF PlotRenderItem::plotRect() {
  return _plotRect;
}


void PlotRenderItem::setRelationList(const KstRelationList &relationList) {
  _relationList = relationList;
}


KstRelationList PlotRenderItem::relationList() const {
  return _relationList;
}


QRectF PlotRenderItem::mapToProjection(const QRectF &rect) {
    return QRectF(mapToProjection(rect.topLeft()), mapToProjection(rect.bottomRight()));
}


QRectF PlotRenderItem::mapFromProjection(const QRectF &rect) {
    return QRectF(mapFromProjection(rect.topLeft()), mapFromProjection(rect.bottomRight()));
}

}

// vim: ts=2 sw=2 et
