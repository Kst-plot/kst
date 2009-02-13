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
class PlotRenderItem;

class PlotRenderItem : public ViewItem
{
  Q_OBJECT
  public:
    enum RenderType { Cartesian, Polar, Sinusoidal };

    PlotRenderItem(PlotItem *parentItem);
    virtual ~PlotRenderItem();

    PlotItem *plotItem() const;

    RenderType type();
    void setType(RenderType type);

    QRectF plotRect() const;

    QRectF projectionRect() const;

    RelationList relationList() const;
    void addRelation(RelationPtr relation);
    void removeRelation(RelationPtr relation);
    void clearRelations();

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

    QRectF computedProjectionRect() const;
    void computeBorder(Qt::Orientation orientation, qreal *min, qreal *max) const;
    void resetSelectionRect();

    virtual bool tryShortcut(const QString &keySequence);

  public Q_SLOTS:
    virtual void edit();
    virtual void raise();
    virtual void lower();
    virtual void createAutoLayout();
    virtual void createCustomLayout();
    virtual void remove();
    virtual void relationUpdated(ObjectPtr object);
    virtual void referenceMode();
    virtual void referenceModeDisabled();

  protected:
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);

    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    virtual QPainterPath shape() const;
    virtual bool maybeReparent();

  private Q_SLOTS:
    void updateGeometry();
    void updateViewMode();

  private:
    virtual void addToMenuForContextEvent(QMenu &menu);

    void updateCursor(const QPointF &pos);
    void updateSelectionRect();

    void computeXAxisRange(qreal *min, qreal *max) const;
    void computeYAxisRange(qreal *min, qreal *max) const;
    void computeAuto(Qt::Orientation orientation, qreal *min, qreal *max) const;
    void computeMeanCentered(Qt::Orientation orientation, qreal *min, qreal *max) const;
    void computeNoSpike(Qt::Orientation orientation, qreal *min, qreal *max) const;

    void highlightNearestDataPoint(const QPointF& position);

  private:
    RenderType _type;
    QPointF _lastPos;
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
