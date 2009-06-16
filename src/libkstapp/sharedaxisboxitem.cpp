/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sharedaxisboxitem.h"

#include "plotitem.h"
#include "plotitemmanager.h"

#include "gridlayouthelper.h"
#include "viewgridlayout.h"

#include "application.h"

#include <debug.h>

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>

// Zoom Debugging.  0 Off, 1 On.
#define DEBUG_ZOOM 0

namespace Kst {

SharedAxisBoxItem::SharedAxisBoxItem(View *parent)
    : ViewItem(parent),
      _layout(0),
      _loaded(false),
      _firstPaint(true),
      _dirty(false),
      _shareX(true),
      _shareY(true),
      _xAxisZoomMode(PlotAxis::Auto),
      _yAxisZoomMode(PlotAxis::Auto) {
  setTypeName("Shared Axis Box");
  setBrush(Qt::transparent);

  _breakAction = new QAction(tr("Break Shared Axis Box"), this);
  _breakAction->setShortcut(Qt::Key_B);
  registerShortcut(_breakAction);
  connect(_breakAction, SIGNAL(triggered()), this, SLOT(breakShare()));

  connect(this, SIGNAL(breakShareSignal()), this, SLOT(breakShare()));
}


SharedAxisBoxItem::~SharedAxisBoxItem() {
}


void SharedAxisBoxItem::paint(QPainter *painter) {
  if (_dirty) {
    if (_firstPaint && _loaded) {
      sharePlots(painter, true);
      _firstPaint = false;
    } else {
      sharePlots(painter, false);
    }
    _dirty = false;
  }
  painter->drawRect(rect());
}


void SharedAxisBoxItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("sharedaxisbox");
    xml.writeAttribute("tiedxzoom", QVariant(isXTiedZoom()).toString());
    xml.writeAttribute("tiedyzoom", QVariant(isYTiedZoom()).toString());
    xml.writeAttribute("sharex", QVariant(isXAxisShared()).toString());
    xml.writeAttribute("sharey", QVariant(isYAxisShared()).toString());
    xml.writeAttribute("xzoommode", QVariant(xAxisZoomMode()).toString());
    xml.writeAttribute("yzoommode", QVariant(yAxisZoomMode()).toString());
    ViewItem::save(xml);
    xml.writeEndElement();
  }
}


void SharedAxisBoxItem::setXAxisZoomMode(PlotAxis::ZoomMode mode) {
  _xAxisZoomMode = mode;
}


void SharedAxisBoxItem::setYAxisZoomMode(PlotAxis::ZoomMode mode) {
  _yAxisZoomMode = mode;
}


void SharedAxisBoxItem::setXAxisShared(const bool shared) {
  _shareX = shared;
  if (shared) {
    _xAxisZoomMode = PlotAxis::Auto;
  } else {
    _xAxisZoomMode = PlotAxis::FixedExpression;
  }
}


void SharedAxisBoxItem::setYAxisShared(const bool shared) {
  _shareY = shared;
  if (shared) {
    _yAxisZoomMode = PlotAxis::Auto;
  } else {
    _yAxisZoomMode = PlotAxis::FixedExpression;
  }
}


