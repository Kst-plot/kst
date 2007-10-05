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
#include <QGraphicsSceneMouseEvent>

#include "math_kst.h"

#include "plotitem.h"

namespace Kst {

VectorCurveRenderItem::VectorCurveRenderItem(PlotItem *parentItem)
  : PlotRenderItem(parentItem) {
  setType(Cartesian);
  setName(tr("Vector Curve"));
}


VectorCurveRenderItem::~VectorCurveRenderItem() {
}


void VectorCurveRenderItem::paintRelations(QPainter *painter) {

  QRectF normalRect = rect();
  normalRect = normalRect.normalized();

  painter->save();
  painter->translate(normalRect.x(), normalRect.y());

  foreach (KstRelationPtr relation, relationList()) {
    //FIXME static_cast to kstvcurve and take advantage of extra api

    KstCurveRenderContext context;
    context.painter = painter;
    context.window = QRect(); //no idea if this should be floating point
    context.penWidth = 1; //FIXME hardcode
    context.xLog = isXAxisLog();
    context.yLog = isYAxisLog();
    context.xLogBase = xLogBase();
    context.yLogBase = yLogBase();

    //FIXME rename these methods in kstvcurve
    //FIXME Completely refactor KstCurveRenderContext now that we know what these are

    //Set the projection box...
    context.XMin = projectionRect().left();
    context.XMax = projectionRect().right();
    context.YMin = projectionRect().top();
    context.YMax = projectionRect().bottom();

    //Set the log box...
    context.x_max = isXAxisLog() ? logXHi(context.XMax, context.xLogBase) : context.XMax;
    context.y_max = isYAxisLog() ? logXHi(context.YMax, context.yLogBase) : context.YMax;
    context.x_min = isXAxisLog() ? logXLo(context.XMin, context.xLogBase) : context.XMin;
    context.y_min = isYAxisLog() ? logXLo(context.YMin, context.yLogBase) : context.YMin;

    //These are the bounding box in regular QGV coord
    context.Lx = plotRect().left();
    context.Hx = plotRect().right();
    context.Ly = plotRect().top();
    context.Hy = plotRect().bottom();

    //To convert between the last two...
    double m_X = double(plotRect().width()-1)/(context.x_max - context.x_min);
    double m_Y = -double(plotRect().height()-1)/(context.y_max - context.y_min);
    double b_X = context.Lx - m_X * context.x_min;
    double b_Y = context.Ly - m_Y * context.y_max;

    context.m_X = m_X;
    context.m_Y = m_Y;
    context.b_X = b_X;
    context.b_Y = b_Y;

    relation->paint(context);
  }

  painter->restore();
}

}

// vim: ts=2 sw=2 et
