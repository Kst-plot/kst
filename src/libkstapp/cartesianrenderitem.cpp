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

#include "cartesianrenderitem.h"

#include <QDebug>
#include <QGraphicsSceneMouseEvent>

#include "math_kst.h"

#include "plotitem.h"

namespace Kst {

CartesianRenderItem::CartesianRenderItem(PlotItem *parentItem)
  : PlotRenderItem(parentItem) {
  setType(Cartesian);
  setTypeName(tr("Cartesian Plot"));
}


CartesianRenderItem::~CartesianRenderItem() {
}


void CartesianRenderItem::paintRelations(QPainter *painter) {

  QRectF normalRect = rect();
  normalRect = normalRect.normalized();

  painter->save();
  painter->translate(normalRect.x(), normalRect.y());

  foreach (RelationPtr relation, relationList()) {

    CurveRenderContext context;
    context.painter = painter;
    context.window = QRect(); //no idea if this should be floating point
    context.penWidth = painter->pen().width(); //floating point??
    context.xLog = plotItem()->xAxis()->axisLog();
    context.yLog = plotItem()->yAxis()->axisLog();
    context.xLogBase = 10.0;
    context.yLogBase = 10.0;
    context.foregroundColor = painter->pen().color();
    context.backgroundColor = painter->brush().color();

    //Set the projection box...
    context.XMin = projectionRect().left();
    context.XMax = projectionRect().right();
    context.YMin = projectionRect().top();
    context.YMax = projectionRect().bottom();

    //Set the log box...
    context.x_max = plotItem()->xAxis()->axisLog() ? logXHi(context.XMax, context.xLogBase) : context.XMax;
    context.y_max = plotItem()->yAxis()->axisLog() ? logXHi(context.YMax, context.yLogBase) : context.YMax;
    context.x_min = plotItem()->xAxis()->axisLog() ? logXLo(context.XMin, context.xLogBase) : context.XMin;
    context.y_min = plotItem()->yAxis()->axisLog() ? logXLo(context.YMin, context.yLogBase) : context.YMin;

    //These are the bounding box in regular QGV coord
    context.Lx = plotRect().left();
    context.Hx = plotRect().right();
    context.Ly = plotRect().top();
    context.Hy = plotRect().bottom();

    //To convert between the last two...
    double m_X = double(plotRect().width())/(context.x_max - context.x_min);
    double m_Y = -double(plotRect().height())/(context.y_max - context.y_min);
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


void CartesianRenderItem::saveInPlot(QXmlStreamWriter &xml) {
  xml.writeStartElement("cartesianrender");
  PlotRenderItem::saveInPlot(xml);
  xml.writeEndElement();
}


bool CartesianRenderItem::configureFromXml(QXmlStreamReader &xml, ObjectStore *store) {
  bool validTag = false;
  if (xml.isStartElement() && xml.name().toString() == "cartesianrender") {
    validTag = PlotRenderItem::configureFromXml(xml, store);
    if (validTag && xml.isEndElement() && xml.name().toString() == "cartesianrender") {
      validTag = true;
    }
  }
  return validTag;
}

}

// vim: ts=2 sw=2 et