bool SharedAxisBoxItem::acceptItems() {
  bool bReturn = false;

  if (_loaded) {
    return true;
  } else {
    _loaded = true;
  }

  _sharedPlots.clear();
  QRectF maxSize(mapToParent(viewRect().topLeft()), mapToParent(viewRect().bottomRight()));
  ViewItem* child = 0;
  if (parentView()) {
    QList<QGraphicsItem*> list = parentView()->items();
    foreach (QGraphicsItem *item, list) {
      ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
      if (!viewItem || !viewItem->isVisible() || viewItem == this ||  viewItem == parentItem() || !collidesWithItem(viewItem, Qt::IntersectsItemBoundingRect)) {
        continue;
      }

      if (PlotItem *plotItem = qobject_cast<PlotItem*>(viewItem)) {
        if (plotItem->parentItem()) {
          ViewItem *parent = static_cast<ViewItem*>(plotItem->parentItem());
          SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(parent);
          if (shareBox) {
            shareBox->breakShare();
          } else if (parent != parentItem()) {
            continue;
          }
        } else if (parentItem()) {
          continue;
        }
        plotItem->setSharedAxisBox(this);
        _sharedPlots << plotItem;
        child = plotItem;
        if (!maxSize.contains(plotItem->mapToParent(plotItem->viewRect().topLeft()))) {
          maxSize.setTop(qMin(plotItem->mapToParent(plotItem->viewRect().topLeft()).y(), maxSize.top()));
          maxSize.setLeft(qMin(plotItem->mapToParent(plotItem->viewRect().topLeft()).x(), maxSize.left()));
        }
        if (!maxSize.contains(plotItem->mapToParent(plotItem->viewRect().bottomRight()))) {
          maxSize.setBottom(qMax(plotItem->mapToParent(plotItem->viewRect().bottomRight()).y(), maxSize.bottom()));
          maxSize.setRight(qMax(plotItem->mapToParent(plotItem->viewRect().bottomRight()).x(), maxSize.right()));
        }
      }
    }
    if (child) {
      setPen(QPen(Qt::white));
      setBrush(Qt::white);
      setSupportsTiedZoom(true);
      ViewGridLayout::updateProjections(this);
      _dirty = true;
      bReturn =  true;
    }
  }
  if (maxSize != viewRect()) {
    setPos(maxSize.topLeft());
    setViewRect(QRectF(mapFromParent(maxSize.topLeft()), mapFromParent(maxSize.bottomRight())));
  }
  return bReturn;
}


void SharedAxisBoxItem::breakShare() {
  _loaded = false;
  QList<QGraphicsItem*> list = QGraphicsItem::children();
  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
    if (!viewItem)
      continue;

    if (PlotItem *plotItem = qobject_cast<PlotItem*>(viewItem)) {
      plotItem->setPos(mapToParent(plotItem->pos()));
      plotItem->setSharedAxisBox(0);
      plotItem->setLabelsVisible(true);
      plotItem->update();
    }
  }
  if (_layout) {
    _layout->reset();
  }
  hide();
}


void SharedAxisBoxItem::lockItems() {
  _sharedPlots.clear();
  QList<QGraphicsItem*> list = QGraphicsItem::children();
  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
    if (!viewItem)
      continue;

    if (PlotItem *plotItem = qobject_cast<PlotItem*>(viewItem)) {
      plotItem->setAllowedGripModes(0);
      plotItem->setFlags(0);
      _sharedPlots << plotItem;
    }
  }
  if (!list.isEmpty()) {
    setPen(QPen(Qt::white));
    setBrush(Qt::white);
  }
}


void SharedAxisBoxItem::shareXAxis() {
  _shareX = !_shareX;
  updateShare();
}


void SharedAxisBoxItem::shareYAxis() {
  _shareY = !_shareY;
  updateShare();
}


void SharedAxisBoxItem::updateShare() {
  if (!_shareX && !_shareY) {
    breakShare();
  } else {
    ViewGridLayout::updateProjections(this, _shareX, _shareY);
    setDirty();
    update();
  }
}


void SharedAxisBoxItem::addToMenuForContextEvent(QMenu &menu) {
  menu.addAction(_breakAction);
}


void SharedAxisBoxItem::triggerContextEvent(QGraphicsSceneContextMenuEvent *event) {
  contextMenuEvent(event);
}


