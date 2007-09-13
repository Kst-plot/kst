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

#include "plotitem.h"

namespace Kst {

PlotRenderItem::PlotRenderItem(const QString &name, PlotItem *parentItem)
  : QObject(parentItem), QGraphicsRectItem(parentItem) {
  _name = name;

  connect(parentItem, SIGNAL(geometryChanged()), this, SLOT(updateGeometry()));
  updateGeometry(); //the initial rect
}


PlotRenderItem::~PlotRenderItem() {
}


PlotItem *PlotRenderItem::plotItem() const {
  return qgraphicsitem_cast<PlotItem*>(parentItem());
}


void PlotRenderItem::updateGeometry() {
  QRectF rect = plotItem()->rect().normalized();
  QPointF margin(plotItem()->marginWidth(), plotItem()->marginHeight());
  QPointF topLeft(rect.topLeft() + margin);
  QPointF bottomRight(rect.bottomRight() - margin);
  setRect(QRectF(topLeft, bottomRight));
}


void PlotRenderItem::setType(RenderType type) {
  _type = type;
}


RenderType PlotRenderItem::type() {
  return _type;
}


QRectF PlotRenderItem::plotRect() const {
  QRectF plotRect = rect();
  plotRect = plotRect.normalized();
  plotRect.moveTopLeft(QPoint(0,0));
  return plotRect;
}


void PlotRenderItem::setRelationList(const KstRelationList &relationList) {
  _relationList = relationList;
}


KstRelationList PlotRenderItem::relationList() const {
  return _relationList;
}


void PlotRenderItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  Q_UNUSED(option);
  Q_UNUSED(widget);
  painter->fillRect(rect(), Qt::white);
  paint(painter);
}


QString PlotRenderItem::leftLabel() const {
  foreach (KstRelationPtr relation, relationList()) {
    if (!relation->yLabel().isEmpty())
      return relation->yLabel();
  }
  return QString();
}


QString PlotRenderItem::bottomLabel() const {
  foreach (KstRelationPtr relation, relationList()) {
    if (!relation->xLabel().isEmpty())
      return relation->xLabel();
  }
  return QString();
}


QString PlotRenderItem::rightLabel() const {
  //FIXME much less than ideal
  QString left = leftLabel();
  foreach (KstRelationPtr relation, relationList()) {
    if (!relation->yLabel().isEmpty() && relation->yLabel() != left)
      return relation->yLabel();
  }
  return QString();
}


QString PlotRenderItem::topLabel() const {
  //FIXME much less than ideal
  QString bottom = bottomLabel();
  foreach (KstRelationPtr relation, relationList()) {
    if (!relation->xLabel().isEmpty() && relation->xLabel() != bottom)
      return relation->xLabel();
    if (!relation->topLabel().isEmpty())
      return relation->topLabel();
  }
  return QString();
}


QRectF PlotRenderItem::mapToProjection(const QRectF &rect) {
  return QRectF(mapToProjection(rect.topLeft()), mapToProjection(rect.bottomRight()));
}


QRectF PlotRenderItem::mapFromProjection(const QRectF &rect) {
  return QRectF(mapFromProjection(rect.topLeft()), mapFromProjection(rect.bottomRight()));
}

}

// vim: ts=2 sw=2 et
