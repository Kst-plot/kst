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
  QList<ViewItem*> items = tieableItemsForView(view);
  foreach (ViewItem* item, items) {
    if (!item->isTiedZoom()) {
      bAllTied = false;
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
  QList<ViewItem *> tieable_items;
  if (view) {
    tieable_items = tieableItemsForView(view);
  } else {
    tieable_items = tieableItems();
  }

  foreach(ViewItem* viewItem, tieable_items) {
    if (viewItem->supportsTiedZoom()) {
      ++n_plots;
      if (viewItem->isTiedZoom()) {
        ++n_tied;
      }
    }
  }

  if (double(n_tied) > (double)n_plots*0.5) {
    tiedZoom = false;
  } else {
    tiedZoom = true;
  }

  foreach(ViewItem* viewItem, tieable_items) {
    if (viewItem->supportsTiedZoom()) {
      viewItem->setTiedZoom(tiedZoom, tiedZoom, false);
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
  QList<QGraphicsItem*> graphics_items = view->scene()->items();
  QList<PlotItem *> plot_items;

  foreach(QGraphicsItem* graphics_item, graphics_items) {
    PlotItem *item = dynamic_cast<PlotItem*>(graphics_item);
    if (item && item->isVisible()) {
      plot_items.append(item);
    }
  }

  qSort(plot_items.begin(), plot_items.end(), shortNameLessThan);

  return plot_items;
}

QList<ViewItem*> PlotItemManager::tieableItems() {
  QList<ViewItem*> allViewItems = ViewItem::getItems<ViewItem>();
  QList<ViewItem*> view_items;
  foreach (ViewItem *item, allViewItems) {
    if (item && item->isVisible() && item->supportsTiedZoom()) {
      view_items.append(item);
    }
  }
  return view_items;
}

QList<ViewItem*> PlotItemManager::tieableItemsForView(View *view) {
  QList<QGraphicsItem*> graphics_items = view->scene()->items();
  QList<ViewItem *> view_items;

  foreach(QGraphicsItem* graphics_item, graphics_items) {
    ViewItem *item = dynamic_cast<ViewItem*>(graphics_item);
    if (item && item->isVisible() && item->supportsTiedZoom()) {
      view_items.append(item);
    }
  }

  qSort(view_items.begin(), view_items.end(), shortNameLessThan);

  return view_items;

}

QList<PlotItem*> PlotItemManager::tiedZoomPlotsForView(View *view) {
  if (kstApp->mainWindow()->isTiedTabs()) {
    QList<PlotItem*> plots;
    QList<View*> keys = PlotItemManager::self()->_tiedZoomViewPlotLists.keys();
    foreach (View *view, keys) {
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
  QList<PlotItem*> plots = plotsForView(plotItem->view());
  foreach (PlotItem* plot, plots) {
    if (plotItem != plot) {
      plot->setAllowUpdates(false);
    }
  }
}


QList<PlotItem*> PlotItemManager::tiedZoomPlots(PlotItem* plotItem) {
  if (kstApp->mainWindow()->isTiedTabs()) {
    QList<PlotItem*> plots;
    QList<View *> keys = PlotItemManager::self()->_tiedZoomViewPlotLists.keys();
    foreach (View *view, keys) {
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
  QList<PlotItem*> plots = plotsForView(plotItem->view());
  foreach (PlotItem* plot, plots) {
    if (plotItem != plot) {
      plot->setAllowUpdates(true);
    }
  }
}


void PlotItemManager::clearFocusedPlots() {
  QList<PlotItem*> plotItems = ViewItem::getItems<PlotItem>();
  foreach (PlotItem* plotItem, plotItems) {
    if (plotItem->isMaximized()) {
      plotItem->plotMaximize();
    }
  }
}

}

// vim: ts=2 sw=2 et