void SharedAxisBoxItem::creationPolygonChanged(View::CreationEvent event) {
  if (event == View::MousePress) {
    ViewItem::creationPolygonChanged(event);
    return;
  }

  if (event == View::MouseMove) {
    ViewItem::creationPolygonChanged(event);
    if (creationState() == ViewItem::None) {
      return;
    }

    QList<PlotItem*> plots;
    if (parentView()) {
      QList<QGraphicsItem*> list = parentView()->items();
      foreach (QGraphicsItem *item, list) {
        ViewItem *viewItem = qgraphicsitem_cast<ViewItem*>(item);
        if (!viewItem || !viewItem->isVisible() || viewItem == this ||  viewItem == parentItem() || !collidesWithItem(viewItem, Qt::IntersectsItemBoundingRect)) {
          continue;
        }
        if (PlotItem *plotItem = qobject_cast<PlotItem*>(viewItem)) {
          plots.append(plotItem);
        }
      }
      highlightPlots(plots);
    }
    return;
  }

  if (event == View::EscapeEvent || event == View::MouseRelease) {
    ViewItem::creationPolygonChanged(event);
    highlightPlots(QList<PlotItem*>());
    return;
  }
}


void SharedAxisBoxItem::highlightPlots(QList<PlotItem*> plots) {
  QList<PlotItem*> currentlyHighlighted = _highlightedPlots;
  _highlightedPlots.clear();

  foreach(PlotItem *plotItem, plots) {
    _highlightedPlots.append(plotItem);
    if (!currentlyHighlighted.contains(plotItem)) {
      plotItem->setHighlighted(true);
      plotItem->update();
    }
  }

  foreach(PlotItem* plotItem, currentlyHighlighted) {
    if (!_highlightedPlots.contains(plotItem)) {
      plotItem->setHighlighted(false);
      plotItem->update();
    }
  }
}


bool SharedAxisBoxItem::tryMousePressEvent(ViewItem* viewItem, QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    if (checkBox().contains(viewItem->mapToParent(event->pos()))) {
      setTiedZoom(!isTiedZoom(), !isTiedZoom());
      return true;
    }
  }
  return false;
}


// Should be cached.
QList<PlotItem*> SharedAxisBoxItem::getSharedPlots() {
  return _sharedPlots;
}


QList<PlotItem*> SharedAxisBoxItem::getTiedPlots() {
  QList<PlotItem*> plots = getSharedPlots();
  if (isTiedZoom()) {
    plots << PlotItemManager::tiedZoomPlotsForView(parentView());
  }
  return plots;
}


QRectF SharedAxisBoxItem::computeRect(PlotAxis::ZoomMode xZoomMode, PlotAxis::ZoomMode yZoomMode) {
  QRectF computedRect;
  foreach(PlotItem* plot, getSharedPlots()) {
    PlotAxis::ZoomMode existingXMode = plot->xAxis()->axisZoomMode();
    PlotAxis::ZoomMode existingYMode = plot->yAxis()->axisZoomMode();
    
    plot->xAxis()->setAxisZoomMode(xZoomMode);
    plot->yAxis()->setAxisZoomMode(yZoomMode);
    
    if (computedRect.isValid()) {
      computedRect = computedRect.united(plot->computedProjectionRect());
    } else {
      computedRect = plot->computedProjectionRect();
    }
    
    plot->xAxis()->setAxisZoomMode(existingXMode);
    plot->yAxis()->setAxisZoomMode(existingYMode);
  }
  return computedRect;
}


void SharedAxisBoxItem::applyZoom(const QRectF &projection, PlotItem* originPlotItem, bool applyX, bool applyY) {
  foreach (PlotItem* plotItem, getTiedPlots()) {
    if ((applyX && applyY) && ((_shareX && _shareY) || (isXTiedZoom() && isYTiedZoom()) || (plotItem == originPlotItem))) {
      plotItem->zoomFixedExpression(projection, true);
    } else if (applyX && (_shareX || isXTiedZoom() || (plotItem == originPlotItem))) {
      plotItem->zoomFixedExpression(QRectF(projection.x(), plotItem->projectionRect().y(), projection.width(), plotItem->projectionRect().height()), true);
    } else if (applyY && (_shareY || isYTiedZoom() || (plotItem == originPlotItem))) {
      plotItem->zoomFixedExpression(QRectF(plotItem->projectionRect().x(), projection.y(), plotItem->projectionRect().width(), projection.height()), true);
    }
  }
}


