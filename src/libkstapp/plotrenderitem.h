/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
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
class PlotRenderItem;

class PlotRenderItem : public ViewItem
{
  Q_OBJECT
  public:
    enum RenderType {First, Cartesian, Polar, Sinusoidal };

    PlotRenderItem(PlotItem *parentItem);
    virtual ~PlotRenderItem();

    PlotItem *plotItem() const;

    virtual RenderType renderType() const;
    void setRenderType(RenderType type);

    QRectF plotRect() const;

    QRectF projectionRect() const;

    RelationList relationList() const;
    void addRelation(RelationPtr relation);
    void removeRelation(RelationPtr relation);
    void clearRelations();
    void setRelationsList(const RelationList &relations);

    virtual void save(QXmlStreamWriter &xml);
    virtual void saveInPlot(QXmlStreamWriter &xml);
    virtual void paint(QPainter *painter);
    virtual void paintRelations(QPainter *painter) = 0;
    void paintReferencePoint(QPainter *painter);
    void paintHighlightPoint(QPainter *painter);

    virtual bool configureFromXml(QXmlStreamReader &xml, ObjectStore *store);

    QString leftLabel() const;
    QString bottomLabel() const;
    QString rightLabel() const;
    QString topLabel() const;
    QString multiRenderItemLabel(bool isX) const;

    QRectF computedProjectionRect() const;
    void computeBorder(Qt::Orientation orientation, double *min, double *max) const;
    void resetSelectionRect();

    virtual bool tryShortcut(const QString &keySequence);

    QList<PlotItem*> sharedOrTiedPlots(bool sharedX, bool sharedY);

    void hoverYZoomMouseCursor(qreal y);
    void hoverXZoomMouseCursor(qreal x);

    void dragYZoomMouseCursor(qreal y);
    void dragXZoomMouseCursor(qreal x);
    QPointF statusMessagePoint;
  public Q_SLOTS:
    virtual void edit();
    virtual void raise();
    virtual void lower();
    virtual void createAutoLayout();
    virtual void createCustomLayout(int columns = 0);
    virtual void remove();
    virtual void referenceMode();
    virtual void referenceModeDisabled();

  protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void wheelEvent(QGraphicsSceneWheelEvent *event);

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    virtual QPainterPath shape() const;
    virtual bool updateViewItemParent();

  private Q_SLOTS:
    void updateGeometry();
    void updateViewMode();

  private:
    virtual void addToMenuForContextEvent(QMenu &menu);

    void updateCursor(const QPointF &pos);
    void updateSelectionRect();

    void computeXAxisRange(double *min, double *max) const;
    void computeYAxisRange(double *min, double *max) const;
    void computeAuto(Qt::Orientation orientation, double *min, double *max) const;
    void computeMeanCentered(Qt::Orientation orientation, double *min, double *max) const;
    void computeNoSpike(Qt::Orientation orientation, double *min, double *max) const;

    void highlightNearestDataPoint(const QPointF& position);
    void setReferencePoint(const QPointF& point);
    void processHoverMoveEvent(const QPointF& p);
  private:
    RenderType _type;
    QPointF _lastPos;
    QPointF _hoverPos;
    bool _referencePointMode;
    QPointF _referencePoint;
    bool _highlightPointActive;
    bool _invertHighlight;
    QPointF _highlightPoint;
    QAction *_referenceMode;
    QAction *_referenceModeDisabled;

    RelationList _relationList;
    SelectionRect _selectionRect;
};

}

#endif

// vim: ts=2 sw=2 et
