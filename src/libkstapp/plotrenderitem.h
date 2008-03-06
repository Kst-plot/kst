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

struct ZoomState {
  QPointer<PlotRenderItem> item;
  QRectF projectionRect;
  int xAxisZoomMode;
  int yAxisZoomMode;
  bool isXAxisLog;
  bool isYAxisLog;
  qreal xLogBase;
  qreal yLogBase;
};

class PlotRenderItem : public ViewItem
{
  Q_OBJECT
  public:
    enum RenderType { Cartesian, Polar, Sinusoidal };
    enum ZoomMode { Auto, AutoBorder, FixedExpression, SpikeInsensitive, MeanCentered };

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

    QRectF plotRect() const;

    QRectF projectionRect() const;
    void setProjectionRect(const QRectF &rect);

    RelationList relationList() const;
    void addRelation(RelationPtr relation);
    void removeRelation(RelationPtr relation);
    void clearRelations();

    virtual void save(QXmlStreamWriter &xml);
    virtual void saveInPlot(QXmlStreamWriter &xml);
    virtual void paint(QPainter *painter);
    virtual void paintRelations(QPainter *painter) = 0;

    virtual bool configureFromXml(QXmlStreamReader &xml, ObjectStore *store);

    QString leftLabel() const;
    QString bottomLabel() const;
    QString rightLabel() const;
    QString topLabel() const;

Q_SIGNALS:
    void projectionRectChanged();

public Q_SLOTS:
    void zoomFixedExpression(const QRectF &projection);
    void zoomMaximum();
    void zoomMaxSpikeInsensitive();
    void zoomYMeanCentered();
    void zoomXMaximum();
    void zoomXRight();
    void zoomXLeft();
    void zoomXOut();
    void zoomXIn();
    void zoomNormalizeXtoY();
    void zoomLogX();
    void zoomYLocalMaximum();
    void zoomYMaximum();
    void zoomYUp();
    void zoomYDown();
    void zoomYOut();
    void zoomYIn();
    void zoomNormalizeYtoX();
    void zoomLogY();
    virtual void edit();

  protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    virtual QPainterPath shape() const;
    virtual QRectF boundingRect() const;
    virtual QSizeF sizeOfGrip() const;
    virtual bool maybeReparent();
    QRectF checkBoxBoundingRect() const;
    QPainterPath checkBox() const;

  private Q_SLOTS:
    void updateGeometry();
    void updateViewMode();

  private:
    void createActions();
    void updateCursor(const QPointF &pos);

    ZoomState currentZoomState();
    void setCurrentZoomState(ZoomState zoomState);

    QRectF computedProjectionRect() const;
    void computeXAxisRange(qreal *min, qreal *max) const;
    void computeYAxisRange(qreal *min, qreal *max) const;
    void computeAuto(Qt::Orientation orientation, qreal *min, qreal *max) const;
    void computeBorder(Qt::Orientation orientation, qreal *min, qreal *max) const;
    void computeMeanCentered(Qt::Orientation orientation, qreal *min, qreal *max) const;

  private:
    RenderType _type;
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
    QAction *_zoomXRight;
    QAction *_zoomXLeft;
    QAction *_zoomXOut;
    QAction *_zoomXIn;
    QAction *_zoomNormalizeXtoY;
    QAction *_zoomLogX;
    QAction *_zoomYLocalMaximum;
    QAction *_zoomYMaximum;
    QAction *_zoomYUp;
    QAction *_zoomYDown;
    QAction *_zoomYOut;
    QAction *_zoomYIn;
    QAction *_zoomNormalizeYtoX;
    QAction *_zoomLogY;

    friend class ZoomCommand;
    friend class ZoomMaximumCommand;
    friend class ZoomMaxSpikeInsensitiveCommand;
    friend class ZoomYMeanCenteredCommand;
    friend class ZoomXMaximumCommand;
    friend class ZoomYLocalMaximumCommand;
    friend class ZoomYMaximumCommand;
};

class KST_EXPORT ZoomCommand : public ViewItemCommand
{
  public:
    ZoomCommand(PlotRenderItem *item, const QString &text);
    virtual ~ZoomCommand();