void SharedAxisBoxItem::zoomFixedExpression(const QRectF &projection, PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomFixedExpression" << projection;
#endif
  _xAxisZoomMode = PlotAxis::FixedExpression;
  _yAxisZoomMode = PlotAxis::FixedExpression;
  applyZoom(projection, originPlotItem);
}


void SharedAxisBoxItem::zoomXRange(const QRectF &projection, PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomXRange" << projection << endl;
#endif
  if (!_shareX) {
    if (originPlotItem) {
      originPlotItem->zoomXRange(projection, true);
    }
  } else {
    _xAxisZoomMode = PlotAxis::FixedExpression;
    applyZoom(projection, originPlotItem, true, false);
  }
}


void SharedAxisBoxItem::zoomYRange(const QRectF &projection, PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomYRange" << projection << endl;
#endif
  if (!_shareY) {
    if (originPlotItem) {
      originPlotItem->zoomYRange(projection, true);
    }
  } else {
    _yAxisZoomMode = PlotAxis::FixedExpression;
    applyZoom(projection, originPlotItem, false, true);
  }
}


void SharedAxisBoxItem::zoomMaximum(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomMaximum" << endl;
#endif
  _xAxisZoomMode = PlotAxis::Auto;
  _yAxisZoomMode = PlotAxis::AutoBorder;
  applyZoom(computeRect(PlotAxis::Auto, PlotAxis::AutoBorder), originPlotItem);
}


void SharedAxisBoxItem::zoomMaxSpikeInsensitive(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomMaxSpikeInsensitive" << endl;
#endif
  _xAxisZoomMode = PlotAxis::Auto;
  _yAxisZoomMode = PlotAxis::SpikeInsensitive;
  applyZoom(computeRect(PlotAxis::Auto, PlotAxis::SpikeInsensitive), originPlotItem);
}


void SharedAxisBoxItem::zoomYMeanCentered(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomYMeanCentered" << endl;
#endif
  _yAxisZoomMode = PlotAxis::MeanCentered;
  applyZoom(computeRect(PlotAxis::Auto, PlotAxis::MeanCentered), originPlotItem, false, true);
}


void SharedAxisBoxItem::zoomXMaximum(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomXMaximum" << endl;
#endif
  _xAxisZoomMode = PlotAxis::Auto;
  applyZoom(computeRect(PlotAxis::Auto, PlotAxis::Auto), originPlotItem, true, false);
}


void SharedAxisBoxItem::zoomXNoSpike(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomXNoSpike" << endl;
#endif
  _xAxisZoomMode = PlotAxis::SpikeInsensitive;
  applyZoom(computeRect(PlotAxis::SpikeInsensitive, PlotAxis::Auto), originPlotItem, true, false);
}


void SharedAxisBoxItem::zoomXAutoBorder(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomXAutoBorder" << endl;
#endif
  _xAxisZoomMode = PlotAxis::AutoBorder;
  applyZoom(computeRect(PlotAxis::AutoBorder, PlotAxis::Auto), originPlotItem, true, false);
}


void SharedAxisBoxItem::zoomXRight(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomXRight" << endl;
#endif
  if (!(_shareX || isXTiedZoom())) {
    if (originPlotItem) {
      originPlotItem->zoomXRight(true);
    }
  } else {
    _xAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, getTiedPlots()) {
      plotItem->zoomXRight(true);
    }
  }
}


void SharedAxisBoxItem::zoomXLeft(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomXLeft" << endl;
#endif
  if (!(_shareX || isXTiedZoom())) {
    if (originPlotItem) {
      originPlotItem->zoomXLeft(true);
    }
  } else {
    _xAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, getTiedPlots()) {
      plotItem->zoomXLeft(true);
    }
  }
}


void SharedAxisBoxItem::zoomXOut(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomXOut" << endl;
#endif
  if (!(_shareX || isXTiedZoom())) {
    if (originPlotItem) {
      originPlotItem->zoomXOut(true);
    }
  } else {
    _xAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, getTiedPlots()) {
      plotItem->zoomXOut(true);
    }
  }
}


