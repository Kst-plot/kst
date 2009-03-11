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

#include "plotitemmanager.h"

#include "view.h"
#include "plotitem.h"

#include <QDebug>
#include <QCoreApplication>

namespace Kst {

static PlotItemManager *_self = 0;
void PlotItemManager::cleanup() {
  delete _self;
  _self = 0;
}


PlotItemManager *PlotItemManager::self() {
  if (!_self) {
    _self = new PlotItemManager;
    qAddPostRoutine(cleanup);
  }
  return _self;
}


PlotItemManager::PlotItemManager() {
}


PlotItemManager::~PlotItemManager() {
}


void PlotItemManager::addPlot(PlotItem *plotItem) {
  if (!_plotLists.contains(plotItem->parentView())) {
    _plotLists.insert(plotItem->parentView(), QList<PlotItem*>() << plotItem);
  } else {
    QList<PlotItem*> list = _plotLists.value(plotItem->parentView());
    list << plotItem;
    _plotLists.insert(plotItem->parentView(), list);
  }
}


void PlotItemManager::addViewItem(ViewItem *viewItem) {
  if (!_viewItemLists.contains(viewItem->parentView())) {
    _viewItemLists.insert(viewItem->parentView(), QList<ViewItem*>() << viewItem);
  } else {
    QList<ViewItem*> list = _viewItemLists.value(viewItem->parentView());
    list << viewItem;
    _viewItemLists.insert(viewItem->parentView(), list);
  }
}


void PlotItemManager::removePlot(PlotItem *plotItem) {
  if (!_plotLists.contains(plotItem->parentView()))
    return;

  QList<PlotItem*> list = _plotLists.value(plotItem->parentView());
  list.removeAll(plotItem);
  _plotLists.insert(plotItem->parentView(), list);
}


void PlotItemManager::removeViewItem(ViewItem *viewItem) {
  if (!_viewItemLists.contains(viewItem->parentView()))
    return;

  QList<ViewItem*> list = _viewItemLists.value(viewItem->parentView());
  list.removeAll(viewItem);
  _viewItemLists.insert(viewItem->parentView(), list);
}


void PlotItemManager::addTiedZoomPlot(PlotItem *plotItem, bool checkAll) {
  if (plotItem->isInSharedAxisBox()) {
    if (!_tiedZoomViewItemPlotLists.contains(plotItem->parentViewItem())) {
      _tiedZoomViewItemPlotLists.insert(plotItem->parentViewItem(), QList<PlotItem*>() << plotItem);
    } else {
      QList<PlotItem*> list = _tiedZoomViewItemPlotLists.value(plotItem->parentViewItem());
      list << plotItem;
      _tiedZoomViewItemPlotLists.insert(plotItem->parentViewItem(), list);
    }
  } else {
    if (!_tiedZoomViewPlotLists.contains(plotItem->parentView())) {
      _tiedZoomViewPlotLists.insert(plotItem->parentView(), QList<PlotItem*>() << plotItem);
    } else {
      QList<PlotItem*> list = _tiedZoomViewPlotLists.value(plotItem->parentView());
      list << plotItem;
      _tiedZoomViewPlotLists.insert(plotItem->parentView(), list);
    }
    if (checkAll) {
      checkAllTied(plotItem->parentView());
    }
  }
}


void PlotItemManager::checkAllTied(View* view) {
  bool bAllTied = true;
  if (_plotLists.contains(view)) {
    foreach(PlotItem* plot, _plotLists[view]) {
      if (plot->supportsTiedZoom() && !plot->isTiedZoom()) {
        bAllTied = false;
      }
    }
  }
  if (_viewItemLists.contains(view)) {
    foreach(ViewItem* viewItem, _viewItemLists[view]) {
      if (viewItem->supportsTiedZoom() && !viewItem->isTiedZoom()) {
        bAllTied = false;
      }
    }
  }
  if (bAllTied) {
    emit allPlotsTiedZoom();
  }
}


void PlotItemManager::setAllTiedZoom(View *view, bool tiedZoom) {
  if (_plotLists.contains(view)) {
    foreach(PlotItem* plot, _plotLists[view]) {
      if (plot->supportsTiedZoom()) {
        plot->setTiedZoom(tiedZoom, false);
      }
    }
  }
  if (_viewItemLists.contains(view)) {
    foreach(ViewItem* viewItem, _viewItemLists[view]) {
      if (viewItem->supportsTiedZoom()) {
        viewItem->setTiedZoom(tiedZoom, false);
      }
    }
  }
}


void PlotItemManager::addTiedZoomViewItem(ViewItem *viewItem, bool checkAll) {
  if (!_tiedZoomViewItemLists.contains(viewItem->parentView())) {
    _tiedZoomViewItemLists.insert(viewItem->parentView(), QList<ViewItem*>() << viewItem);
  } else {
    QList<ViewItem*> list = _tiedZoomViewItemLists.value(viewItem->parentView());
    list << viewItem;
    _tiedZoomViewItemLists.insert(viewItem->parentView(), list);
  }
  if (checkAll) {
    checkAllTied(viewItem->parentView());
  }
}


void PlotItemManager::removeTiedZoomPlot(PlotItem *plotItem) {
  if (_tiedZoomViewItemPlotLists.contains(plotItem->parentViewItem())) {
    QList<PlotItem*> list = _tiedZoomViewItemPlotLists.value(plotItem->parentViewItem());
    list.removeAll(plotItem);
    _tiedZoomViewItemPlotLists.insert(plotItem->parentViewItem(), list);
  } else if (_tiedZoomViewPlotLists.contains(plotItem->parentView())) {
    QList<PlotItem*> list = _tiedZoomViewPlotLists.value(plotItem->parentView());
    list.removeAll(plotItem);
    _tiedZoomViewPlotLists.insert(plotItem->parentView(), list);
    emit tiedZoomRemoved();
  }
}


void PlotItemManager::removeTiedZoomViewItem(ViewItem *viewItem) {
  if (_tiedZoomViewItemLists.contains(viewItem->parentView())) {
    QList<ViewItem*> list = _tiedZoomViewItemLists.value(viewItem->parentView());
    list.removeAll(viewItem);
    _tiedZoomViewItemLists.insert(viewItem->parentView(), list);
    emit tiedZoomRemoved();
  }
}


QList<PlotItem*> PlotItemManager::plotsForView(View *view) {
  if (PlotItemManager::self()->_plotLists.contains(view)) {
    return PlotItemManager::self()->_plotLists.value(view);
  }
  return QList<PlotItem*>();
}


QList<PlotItem*> PlotItemManager::tiedZoomPlotsForView(View *view) {
  if (PlotItemManager::self()->_tiedZoomViewPlotLists.contains(view)) {
    QList<PlotItem*> plots = PlotItemManager::self()->_tiedZoomViewPlotLists.value(view);
    if (PlotItemManager::self()->_tiedZoomViewItemLists.contains(view)) {
      foreach (ViewItem *viewItem, PlotItemManager::self()->_tiedZoomViewItemLists.value(view)) {
        plots << tiedZoomPlotsForViewItem(viewItem);
      }
    }
    return plots;
  }
  return QList<PlotItem*>();
}


QList<PlotItem*> PlotItemManager::tiedZoomPlotsForViewItem(ViewItem *viewItem) {
  if (PlotItemManager::self()->_tiedZoomViewItemPlotLists.contains(viewItem)) {
    return PlotItemManager::self()->_tiedZoomViewItemPlotLists.value(viewItem);
  }
  return QList<PlotItem*>();
}


void PlotItemManager::setFocusPlot(PlotItem *plotItem) {
  _focusedPlots.append(plotItem);
  if (_plotLists.contains(plotItem->parentView())) {
    foreach (PlotItem* plot, _plotLists.value(plotItem->parentView())) {
      if (plotItem != plot) {
        plot->setAllowUpdates(false);
      }
    }
  }
}


QList<PlotItem*> PlotItemManager::tiedZoomPlots(PlotItem* plotItem) {
  if (plotItem->isInSharedAxisBox() && !plotItem->parentViewItem()->isTiedZoom()) {
    return tiedZoomPlotsForViewItem(plotItem->parentViewItem());
  } else {
    return tiedZoomPlotsForView(plotItem->parentView());
  }
}


void PlotItemManager::removeFocusPlot(PlotItem *plotItem) {
  _focusedPlots.removeAll(plotItem);
  if (_plotLists.contains(plotItem->parentView())) {
    foreach (PlotItem* plot, _plotLists.value(plotItem->parentView())) {
      if (plotItem != plot) {
        plot->setAllowUpdates(true);
      }
    }
  }
}


void PlotItemManager::clearFocusedPlots() {
  foreach (PlotItem* plotItem, _focusedPlots) {
    plotItem->plotMaximize();
  }
}

}

// vim: ts=2 sw=2 et