    virtual void undo();
    virtual void redo();

    virtual void applyZoomTo(PlotRenderItem *item) = 0;

  private:
    QList<ZoomState> _originalStates;
};

class KST_EXPORT ZoomFixedExpressionCommand : public ZoomCommand
{
  public:
    ZoomFixedExpressionCommand(PlotRenderItem *item, const QRectF &fixed)
        : ZoomCommand(item, QObject::tr("Zoom Fixed Expression")), _fixed(fixed) {}
    virtual ~ZoomFixedExpressionCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);

  private:
    QRectF _fixed;
};

class KST_EXPORT ZoomMaximumCommand : public ZoomCommand
{
  public:
    ZoomMaximumCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Maximum")) {}
    virtual ~ZoomMaximumCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

class KST_EXPORT ZoomMaxSpikeInsensitiveCommand : public ZoomCommand
{
  public:
    ZoomMaxSpikeInsensitiveCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Max Spike Insensitive")) {}
    virtual ~ZoomMaxSpikeInsensitiveCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

class KST_EXPORT ZoomYMeanCenteredCommand : public ZoomCommand
{
  public:
    ZoomYMeanCenteredCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Y Mean Centered")) {}
    virtual ~ZoomYMeanCenteredCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

class KST_EXPORT ZoomXMaximumCommand : public ZoomCommand
{
  public:
    ZoomXMaximumCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Zoom X Maximum")) {}
    virtual ~ZoomXMaximumCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

class KST_EXPORT ZoomXRightCommand : public ZoomCommand
{
  public:
    ZoomXRightCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Scroll X Right")) {}
    virtual ~ZoomXRightCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

class KST_EXPORT ZoomXLeftCommand : public ZoomCommand
{
  public:
    ZoomXLeftCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Scroll X Left")) {}
    virtual ~ZoomXLeftCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

class KST_EXPORT ZoomXOutCommand : public ZoomCommand
{
  public:
    ZoomXOutCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Zoom X Out")) {}
    virtual ~ZoomXOutCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

class KST_EXPORT ZoomXInCommand : public ZoomCommand
{
  public:
    ZoomXInCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Zoom X In")) {}
    virtual ~ZoomXInCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

class KST_EXPORT ZoomNormalizeXToYCommand : public ZoomCommand
{
  public:
    ZoomNormalizeXToYCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Normalize X to Y")) {}
    virtual ~ZoomNormalizeXToYCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

class KST_EXPORT ZoomYLocalMaximumCommand : public ZoomCommand
{
  public:
    ZoomYLocalMaximumCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Y Local Maximum")) {}
    virtual ~ZoomYLocalMaximumCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

class KST_EXPORT ZoomYMaximumCommand : public ZoomCommand
{
  public:
    ZoomYMaximumCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Y Maximum")) {}
    virtual ~ZoomYMaximumCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

class KST_EXPORT ZoomYUpCommand : public ZoomCommand
{
  public:
    ZoomYUpCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Y Up")) {}
    virtual ~ZoomYUpCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

class KST_EXPORT ZoomYDownCommand : public ZoomCommand
{
  public:
    ZoomYDownCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Y Down")) {}
    virtual ~ZoomYDownCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

class KST_EXPORT ZoomYOutCommand : public ZoomCommand
{
  public:
    ZoomYOutCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Y Out")) {}
    virtual ~ZoomYOutCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

class KST_EXPORT ZoomYInCommand : public ZoomCommand
{
  public:
    ZoomYInCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Y In")) {}
    virtual ~ZoomYInCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

class KST_EXPORT ZoomNormalizeYToXCommand : public ZoomCommand
{
  public:
    ZoomNormalizeYToXCommand(PlotRenderItem *item)
        : ZoomCommand(item, QObject::tr("Zoom Normalize Y to X")) {}
    virtual ~ZoomNormalizeYToXCommand() {}

    virtual void applyZoomTo(PlotRenderItem *item);
};

}

#endif

// vim: ts=2 sw=2 et
