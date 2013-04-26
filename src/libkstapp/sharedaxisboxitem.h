/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SHAREDAXISBOXITEM_H
#define SHAREDAXISBOXITEM_H

#include "viewitem.h"
#include "plotitem.h"
#include "graphicsfactory.h"

namespace Kst {

class SharedAxisBoxItem : public ViewItem
{
  Q_OBJECT
  public:
    friend class SharedAxisBoxItemFactory;

    SharedAxisBoxItem(View *parent);
    virtual ~SharedAxisBoxItem();

    virtual void save(QXmlStreamWriter &xml);
    virtual void paint(QPainter *painter);

    virtual void addToMenuForContextEvent(QMenu &menu);
    void triggerContextEvent(QGraphicsSceneContextMenuEvent *event);

    bool tryMousePressEvent(ViewItem* viewItem, QGraphicsSceneMouseEvent *event);
    void setDirty() { _dirty = true; }

    bool isXAxisShared() const { return _shareX; }
    void setXAxisShared(const bool shared);

    bool isYAxisShared() const { return _shareY; }
    void setYAxisShared(const bool shared);

    PlotAxis::ZoomMode xAxisZoomMode() const { return _xAxisZoomMode; }
    void setXAxisZoomMode(PlotAxis::ZoomMode mode);

    PlotAxis::ZoomMode yAxisZoomMode() const { return _yAxisZoomMode; }
    void setYAxisZoomMode(PlotAxis::ZoomMode mode);

    void updateZoomForDataUpdate(qint64 serial);

    PlotItem* keyPlot() { return _keyPlot; };
    const QString defaultsGroupName() const {return QString("plot");}

    virtual int nRows();
    virtual int nCols();

  Q_SIGNALS:
    void breakShareSignal();

  public Q_SLOTS:
    void breakShare();
    bool acceptItems();
    void lockItems();

    void shareXAxis();
    void shareYAxis();

    void zoomFixedExpression(const QRectF &projection, PlotItem* originPlotItem);
    void zoomXRange(const QRectF &projection, PlotItem* originPlotItem);
    void zoomYRange(const QRectF &projection, PlotItem* originPlotItem);
    void zoomMaximum(PlotItem* originPlotItem);
    void zoomMaxSpikeInsensitive(PlotItem* originPlotItem);
    void zoomMeanCentered(PlotItem* originPlotItem);

    void zoomXMeanCentered(PlotItem* originPlotItem);
    void zoomXMaximum(PlotItem* originPlotItem);
    void zoomXNoSpike(PlotItem* originPlotItem);
    void zoomXAutoBorder(PlotItem* originPlotItem);
    void zoomXRight(PlotItem* originPlotItem);
    void zoomXLeft(PlotItem* originPlotItem);
    void zoomXOut(PlotItem* originPlotItem);
    void zoomXIn(PlotItem* originPlotItem);
    void zoomNormalizeXtoY(PlotItem* originPlotItem);
    void zoomLogX(PlotItem* originPlotItem, bool autoEnable = true, bool enable = true);

    void zoomYMeanCentered(PlotItem* originPlotItem);
    void zoomYLocalMaximum(PlotItem* originPlotItem);
    void zoomYMaximum(PlotItem* originPlotItem);
    void zoomYNoSpike(PlotItem* originPlotItem);
    void zoomYAutoBorder(PlotItem* originPlotItem);
    void zoomYUp(PlotItem* originPlotItem);
    void zoomYDown(PlotItem* originPlotItem);
    void zoomYOut(PlotItem* originPlotItem);
    void zoomYIn(PlotItem* originPlotItem);
    void zoomNormalizeYtoX(PlotItem* originPlotItem);
    void zoomLogY(PlotItem* originPlotItem, bool autoEnable = true, bool enable = true);
    QList<PlotItem*> getSharedPlots();

  protected Q_SLOTS:
    virtual void creationPolygonChanged(View::CreationEvent event);

  private:
    void updateShare();
    void updatePlotTiedZoomSupport();
    void highlightPlots(QList<PlotItem*> plots);

    QRectF computeRect(PlotAxis::ZoomMode xMode, PlotAxis::ZoomMode yMode);
    void applyZoom(const QRectF &projection, PlotItem* originPlotItem, bool applyX = true, bool applyY = true);

    QList<PlotItem*> getTiedPlots(PlotItem* originPlotItem);

    QAction *_breakAction;

    QPointer<ViewGridLayout> _layout;
    QList<PlotItem*> _highlightedPlots;
    QList<PlotItem*> _sharedPlots;
    PlotItem* _keyPlot;

    bool _loaded;
    bool _firstPaint;
    bool _dirty;
    bool _shareX, _shareY;
    qint64 _serialOfLastChange;

    PlotAxis::ZoomMode _xAxisZoomMode, _yAxisZoomMode;
    bool _sharedIsDirty;
};

class CreateSharedAxisBoxCommand : public CreateCommand
{
  Q_OBJECT
  public:
    CreateSharedAxisBoxCommand() : CreateCommand(QObject::tr("Create Shared Axis Box")) {}
    CreateSharedAxisBoxCommand(View *view) : CreateCommand(view, QObject::tr("Create Shared Axis Box")) {}
    virtual ~CreateSharedAxisBoxCommand() {}
    virtual void undo();
    virtual void redo();
    virtual void createItem();

  public Q_SLOTS:
    virtual void creationComplete();
};

class SharedAxisBoxItemFactory : public GraphicsFactory {
  public:
    SharedAxisBoxItemFactory();
    ~SharedAxisBoxItemFactory();
    ViewItem* generateGraphics(QXmlStreamReader& stream, ObjectStore *store, View *view, ViewItem *parent = 0);
};

}

#endif

// vim: ts=2 sw=2 et
