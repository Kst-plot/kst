/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PLOTRENDERITEM_H
#define PLOTRENDERITEM_H

#include "viewitem.h"

#include <QList>
#include <QPainterPath>

#include "kstrelation.h"
#include "selectionrect.h"

namespace Kst {

class PlotItem;

enum RenderType { Cartesian, Polar, Sinusoidal };

class PlotRenderItem : public ViewItem
{
  Q_OBJECT
  public:
    PlotRenderItem(const QString &name, PlotItem *parentItem);
    virtual ~PlotRenderItem();

    PlotItem *plotItem() const;

    void setType(RenderType type);
    RenderType type();

    QRectF plotRect() const;
    QRectF zoomRect() const;
    QRectF projectionRect() const;

    void setRelationList(const KstRelationList &relationList);
    KstRelationList relationList() const;

    virtual void paint(QPainter *painter);
    virtual void paintRelations(QPainter *painter) = 0;

    QString leftLabel() const;
    QString bottomLabel() const;
    QString rightLabel() const;
    QString topLabel() const;

    QPointF mapToProjection(const QPointF &point) const;
    QPointF mapFromProjection(const QPointF &point) const;
    QRectF mapToProjection(const QRectF &rect) const;
    QRectF mapFromProjection(const QRectF &rect) const;

  protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    virtual QTransform projectionTransform() const;

  private Q_SLOTS:
    void updateGeometry();
    void updateViewMode();

  private:
    RenderType _type;
    KstRelationList _relationList;
    QRectF _zoomRect;
    SelectionRect _selectionRect;
};

}

#endif

// vim: ts=2 sw=2 et
