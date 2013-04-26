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

#include "sharedaxisboxitem.h"

#include "plotitem.h"
#include "plotitemmanager.h"

#include "viewgridlayout.h"
#include "formatgridhelper.h"

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
      _keyPlot(0),
      _loaded(false),
      _firstPaint(true),
      _dirty(false),
      _shareX(true),
      _shareY(true),
      _xAxisZoomMode(PlotAxis::Auto),
      _yAxisZoomMode(PlotAxis::Auto),
      _sharedIsDirty(false) {
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
    } else if (_sharedIsDirty) {
      sharePlots(painter, false);
    }
    _sharedIsDirty = false;
    updatePlotTiedZoomSupport();
    _keyPlot = 0;
    foreach (PlotItem* plotItem, _sharedPlots) {
      if (!_keyPlot) {
        _keyPlot = plotItem;
      } else {
        if ((plotItem->pos().x() > _keyPlot->pos().x()) || ((plotItem->pos().y() < _keyPlot->pos().y()))) {
          _keyPlot = plotItem;
        }
      }
    }
    _dirty = false;

  }
  painter->drawRect(rect());
}


void SharedAxisBoxItem::updatePlotTiedZoomSupport() {
  foreach (PlotItem* plot, _sharedPlots) {
    plot->setSupportsTiedZoom(!(_shareX && _shareY));
  }
}


