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
  if (!_plotLists.contains(plotItem->view())) {
    _plotLists.insert(plotItem->view(), QList<PlotItem*>() << plotItem);
  } else {
    QList<PlotItem*> list = _plotLists.value(plotItem->view());
    list << plotItem;
    _plotLists.insert(plotItem->view(), list);
  }
}


void PlotItemManager::addViewItem(ViewItem *viewItem) {
  if (!_viewItemLists.contains(viewItem->view())) {
    _viewItemLists.insert(viewItem->view(), QList<ViewItem*>() << viewItem);
  } else {
    QList<ViewItem*> list = _viewItemLists.value(viewItem->view());
    list << viewItem;
    _viewItemLists.insert(viewItem->view(), list);
  }
}


void PlotItemManager::removePlot(PlotItem *plotItem) {
  if (!_plotLists.contains(plotItem->view()))
    return;
  QList<PlotItem*> list = _plotLists.value(plotItem->view());
  list.removeAll(plotItem);
  _plotLists.insert(plotItem->view(), list);
}


void PlotItemManager::removeViewItem(ViewItem *viewItem) {
  if (!_viewItemLists.contains(viewItem->view()))
    return;

  QList<ViewItem*> list = _viewItemLists.value(viewItem->view());
  list.removeAll(viewItem);
  _viewItemLists.insert(viewItem->view(), list);
}


void PlotItemManager::addTiedZoomPlot(PlotItem *plotItem, bool checkAll) {
  if (!_tiedZoomViewPlotLists.contains(plotItem->view())) {
    _tiedZoomViewPlotLists.insert(plotItem->view(), QList<PlotItem*>() << plotItem);
  } else {
    QList<PlotItem*> list = _tiedZoomViewPlotLists.value(plotItem->view());
    list << plotItem;
    _tiedZoomViewPlotLists.insert(plotItem->view(), list);
  }
  if (checkAll) {
    checkAllTied(plotItem->view());
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


void PlotItemManager::toggleAllTiedZoom(View *view) {

  bool tiedZoom;

  // vote on if we should tie all, or untie all
  int n_plots=0, n_tied=0;
  if (_viewItemLists.contains(view)) {
    foreach(ViewItem* viewItem, _viewItemLists[view]) {
      if (viewItem->supportsTiedZoom()) {
        ++n_plots;
        if (viewItem->isTiedZoom()) {
          ++n_tied;
        }
      }
    }
  }

  if (double(n_tied) > (double)n_plots*0.5) {
    tiedZoom = false;
  } else {
    tiedZoom = true;
  }

  if (_viewItemLists.contains(view)) {
    foreach(ViewItem* viewItem, _viewItemLists[view]) {
      if (viewItem->supportsTiedZoom()) {
        viewItem->setTiedZoom(tiedZoom, tiedZoom, false);
      }
    }
  }
}


void PlotItemManager::addTiedZoomViewItem(ViewItem *viewItem, bool checkAll) {
  if (!_tiedZoomViewItemLists.contains(viewItem->view())) {
    _tiedZoomViewItemLists.insert(viewItem->view(), QList<ViewItem*>() << viewItem);
  } else {
    QList<ViewItem*> list = _tiedZoomViewItemLists.value(viewItem->view());
    list << viewItem;
    _tiedZoomViewItemLists.insert(viewItem->view(), list);
  }
  if (checkAll) {
    checkAllTied(viewItem->view());
  }
}


void PlotItemManager::removeTiedZoomPlot(PlotItem *plotItem) {
  if (_tiedZoomViewItemPlotLists.contains(plotItem->parentViewItem())) {
    QList<PlotItem*> list = _tiedZoomViewItemPlotLists.value(plotItem->parentViewItem());
    list.removeAll(plotItem);
    _tiedZoomViewItemPlotLists.insert(plotItem->parentViewItem(), list);
  } else if (_tiedZoomViewPlotLists.contains(plotItem->view())) {
    QList<PlotItem*> list = _tiedZoomViewPlotLists.value(plotItem->view());
    list.removeAll(plotItem);
    _tiedZoomViewPlotLists.insert(plotItem->view(), list);
    emit tiedZoomRemoved();
  }
}


void PlotItemManager::removeTiedZoomViewItem(ViewItem *viewItem) {
  if (_tiedZoomViewItemLists.contains(viewItem->view())) {
    QList<ViewItem*> list = _tiedZoomViewItemLists.value(viewItem->view());
    list.removeAll(viewItem);
    _tiedZoomViewItemLists.insert(viewItem->view(), list);
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
  if (kstApp->mainWindow()->isTiedTabs()) {
    QList<PlotItem*> plots;
    foreach (View *view, PlotItemManager::self()->_tiedZoomViewPlotLists.keys()) {
      const QList<PlotItem*> plotlist = PlotItemManager::self()->_tiedZoomViewPlotLists.value(view);
      foreach(PlotItem* item, plotlist) {
        plots.append(item);
      }
    }
    return plots;
  } else {
    if (PlotItemManager::self()->_tiedZoomViewPlotLists.contains(view)) {
      QList<PlotItem*> plots = PlotItemManager::self()->_tiedZoomViewPlotLists.value(view);
      if (PlotItemManager::self()->_tiedZoomViewItemLists.contains(view)) {
        foreach (ViewItem *viewItem, PlotItemManager::self()->_tiedZoomViewItemLists.value(view)) {
          plots << tiedZoomPlotsForViewItem(viewItem);
        }
      }
      return plots;
    }
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
  if (_plotLists.contains(plotItem->view())) {
    foreach (PlotItem* plot, _plotLists.value(plotItem->view())) {
      if (plotItem != plot) {
        plot->setAllowUpdates(false);
      }
    }
  }
}


QList<PlotItem*> PlotItemManager::tiedZoomPlots(PlotItem* plotItem) {
  if (kstApp->mainWindow()->isTiedTabs()) {
    QList<PlotItem*> plots;
    foreach (View *view, PlotItemManager::self()->_tiedZoomViewPlotLists.keys()) {
      const QList<PlotItem*> plotlist = PlotItemManager::self()->_tiedZoomViewPlotLists.value(view);
      foreach(PlotItem* item, plotlist) {
        plots.append(item);
      }
    }

    return plots;

  } else {
    if (plotItem->isInSharedAxisBox() && !plotItem->parentViewItem()->isTiedZoom()) {
      return tiedZoomPlotsForViewItem(plotItem->parentViewItem());
    } else {
      return tiedZoomPlotsForView(plotItem->view());
    }
  }
}

QList<ViewItem*> PlotItemManager::tiedZoomViewItems(PlotItem* plotItem) {
  return PlotItemManager::self()->_tiedZoomViewItemLists.value(plotItem->view());
}


void PlotItemManager::removeFocusPlot(PlotItem *plotItem) {
  _focusedPlots.removeAll(plotItem);
  if (_plotLists.contains(plotItem->view())) {
    foreach (PlotItem* plot, _plotLists.value(plotItem->view())) {
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
