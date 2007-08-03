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

//FIXME how many?
static int DESIRED_NUMBER_OF_POINTS_FOR_INITIAL_VIEW = 100;

namespace Kst {

VectorCurveRenderItem::VectorCurveRenderItem(const QString &name)
  : PlotRenderItem(name) {
  setType(Cartesian);
}

VectorCurveRenderItem::~VectorCurveRenderItem() {
}


void VectorCurveRenderItem::paint(QPainter *painter) {
  foreach (KstRelationPtr relation, relationList()) {
    //FIXME static_cast to kstvcurve and take advantage of extra api

    KstCurveRenderContext context;
    context.painter = painter;
    context.window = plotRect().toRect(); //no idea if this should be floating point

    //FIXME rename these methods in kstvcurve
    QRectF vectorRect(relation->minX(),
                      relation->minY(),
                      relation->maxX(),
                      relation->maxY());

    QTransform t;
    qreal scaleFactor = 1.0 / (relation->sampleCount() / DESIRED_NUMBER_OF_POINTS_FOR_INITIAL_VIEW);
    t.scale(scaleFactor, scaleFactor);

    QRectF zoomRect = t.mapRect(vectorRect);
    zoomRect.moveTopLeft(vectorRect.topLeft());

//     qDebug() << "============================================================>"
//              << "vectorRect" << vectorRect
//              << "zoombox" << zoomRect
//              << "plotRect" << plotRect() << endl;

    //FIXME Completely refactor KstCurveRenderContext now that we know what these are

    //Set what amounts to the zoombox...
    context.XMin = zoomRect.left();
    context.XMax = zoomRect.right();
    context.YMin = zoomRect.top();
    context.YMax = zoomRect.bottom();

    //These are the bounding box in regular QGV coord
    context.Lx = plotRect().left();
    context.Hx = plotRect().right();
    context.Ly = plotRect().top();
    context.Hy = plotRect().bottom();

    //To convert between the last two...
    double m_X =  double(plotRect().width()-1)/(context.XMax - context.XMin);
    double m_Y = -double(plotRect().height()-1)/(context.YMax - context.YMin);
    double b_X = context.Lx - m_X * context.XMin;
    double b_Y = context.Ly - m_Y * context.YMax;

    context.m_X = m_X;
    context.m_Y = m_Y;
    context.b_X = b_X;
    context.b_Y = b_Y;

    painter->save();
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