void SharedAxisBoxItem::save(QXmlStreamWriter &xml) {
  if (isVisible()) {
    xml.writeStartElement("sharedaxisbox");
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
  _sharedIsDirty = true;
}


void SharedAxisBoxItem::setYAxisShared(const bool shared) {
  _shareY = shared;
  if (shared) {
    _yAxisZoomMode = PlotAxis::Auto;
  } else {
    _yAxisZoomMode = PlotAxis::FixedExpression;
  }
  _sharedIsDirty = true;
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
  if (view()) {
    QList<QGraphicsItem*> list = view()->items();
    foreach (QGraphicsItem *item, list) {
      ViewItem *viewItem = dynamic_cast<ViewItem*>(item);
      if (!viewItem || !viewItem->isVisible() || viewItem == this ||
          viewItem == parentItem() || !collidesWithItem(viewItem, Qt::IntersectsItemBoundingRect)) {
        continue;
      }

      if (PlotItem *plotItem = qobject_cast<PlotItem*>(viewItem)) {
        if (plotItem->parentItem()) {
          ViewItem *parent = static_cast<ViewItem*>(plotItem->parentItem());
          SharedAxisBoxItem *shareBox = qobject_cast<SharedAxisBoxItem*>(parent);
          if (shareBox) {
            // share boxes can't contain share boxes.  Bail out now!
            return false;
          } else if (parent != parentItem()) {
            continue;
          }
        } else if (parentItem()) {
          continue;
        }
        plotItem->setSharedAxisBox(this);

        _sharedPlots << plotItem;
        connect(plotItem, SIGNAL(breakShareTriggered()), this, SLOT(breakShare()));
        connect(plotItem, SIGNAL(shareXAxisTriggered()), this, SLOT(shareXAxis()));
        connect(plotItem, SIGNAL(shareYAxisTriggered()), this, SLOT(shareYAxis()));

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
      storePen(QPen(Qt::white));
      setBrush(Qt::white);
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

int SharedAxisBoxItem::nRows() {
  QList<PlotItem*> plotList = getSharedPlots();
  QList<ViewItem*> viewItemList;
  int n_plots = plotList.size();
  for (int i_plot = 0; i_plot<n_plots; i_plot++) {
    viewItemList.append(dynamic_cast<ViewItem*>(plotList.at(i_plot)));
  }

  FormatGridHelper grid(viewItemList);
  return grid.n_rows;
}


int SharedAxisBoxItem::nCols() {
  QList<PlotItem*> plotList = getSharedPlots();
  QList<ViewItem*> viewItemList;
  int n_plots = plotList.size();
  for (int i_plot = 0; i_plot<n_plots; i_plot++) {
    viewItemList.append(dynamic_cast<ViewItem*>(plotList.at(i_plot)));
  }

  FormatGridHelper grid(viewItemList);
  return grid.n_cols;
}


void SharedAxisBoxItem::breakShare() {
  _loaded = false;
  QList<PlotItem*> plotList = getSharedPlots();
  QList<ViewItem*> viewItemList;
  int n_plots = plotList.size();
  for (int i_plot = 0; i_plot<n_plots; i_plot++) {
    viewItemList.append(dynamic_cast<ViewItem*>(plotList.at(i_plot)));
  }

  FormatGridHelper grid(viewItemList);

  double height;
  double width;

  if (grid.n_rows>0) {
    height = double(rect().height())/double(grid.n_rows);
  } else {
    height = rect().height();
  }
  if (grid.n_cols>0) {
    width = double(rect().width())/double(grid.n_cols);
  } else {
    width = rect().width();
  }
  QPointF P0 = rect().topLeft();
  for (int i_plot = 0; i_plot<n_plots; i_plot++) {
    PlotItem *plotItem = plotList.at(i_plot);

    plotItem->setRect(0,0,width, height);
    plotItem->setPos(mapToParent(P0 + QPointF(grid.rcList.at(i_plot).col*width, grid.rcList.at(i_plot).row*height)));
    plotItem->setSharedAxisBox(0);
    plotItem->setTopSuppressed(false);
    plotItem->setBottomSuppressed(false);
    plotItem->setLeftSuppressed(false);
    plotItem->setRightSuppressed(false);
    plotItem->update();
  }
  if (_layout) {
    _layout->reset();
  }
  hide();
}


void SharedAxisBoxItem::lockItems() {
  _sharedPlots.clear();
  QList<QGraphicsItem*> list = QGraphicsItem::childItems();
  foreach (QGraphicsItem *item, list) {
    ViewItem *viewItem = dynamic_cast<ViewItem*>(item);
    if (!viewItem)
      continue;

    if (PlotItem *plotItem = qobject_cast<PlotItem*>(viewItem)) {
      plotItem->setAllowedGripModes(0);
      plotItem->setFlags(0);

      _sharedPlots << plotItem;
    }
  }
  if (!list.isEmpty()) {
    storePen(QPen(Qt::white));
    setBrush(Qt::white);
  }
}


void SharedAxisBoxItem::shareXAxis() {
  _shareX = !_shareX;
  _sharedIsDirty = true;
  updateShare();
}


void SharedAxisBoxItem::shareYAxis() {
  _shareY = !_shareY;
  _sharedIsDirty = true;
  updateShare();
}


void SharedAxisBoxItem::updateShare() {
  ViewGridLayout::updateProjections(this, _shareX, _shareY);
  view()->setPlotBordersDirty(true);
  setDirty();
  update();
  if (!_shareX && !_shareY) {
    breakShare();
  } else {
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
    if (view()) {
      QList<QGraphicsItem*> list = view()->items();
      foreach (QGraphicsItem *item, list) {
        ViewItem *viewItem = dynamic_cast<ViewItem*>(item);
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


QList<PlotItem*> SharedAxisBoxItem::getSharedPlots() {
  return _sharedPlots;
}

QList<PlotItem*> SharedAxisBoxItem::getTiedPlots(PlotItem* originPlotItem) {
  QList<PlotItem*> plots;

  if (originPlotItem) {
    return PlotItemManager::tiedZoomPlotsForView(view());
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
  QList<PlotItem*> allPlots = getSharedPlots();
  QList<PlotItem*> plotTied;
  if (originPlotItem && 
      originPlotItem->isTiedZoom() && 
      originPlotItem->isInSharedAxisBox() && 
      (originPlotItem->sharedAxisBox() == this)) {
    plotTied = PlotItemManager::tiedZoomPlotsForView(view());
    foreach (PlotItem* plotItem, plotTied) {      
      // tie only changes the unshared axis for shared plots
      if ((!_shareX || !plotItem->isInSharedAxisBox()) && applyX) {
        switch (xAxisZoomMode()) {
          case PlotAxis::Auto:
            plotItem->zoomXMaximum(true);
            break;
          case PlotAxis::AutoBorder:
            plotItem->zoomXAutoBorder(true);
            break;
          case PlotAxis::SpikeInsensitive:
            plotItem->zoomXNoSpike(true);
            break;
          default:
            plotItem->zoomXRange(projection,true);
            break;

        }
      }

      if ((!_shareY || !plotItem->isInSharedAxisBox()) && applyY) {
        switch (yAxisZoomMode()) {
          case PlotAxis::Auto:
            plotItem->zoomYMaximum(true);
            break;
          case PlotAxis::AutoBorder:
            plotItem->zoomYAutoBorder(true);
            break;
          case PlotAxis::SpikeInsensitive:
            plotItem->zoomYNoSpike(true);
            break;
          default:
            plotItem->zoomYRange(projection,true);
            break;
        }
      }
    }
  }
  foreach (PlotItem* plotItem, allPlots) {
    if ((applyX && applyY) && ((_shareX && _shareY) || (isXTiedZoom() && isYTiedZoom()))) {
      plotItem->zoomFixedExpression(projection, true);
    } else if (applyX && (_shareX || isXTiedZoom() || (plotItem == originPlotItem))) {
      plotItem->zoomXRange(QRectF(projection.x(), plotItem->projectionRect().y(), projection.width(), plotItem->projectionRect().height()), true);
    } else if (applyY && (_shareY || isYTiedZoom() || (plotItem == originPlotItem))) {
      plotItem->zoomYRange(QRectF(plotItem->projectionRect().x(), projection.y(), plotItem->projectionRect().width(), projection.height()), true);
    }
  }
}


void SharedAxisBoxItem::zoomFixedExpression(const QRectF &projection, PlotItem* originPlotItem) {
  _xAxisZoomMode = PlotAxis::FixedExpression;
  _yAxisZoomMode = PlotAxis::FixedExpression;
  if (originPlotItem) {
    originPlotItem->zoomFixedExpression(projection, true);
    if (originPlotItem->isTiedZoom() && originPlotItem->isInSharedAxisBox() && (originPlotItem->sharedAxisBox() == this)) {
      QList<PlotItem*> plotTied = PlotItemManager::tiedZoomPlotsForView(view());

      foreach(PlotItem* plotItem, plotTied) {
        plotItem->zoomFixedExpression(projection, true);
      }
    }
  }
  applyZoom(projection, originPlotItem, true, true);
}


void SharedAxisBoxItem::zoomXRange(const QRectF &projection, PlotItem* originPlotItem) {
  if (!_shareX) {
    if (originPlotItem) {
      originPlotItem->zoomXRange(projection, true);
    }
  } else {
    _xAxisZoomMode = PlotAxis::FixedExpression;
    applyZoom(projection, originPlotItem, true, false);
  }

  if (originPlotItem) {
    if (originPlotItem->isTiedZoom() && originPlotItem->isInSharedAxisBox() && (originPlotItem->sharedAxisBox() == this)) {
      QList<PlotItem*> plotTied = PlotItemManager::tiedZoomPlotsForView(view());

      foreach(PlotItem* plotItem, plotTied) {
        plotItem->zoomXRange(projection, true);
      }
    }
  }

}


void SharedAxisBoxItem::zoomYRange(const QRectF &projection, PlotItem* originPlotItem) {
  if (!_shareY) {
    if (originPlotItem) {
      originPlotItem->zoomYRange(projection, true);
    }
  } else {
    _yAxisZoomMode = PlotAxis::FixedExpression;
    applyZoom(projection, originPlotItem, false, true);
  }

  if (originPlotItem) {
    if (originPlotItem->isTiedZoom() && originPlotItem->isInSharedAxisBox() && (originPlotItem->sharedAxisBox() == this)) {
      QList<PlotItem*> plotTied = PlotItemManager::tiedZoomPlotsForView(view());

      foreach(PlotItem* plotItem, plotTied) {
        plotItem->zoomYRange(projection, true);
      }
    }
  }

}


void SharedAxisBoxItem::zoomMaximum(PlotItem* originPlotItem) {
  if (!originPlotItem) {
    originPlotItem = keyPlot();
  }

  _xAxisZoomMode = PlotAxis::Auto;
  _yAxisZoomMode = PlotAxis::AutoBorder;
  if (originPlotItem) {
    originPlotItem->zoomMaximum(true);
    if (originPlotItem->isTiedZoom() && originPlotItem->isInSharedAxisBox() && (originPlotItem->sharedAxisBox() == this)) {
      QList<PlotItem*> plotTied = PlotItemManager::tiedZoomPlotsForView(view());

      foreach(PlotItem* plotItem, plotTied) {
        plotItem->zoomMaximum(true);
      }
    }
  }
  applyZoom(computeRect(PlotAxis::Auto, PlotAxis::AutoBorder), originPlotItem);
}


void SharedAxisBoxItem::zoomMaxSpikeInsensitive(PlotItem* originPlotItem) {
  _xAxisZoomMode = PlotAxis::Auto;
  _yAxisZoomMode = PlotAxis::SpikeInsensitive;
  if (originPlotItem) {
    originPlotItem->zoomMaxSpikeInsensitive(true);
  }
  applyZoom(computeRect(PlotAxis::Auto, PlotAxis::SpikeInsensitive), originPlotItem);
}


void SharedAxisBoxItem::zoomMeanCentered(PlotItem* originPlotItem) {
  _yAxisZoomMode = PlotAxis::MeanCentered;
  applyZoom(computeRect(PlotAxis::Auto, PlotAxis::MeanCentered), originPlotItem, false, true);
}

void SharedAxisBoxItem::zoomYMeanCentered(PlotItem* originPlotItem) {
  _yAxisZoomMode = PlotAxis::MeanCentered;
  if (originPlotItem) {
    originPlotItem->zoomYMeanCentered(true);
  }
  applyZoom(computeRect(PlotAxis::Auto, PlotAxis::MeanCentered), originPlotItem, false, true);
}

void SharedAxisBoxItem::zoomXMeanCentered(PlotItem* originPlotItem) {
  _xAxisZoomMode = PlotAxis::MeanCentered;
  if (originPlotItem) {
    originPlotItem->zoomXMeanCentered(true);
  }
  applyZoom(computeRect(PlotAxis::MeanCentered, PlotAxis::MeanCentered), originPlotItem, true, false);
}

void SharedAxisBoxItem::zoomXMaximum(PlotItem* originPlotItem) {
  _xAxisZoomMode = PlotAxis::Auto;
  if (originPlotItem) {
    originPlotItem->zoomXMaximum(true);
  }
  applyZoom(computeRect(PlotAxis::Auto, PlotAxis::Auto), originPlotItem, true, false);
}


void SharedAxisBoxItem::zoomXNoSpike(PlotItem* originPlotItem) {
  _xAxisZoomMode = PlotAxis::SpikeInsensitive;
  if (originPlotItem) {
    originPlotItem->zoomXNoSpike(true);
  }
  applyZoom(computeRect(PlotAxis::SpikeInsensitive, PlotAxis::Auto), originPlotItem, true, false);
}


void SharedAxisBoxItem::zoomXAutoBorder(PlotItem* originPlotItem) {
  _xAxisZoomMode = PlotAxis::AutoBorder;
  if (originPlotItem) {
    originPlotItem->zoomXAutoBorder(true);
  }
  applyZoom(computeRect(PlotAxis::AutoBorder, PlotAxis::Auto), originPlotItem, true, false);
}


void SharedAxisBoxItem::zoomXRight(PlotItem* originPlotItem) {
  QList<PlotItem*> allPlots;
  if (_shareX) {
    allPlots = getSharedPlots();
  } else {
    allPlots = getTiedPlots(originPlotItem);
  }

  bool origin_tied = false;
  if (originPlotItem) {
    origin_tied = originPlotItem->isTiedZoom();
  }

  if (!(_shareX || origin_tied)) {
    if (originPlotItem) {
      originPlotItem->zoomXRight(true);
    }
  } else {
    _xAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, allPlots) {
      plotItem->zoomXRight(true);
    }
  }

  if (originPlotItem && originPlotItem->isTiedZoom() && originPlotItem->isInSharedAxisBox() && (originPlotItem->sharedAxisBox() == this)) {
    QList<PlotItem*> plotTied = PlotItemManager::tiedZoomPlotsForView(view());

    foreach(PlotItem* plotItem, plotTied) {
      if (!allPlots.contains(plotItem)) {
        plotItem->zoomXRight(true);
      }
    }
  }
}


void SharedAxisBoxItem::zoomXLeft(PlotItem* originPlotItem) {
  QList<PlotItem*> allPlots;
  if (_shareX) {
    allPlots = getSharedPlots();
  } else {
    allPlots = getTiedPlots(originPlotItem);
  }

  bool origin_tied = false;
  if (originPlotItem) {
    origin_tied = originPlotItem->isTiedZoom();
  }

  if (!(_shareX || origin_tied)) {
    if (originPlotItem) {
      originPlotItem->zoomXLeft(true);
    }
  } else {
    _xAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, allPlots) {
      plotItem->zoomXLeft(true);
    }
  }
  if (originPlotItem && originPlotItem->isTiedZoom() && originPlotItem->isInSharedAxisBox() && (originPlotItem->sharedAxisBox() == this)) {
    QList<PlotItem*> plotTied = PlotItemManager::tiedZoomPlotsForView(view());

    foreach(PlotItem* plotItem, plotTied) {
      if (!allPlots.contains(plotItem)) {
        plotItem->zoomXLeft(true);
      }
    }
  }
}


void SharedAxisBoxItem::zoomXOut(PlotItem* originPlotItem) {
  QList<PlotItem*> allPlots;
  if (_shareX) {
    allPlots = getSharedPlots();
  } else {
    allPlots = getTiedPlots(originPlotItem);
  }

  bool origin_tied = false;
  if (originPlotItem) {
    origin_tied = originPlotItem->isTiedZoom();
  }

  if (!(_shareX || origin_tied)) {
    if (originPlotItem) {
      originPlotItem->zoomXOut(true);
    }
  } else {
    _xAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, allPlots) {
      plotItem->zoomXOut(true);
    }
  }
  if (originPlotItem && originPlotItem->isTiedZoom() && originPlotItem->isInSharedAxisBox() && (originPlotItem->sharedAxisBox() == this)) {
    QList<PlotItem*> plotTied = PlotItemManager::tiedZoomPlotsForView(view());

    foreach(PlotItem* plotItem, plotTied) {
      if (!allPlots.contains(plotItem)) {
        plotItem->zoomXOut(true);
      }
    }
  }

}


void SharedAxisBoxItem::zoomXIn(PlotItem* originPlotItem) {
  QList<PlotItem*> allPlots;
  if (_shareX) {
    allPlots = getSharedPlots();
  } else {
    allPlots = getTiedPlots(originPlotItem);
  }

  bool origin_tied = false;
  if (originPlotItem) {
    origin_tied = originPlotItem->isTiedZoom();
  }

  if (!(_shareX || origin_tied)) {
    if (originPlotItem) {
      originPlotItem->zoomXIn(true);
    }
  } else {
    _xAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, allPlots) {
      plotItem->zoomXIn(true);
    }
  }
  if (originPlotItem && originPlotItem->isTiedZoom() && originPlotItem->isInSharedAxisBox() && (originPlotItem->sharedAxisBox() == this)) {
    QList<PlotItem*> plotTied = PlotItemManager::tiedZoomPlotsForView(view());

    foreach(PlotItem* plotItem, plotTied) {
      if (!allPlots.contains(plotItem)) {
        plotItem->zoomXIn(true);
      }
    }
  }

}


void SharedAxisBoxItem::zoomNormalizeXtoY(PlotItem* originPlotItem) {
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
    if (originPlotItem) {
      originPlotItem->zoomNormalizeXtoY(true);
    }
    applyZoom(computedRect, originPlotItem, true, false);
  }
}


void SharedAxisBoxItem::zoomLogX(PlotItem* originPlotItem, bool autoEnable, bool enable) {
  bool enableLog;
  if (autoEnable && originPlotItem) {
    enableLog = !originPlotItem->xAxis()->axisLog();
  } else {
    enableLog = enable;
  }
  QList<PlotItem*> allPlots;
  if (_shareX) {
    allPlots = getSharedPlots();
  } else {
    allPlots = getTiedPlots(originPlotItem);
  }

  bool origin_tied = false;
  if (originPlotItem) {
    origin_tied = originPlotItem->isTiedZoom();
  }

  if (!(_shareX || origin_tied)) {
    if (originPlotItem) {
      originPlotItem->zoomLogX(true, false, enableLog);
    }
  } else {
    foreach(PlotItem* plotItem, allPlots) {
      plotItem->zoomLogX(true, false, enableLog);
    }
  }
  if (originPlotItem && originPlotItem->isTiedZoom() && originPlotItem->isInSharedAxisBox() && (originPlotItem->sharedAxisBox() == this)) {
    QList<PlotItem*> plotTied = PlotItemManager::tiedZoomPlotsForView(view());

    foreach(PlotItem* plotItem, plotTied) {
      if (!allPlots.contains(plotItem)) {
        plotItem->zoomLogX(true, false, enableLog);
      }
    }
  }
}


void SharedAxisBoxItem::zoomYLocalMaximum(PlotItem* originPlotItem) {
  if (!originPlotItem) {
    originPlotItem = keyPlot();
  }

  _yAxisZoomMode = PlotAxis::FixedExpression;
  if (originPlotItem) {
    originPlotItem->zoomYLocalMaximum(true);
    if (originPlotItem->isTiedZoom() && originPlotItem->isInSharedAxisBox() && (originPlotItem->sharedAxisBox() == this)) {
      QList<PlotItem*> plotTied = PlotItemManager::tiedZoomPlotsForView(view());

      foreach(PlotItem* plotItem, plotTied) {
        plotItem->zoomYLocalMaximum(true);
      }
    }
  }
}

void SharedAxisBoxItem::zoomYMaximum(PlotItem* originPlotItem) {
  _yAxisZoomMode = PlotAxis::Auto;
  if (originPlotItem) {
    originPlotItem->zoomYMaximum(true);
  }
  applyZoom(computeRect(PlotAxis::Auto, PlotAxis::Auto), originPlotItem, false, true);
}


void SharedAxisBoxItem::zoomYNoSpike(PlotItem* originPlotItem) {
  _yAxisZoomMode = PlotAxis::SpikeInsensitive;
  if (originPlotItem) {
    originPlotItem->zoomYNoSpike(true);
  }
  applyZoom(computeRect(PlotAxis::Auto, PlotAxis::SpikeInsensitive), originPlotItem, false, true);
}


void SharedAxisBoxItem::zoomYAutoBorder(PlotItem* originPlotItem) {
  _yAxisZoomMode = PlotAxis::AutoBorder;
  if (originPlotItem) {
    originPlotItem->zoomYAutoBorder(true);
  }
  applyZoom(computeRect(PlotAxis::Auto, PlotAxis::AutoBorder), originPlotItem, false, true);
}


void SharedAxisBoxItem::zoomYUp(PlotItem* originPlotItem) {
  QList<PlotItem*> allPlots;
  if (_shareY) {
    allPlots = getSharedPlots();
  } else {
    allPlots = getTiedPlots(originPlotItem);
  }

  bool origin_tied = false;
  if (originPlotItem) {
    origin_tied = originPlotItem->isTiedZoom();
  }

  if (!(_shareY || origin_tied)) {
    if (originPlotItem) {
      originPlotItem->zoomYUp(true);
    }
  } else {
    _yAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, allPlots) {
      plotItem->zoomYUp(true);
    }
  }
  if (originPlotItem && originPlotItem->isTiedZoom() && originPlotItem->isInSharedAxisBox() && (originPlotItem->sharedAxisBox() == this)) {
    QList<PlotItem*> plotTied = PlotItemManager::tiedZoomPlotsForView(view());

    foreach(PlotItem* plotItem, plotTied) {
      if (!allPlots.contains(plotItem)) {
        plotItem->zoomYUp(true);
      }
    }
  }
}


void SharedAxisBoxItem::zoomYDown(PlotItem* originPlotItem) {
  QList<PlotItem*> allPlots;
  if (_shareY) {
    allPlots = getSharedPlots();
  } else {
    allPlots = getTiedPlots(originPlotItem);
  }

  bool origin_tied = false;
  if (originPlotItem) {
    origin_tied = originPlotItem->isTiedZoom();
  }

  if (!(_shareY || origin_tied)) {

    if (originPlotItem) {
      originPlotItem->zoomYDown(true);
    }
  } else {
    _yAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, allPlots) {
      plotItem->zoomYDown(true);
    }
  }
  if (originPlotItem && originPlotItem->isTiedZoom() && originPlotItem->isInSharedAxisBox() && (originPlotItem->sharedAxisBox() == this)) {
    QList<PlotItem*> plotTied = PlotItemManager::tiedZoomPlotsForView(view());

    foreach(PlotItem* plotItem, plotTied) {
      if (!allPlots.contains(plotItem)) {
        plotItem->zoomYDown(true);
      }
    }
  }
}


void SharedAxisBoxItem::zoomYOut(PlotItem* originPlotItem) {
  QList<PlotItem*> allPlots;
  if (_shareY) {
    allPlots = getSharedPlots();
  } else {
    allPlots = getTiedPlots(originPlotItem);
  }

  bool origin_tied = false;
  if (originPlotItem) {
    origin_tied = originPlotItem->isTiedZoom();
  }

  if (!(_shareY || origin_tied)) {

    if (originPlotItem) {
      originPlotItem->zoomYOut(true);
    }
  } else {
    _yAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, allPlots) {
      plotItem->zoomYOut(true);
    }
  }
  if (originPlotItem && originPlotItem->isTiedZoom() && originPlotItem->isInSharedAxisBox() && (originPlotItem->sharedAxisBox() == this)) {
    QList<PlotItem*> plotTied = PlotItemManager::tiedZoomPlotsForView(view());

    foreach(PlotItem* plotItem, plotTied) {
      if (!allPlots.contains(plotItem)) {
        plotItem->zoomYOut(true);
      }
    }
  }
}


void SharedAxisBoxItem::zoomYIn(PlotItem* originPlotItem) {
  QList<PlotItem*> allPlots;
  if (_shareY) {
    allPlots = getSharedPlots();
  } else {
    allPlots = getTiedPlots(originPlotItem);
  }

  bool origin_tied = false;
  if (originPlotItem) {
    origin_tied = originPlotItem->isTiedZoom();
  }

  if (!(_shareY || origin_tied)) {

    if (originPlotItem) {
      originPlotItem->zoomYIn(true);
    }
  } else {
    _yAxisZoomMode = PlotAxis::FixedExpression;
    foreach(PlotItem* plotItem, allPlots) {
      plotItem->zoomYIn(true);
    }
  }
  if (originPlotItem && originPlotItem->isTiedZoom() && originPlotItem->isInSharedAxisBox() && (originPlotItem->sharedAxisBox() == this)) {
    QList<PlotItem*> plotTied = PlotItemManager::tiedZoomPlotsForView(view());

    foreach(PlotItem* plotItem, plotTied) {
      if (!allPlots.contains(plotItem)) {
        plotItem->zoomYIn(true);
      }
    }
  }
}


void SharedAxisBoxItem::zoomNormalizeYtoX(PlotItem* originPlotItem) {
  if (!originPlotItem) {
    return;
  }

  bool origin_tied = false;
  if (originPlotItem) {
    origin_tied = originPlotItem->isTiedZoom();
  }

  if (!(_shareY || origin_tied)) {

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
    originPlotItem->zoomNormalizeYtoX(true);
    applyZoom(computedRect, originPlotItem, false, true);
  }
}


void SharedAxisBoxItem::zoomLogY(PlotItem* originPlotItem, bool autoEnable, bool enable) {
  QList<PlotItem*> allPlots;
  if (_shareY) {
    allPlots = getSharedPlots();
  } else {
    allPlots = getTiedPlots(originPlotItem);
  }
  bool enableLog;
  if (autoEnable && originPlotItem) {
    enableLog = !originPlotItem->yAxis()->axisLog();
  } else {
    enableLog = enable;
  }


  bool origin_tied = false;
  if (originPlotItem) {
    origin_tied = originPlotItem->isTiedZoom();
  }

  if (!(_shareY || origin_tied)) {

    if (originPlotItem) {
      originPlotItem->zoomLogY(true, false, enableLog);
    }
  } else {
    foreach(PlotItem* plotItem, allPlots) {
      plotItem->zoomLogY(true, false, enableLog);
    }
  }
  if (originPlotItem && originPlotItem->isTiedZoom() && originPlotItem->isInSharedAxisBox() && (originPlotItem->sharedAxisBox() == this)) {
    QList<PlotItem*> plotTied = PlotItemManager::tiedZoomPlotsForView(view());

    foreach(PlotItem* plotItem, plotTied) {
      if (!allPlots.contains(plotItem)) {
        plotItem->zoomLogY(true, false, enableLog);
      }
    }
  }
}



void SharedAxisBoxItem::updateZoomForDataUpdate(qint64 serial) {
  if (serial == _serialOfLastChange) {
    return;
  }
  _serialOfLastChange = serial;
  if (_shareX) {
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
  }
  if (_shareY) {
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
        zoomMeanCentered(0);
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
      kstApp->mainWindow()->clearDrawingMarker();
      deleteLater();
      _item->deleteLater();
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
          rc->setParentViewItem(parent);
        }
        QXmlStreamAttributes attrs = xml.attributes();
        QStringRef av = attrs.value("sharex");
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
          if (PlotItem *plotItem = qobject_cast<PlotItem*>(i)) {
            plotItem->setSharedAxisBox(rc);
            rc->_sharedPlots << plotItem;
            rc->connect(plotItem, SIGNAL(breakShareTriggered()), rc, SLOT(breakShare()));
            rc->connect(plotItem, SIGNAL(shareXAxisTriggered()), rc, SLOT(shareXAxis()));
            rc->connect(plotItem, SIGNAL(shareYAxisTriggered()), rc, SLOT(shareYAxis()));
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
