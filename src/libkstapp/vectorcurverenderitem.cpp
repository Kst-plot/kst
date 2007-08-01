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

#include "vectorcurverenderitem.h"

#include <QDebug>

namespace Kst {

VectorCurveRenderItem::VectorCurveRenderItem(const QString &name)
  : PlotRenderItem(name) {
  setType(Cartesian);
}

VectorCurveRenderItem::~VectorCurveRenderItem() {
}


void VectorCurveRenderItem::paint(QPainter *painter) {
    Q_UNUSED(painter);
}


QList<QPainterPath> VectorCurveRenderItem::projectedPaths() {
//   QList<QPainterPath> paths;
// 
//   foreach (KstRelationPtr relation, relationList()) {
//     QPainterPath path;
//     KstCurveRenderContext context;
//     context.path = &path;
//     context.rect = range().toRect();
//     relation->paint(context);
//     paths << path;
//   }
//   return paths;
}


QPointF VectorCurveRenderItem::mapToProjection(const QPointF &point) {
  return point;
}


QPointF VectorCurveRenderItem::mapFromProjection(const QPointF &point) {
  return point;
}

}

// vim: ts=2 sw=2 et
