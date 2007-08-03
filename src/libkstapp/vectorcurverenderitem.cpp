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
  foreach (KstRelationPtr relation, relationList()) {
    KstCurveRenderContext context;
    context.painter = painter;
    context.window = range().toRect(); //no idea if this should be floating point

    //Everything that comes next is magic...  I took most from kst2dplot, hints
    //from barth and trial and error...

    //These are the region of the plot in vector coord
    context.XMin = 0.0;
    context.XMax = 0.1;
    context.YMin = 0.0;
    context.YMax = 0.1;

    //These are the bounding box in regular QGV coord
    context.Lx = range().left();
    context.Hx = range().right();
    context.Ly = range().top();
    context.Hy = range().bottom();

    //To convert between the last two...
    double m_X =  double(range().width()-1)/(context.XMax - context.XMin);
    double m_Y = -double(range().height()-1)/(context.YMax - context.YMin);
    double b_X = context.Lx - m_X * context.XMin;
    double b_Y = context.Ly - m_Y * context.YMax;

    context.m_X = m_X;
    context.m_Y = m_Y;
    context.b_X = b_X;
    context.b_Y = b_Y;

    painter->save();

//     qDebug() << "origin of plot:" << QPoint(context.Lx, context.Hy) << endl;
//     qDebug() << "origin of relation:" << QPoint(relation->minX(), relation->minY()) << endl;

    relation->paint(context);
    painter->restore();
  }
}


QPointF VectorCurveRenderItem::mapToProjection(const QPointF &point) {
  return point;
}


QPointF VectorCurveRenderItem::mapFromProjection(const QPointF &point) {
  return point;
}

}

// vim: ts=2 sw=2 et
