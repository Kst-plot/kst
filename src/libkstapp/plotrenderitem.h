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

#include "relation.h"
#include "selectionrect.h"

namespace Kst {

class PlotItem;

enum RenderType { Cartesian, Polar, Sinusoidal };

class PlotRenderItem : public ViewItem
{
  Q_OBJECT
  public:
    enum ZoomMode { Auto, AutoBorder, Expression, SpikeInsensitive, MeanCentered };

    PlotRenderItem(PlotItem *parentItem);
    virtual ~PlotRenderItem();

    PlotItem *plotItem() const;

    RenderType type();
    void setType(RenderType type);

    bool isTiedZoom() const;
    void setTiedZoom(bool tiedZoom);

    ZoomMode xAxisZoomMode() const;
    void setXAxisZoomMode(ZoomMode mode);

    ZoomMode yAxisZoomMode() const;
    void setYAxisZoomMode(ZoomMode mode);

    bool isXAxisLog() const;
    void setXAxisLog(bool log);

    qreal xLogBase() const;
    void setXLogBase(qreal xLogBase);

    bool isYAxisLog() const;
    void setYAxisLog(bool log);

    qreal yLogBase() const;
    void setYLogBase(qreal yLogBase);

    QRectF plotRect() const;
    QRectF computedProjectionRect() const;

    QRectF projectionRect() const;
    void setProjectionRect(const QRectF &rect);

    RelationList relationList() const;
    void setRelationList(const RelationList &relationList);

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

public Q_SLOTS:
    void zoomMaximum();
    void zoomMaxSpikeInsensitive();
//     void zoomPrevious();
    void zoomYMeanCentered();
    void zoomXMaximum();
    void zoomXOut();
    void zoomXIn();
    void zoomNormalizeXtoY();
    void zoomLogX();
    void zoomYLocalMaximum();
    void zoomYMaximum();
    void zoomYOut();
    void zoomYIn();
    void zoomNormalizeYtoX();
    void zoomLogY();

  protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    virtual QTransform projectionTransform() const;

    virtual QPainterPath shape() const;
    virtual QRectF boundingRect() const;
    virtual QSizeF sizeOfGrip() const;
    virtual bool maybeReparent();
    QRectF checkBoxBoundingRect() const;
    QPainterPath checkBox() const;

  private Q_SLOTS:
    void updateGeometry();
    void updateViewMode();
    void xAxisRange(qreal *min, qreal *max) const;
    void yAxisRange(qreal *min, qreal *max) const;

  private:
    void createActions();
    void updateCursor(const QPointF &pos);

  private:
    RenderType _type;
    bool _isTiedZoom;
    ZoomMode _xAxisZoomMode;
    ZoomMode _yAxisZoomMode;
    bool _isXAxisLog;
    bool _isYAxisLog;
    qreal _xLogBase;
    qreal _yLogBase;

    RelationList _relationList;
    QRectF _projectionRect;
    SelectionRect _selectionRect;

    QAction *_zoomMaximum;
    QAction *_zoomMaxSpikeInsensitive;
    QAction *_zoomPrevious;
    QAction *_zoomYMeanCentered;
    QAction *_zoomXMaximum;
    QAction *_zoomXOut;
    QAction *_zoomXIn;
    QAction *_zoomNormalizeXtoY;
    QAction *_zoomLogX;
    QAction *_zoomYLocalMaximum;
    QAction *_zoomYMaximum;
    QAction *_zoomYOut;
    QAction *_zoomYIn;
    QAction *_zoomNormalizeYtoX;
    QAction *_zoomLogY;

};

}

#endif

// vim: ts=2 sw=2 et