void SharedAxisBoxItem::zoomXIn(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomXIn" << endl;
#endif
  if (!(_shareX || isXTiedZoom())) {
    if (originPlotItem) {
      originPlotItem->zoomXIn(true);
    }
  } else {
    _xAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, getTiedPlots()) {
      plotItem->zoomXIn(true);
    }
  }
}


void SharedAxisBoxItem::zoomNormalizeXtoY(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomNormalizeXtoY" << endl;
#endif
  if (!(_shareX || isXTiedZoom())) {
    if (originPlotItem) {
      originPlotItem->zoomNormalizeXtoY(true);
    }
  } else {
    QRectF computedRect;
    foreach(PlotItem *plotItem, getSharedPlots()) {
      QRectF compute = plotItem->projectionRect();
      qreal mean = compute.center().x();
      qreal range = plotItem->plotRect().width() * compute.height() / plotItem->plotRect().height();

      compute.setLeft(mean - (range / 2.0));
      compute.setRight(mean + (range / 2.0));

      if (computedRect.isValid()) {
        computedRect = computedRect.united(compute);
      } else {
        computedRect = compute;
      }
    }
    _xAxisZoomMode = PlotAxis::FixedExpression;
    applyZoom(computedRect, originPlotItem, true, false);
  }
}


void SharedAxisBoxItem::zoomLogX(PlotItem* originPlotItem, bool autoEnable, bool enable) {
#if DEBUG_ZOOM
  qDebug() << "zoomLogX" << endl;
#endif
  if (!(_shareX || isXTiedZoom())) {
    if (originPlotItem) {
      originPlotItem->zoomLogX(true);
    }
  } else {
    bool enableLog;
    if (autoEnable) {
      enableLog = !originPlotItem->xAxis()->axisLog();
    } else {
      enableLog = enable;
    }
    foreach(PlotItem* plotItem, getTiedPlots()) {
      plotItem->zoomLogX(true, false, enableLog);
    }
  }
}


void SharedAxisBoxItem::zoomYLocalMaximum(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomYLocalMaximum" << endl;
#endif
  if (!(_shareY || isYTiedZoom())) {
    if (originPlotItem) {
      originPlotItem->zoomYLocalMaximum(true);
    }
  } else {
    QRectF computedRect;
    foreach(PlotItem *plotItem, getSharedPlots()) {
      qreal minimum = plotItem->yAxis()->axisLog() ? 0.0 : -0.1;
      qreal maximum = 0.1;
      plotItem->computedRelationalMax(minimum, maximum);
      plotItem->computeBorder(Qt::Vertical, minimum, maximum);

      QRectF compute = plotItem->projectionRect();
      compute.setTop(minimum);
      compute.setBottom(maximum);

      if (computedRect.isValid()) {
        computedRect = computedRect.united(compute);
      } else {
        computedRect = compute;
      }
    }
    _yAxisZoomMode = PlotAxis::FixedExpression;
    applyZoom(computedRect, originPlotItem, false, true);
  }
}


void SharedAxisBoxItem::zoomYMaximum(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomYMaximum" << endl;
#endif
  _yAxisZoomMode = PlotAxis::Auto;
  applyZoom(computeRect(PlotAxis::Auto, PlotAxis::Auto), originPlotItem, false, true);
}


void SharedAxisBoxItem::zoomYNoSpike(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomYNoSpike" << endl;
#endif
  _yAxisZoomMode = PlotAxis::SpikeInsensitive;
  applyZoom(computeRect(PlotAxis::Auto, PlotAxis::SpikeInsensitive), originPlotItem, false, true);
}


void SharedAxisBoxItem::zoomYAutoBorder(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomYAutoBorder" << endl;
#endif
  _yAxisZoomMode = PlotAxis::AutoBorder;
  applyZoom(computeRect(PlotAxis::Auto, PlotAxis::AutoBorder), originPlotItem, false, true);
}


