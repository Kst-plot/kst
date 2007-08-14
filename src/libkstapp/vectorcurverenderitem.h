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

#ifndef VECTORCURVERENDERITEM_H
#define VECTORCURVERENDERITEM_H

#include "plotrenderitem.h"

namespace Kst {

class VectorCurveRenderItem : public PlotRenderItem
{
  Q_OBJECT
  public:
    VectorCurveRenderItem(const QString &name, PlotItem *parentItem);
    virtual ~VectorCurveRenderItem();

    virtual void paint(QPainter *painter);

  protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    virtual QPointF mapToProjection(const QPointF &point);
    virtual QPointF mapFromProjection(const QPointF &point);

  private:
    QRectF _selectionRect;
};

}

#endif

// vim: ts=2 sw=2 et