void SharedAxisBoxItem::zoomYUp(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomYUp" << endl;
#endif
  if (!(_shareY || isYTiedZoom())) {
    if (originPlotItem) {
      originPlotItem->zoomYUp(true);
    }
  } else {
    _yAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, getTiedPlots()) {
      plotItem->zoomYUp(true);
    }
  }
}


void SharedAxisBoxItem::zoomYDown(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomYDown" << endl;
#endif
  if (!(_shareY || isYTiedZoom())) {
    if (originPlotItem) {
      originPlotItem->zoomYDown(true);
    }
  } else {
    _yAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, getTiedPlots()) {
      plotItem->zoomYDown(true);
    }
  }
}


void SharedAxisBoxItem::zoomYOut(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomYOut" << endl;
#endif
  if (!(_shareY || isYTiedZoom())) {
    if (originPlotItem) {
      originPlotItem->zoomYOut(true);
    }
  } else {
    _yAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, getTiedPlots()) {
      plotItem->zoomYOut(true);
    }
  }
}


void SharedAxisBoxItem::zoomYIn(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomYIn" << endl;
#endif
  if (!(_shareY || isYTiedZoom())) {
    if (originPlotItem) {
      originPlotItem->zoomYIn(true);
    }
  } else {
    _yAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, getTiedPlots()) {
      plotItem->zoomYIn(true);
    }
  }
}


void SharedAxisBoxItem::zoomNormalizeYtoX(PlotItem* originPlotItem) {
#if DEBUG_ZOOM
  qDebug() << "zoomNormalizeYtoX" << endl;
#endif
  if (!(_shareY || isYTiedZoom())) {
    if (originPlotItem) {
      originPlotItem->zoomNormalizeXtoY(true);
    }
  } else {
    QRectF computedRect;
    foreach(PlotItem *plotItem, getSharedPlots()) {
      QRectF compute = plotItem->projectionRect();

      if (computedRect.isValid()) {
        computedRect = computedRect.united(compute);
      } else {
        computedRect = compute;
      }
    }
    qreal mean = computedRect.center().y();
    qreal range = originPlotItem->plotRect().width() * computedRect.height() / originPlotItem->plotRect().height();

    computedRect.setTop(mean - (range / 2.0));
    computedRect.setBottom(mean + (range / 2.0));

    _yAxisZoomMode = PlotAxis::FixedExpression;
    applyZoom(computedRect, originPlotItem, false, true);
  }
}


void SharedAxisBoxItem::zoomLogY(PlotItem* originPlotItem, bool autoEnable, bool enable) {
#if DEBUG_ZOOM
  qDebug() << "zoomLogY" << endl;
#endif
  if (!(_shareY || isYTiedZoom())) {
    if (originPlotItem) {
      originPlotItem->zoomLogY(true);
    }
  } else {
    bool enableLog;
    if (autoEnable && originPlotItem) {
      enableLog = !originPlotItem->yAxis()->axisLog();
    } else {
      enableLog = enable;
    }
    foreach(PlotItem* plotItem, getTiedPlots()) {
      plotItem->zoomLogY(true, false, enableLog);
    }
  }
}


void SharedAxisBoxItem::updateZoomForDataUpdate() {
  if (xAxisZoomMode() == PlotAxis::Auto && yAxisZoomMode() == PlotAxis::AutoBorder) {
    zoomMaximum(0);
  } else if (xAxisZoomMode() == PlotAxis::Auto && yAxisZoomMode() == PlotAxis::SpikeInsensitive) {
    zoomMaxSpikeInsensitive(0);
  } else {
    switch (xAxisZoomMode()) {
      case PlotAxis::Auto:
        zoomXMaximum(0);
        break;
      case PlotAxis::AutoBorder:
        zoomXAutoBorder(0);
        break;
      case PlotAxis::SpikeInsensitive:
        zoomXNoSpike(0);
        break;
      default:
        break;
    }
    switch (yAxisZoomMode()) {
      case PlotAxis::Auto:
        zoomYMaximum(0);
        break;
      case PlotAxis::AutoBorder:
        zoomYAutoBorder(0);
        break;
      case PlotAxis::SpikeInsensitive:
        zoomYNoSpike(0);
        break;
      case PlotAxis::MeanCentered:
        zoomYMeanCentered(0);
        break;
      default:
        break;
    }
  }
}


void CreateSharedAxisBoxCommand::createItem() {
  _item = new SharedAxisBoxItem(_view);
  _view->setCursor(Qt::CrossCursor);

  CreateCommand::createItem();
}


void CreateSharedAxisBoxCommand::undo() {
  Q_ASSERT(_item);
  _item->hide();
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(_item);
  if (shareBox) {
    shareBox->breakShare();
  }
}


void CreateSharedAxisBoxCommand::redo() {
  Q_ASSERT(_item);
  _item->show();
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(_item);
  if (shareBox) {
    if (!shareBox->acceptItems()) {
      _item->hide();
    }
  }
}


void CreateSharedAxisBoxCommand::creationComplete() {
  Q_ASSERT(_item);
  SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(_item);
  if (shareBox) {
    if (shareBox->acceptItems()) {
      CreateCommand::creationComplete();
    } else {
      delete _item;
      deleteLater();
      kstApp->mainWindow()->clearDrawingMarker();
    }
  }
}


SharedAxisBoxItemFactory::SharedAxisBoxItemFactory()
: GraphicsFactory() {
  registerFactory("sharedaxisbox", this);
}


SharedAxisBoxItemFactory::~SharedAxisBoxItemFactory() {
}


ViewItem* SharedAxisBoxItemFactory::generateGraphics(QXmlStreamReader& xml, ObjectStore *store, View *view, ViewItem *parent) {
  SharedAxisBoxItem *rc = 0;
  while (!xml.atEnd()) {
    bool validTag = true;
    if (xml.isStartElement()) {
      if (!rc && xml.name().toString() == "sharedaxisbox") {
        Q_ASSERT(!rc);
        rc = new SharedAxisBoxItem(view);
        if (parent) {
          rc->setParent(parent);
        }
        QXmlStreamAttributes attrs = xml.attributes();
        QStringRef av;
        bool xTiedZoom = false, yTiedZoom = false;
        av = attrs.value("tiedxzoom");
        if (!av.isNull()) {
          xTiedZoom = QVariant(av.toString()).toBool();
        }
        av = attrs.value("tiedyzoom");
        if (!av.isNull()) {
          yTiedZoom = QVariant(av.toString()).toBool();
        }
        rc->setTiedZoom(xTiedZoom, yTiedZoom);
        av = attrs.value("sharex");
        if (!av.isNull()) {
          rc->setXAxisShared(QVariant(av.toString()).toBool());
        }
        av = attrs.value("sharey");
        if (!av.isNull()) {
          rc->setYAxisShared(QVariant(av.toString()).toBool());
        }
        av = attrs.value("xzoommode");
        if (!av.isNull()) {
          rc->setXAxisZoomMode((PlotAxis::ZoomMode)av.toString().toInt());
        }
        av = attrs.value("yzoommode");
        if (!av.isNull()) {
          rc->setYAxisZoomMode((PlotAxis::ZoomMode)av.toString().toInt());
        }
        // Add any new specialized SharedAxisBoxItem Properties here.
      } else {
        Q_ASSERT(rc);
        if (!rc->parse(xml, validTag) && validTag) {
          ViewItem *i = GraphicsFactory::parse(xml, store, view, rc);
          if (!i) {
          }
        }
      }
    } else if (xml.isEndElement()) {
      if (xml.name().toString() == "sharedaxisbox") {
        break;
      } else {
        validTag = false;
      }
    }
    if (!validTag) {
      qDebug("invalid Tag\n");
      Debug::self()->log(QObject::tr("Error creating sharedaxisbox object from Kst file."), Debug::Warning);
      delete rc;
      return 0;
    }
    xml.readNext();
  }
  rc->lockItems();
  return rc;
}

}

// vim: ts=2 sw=2 et
