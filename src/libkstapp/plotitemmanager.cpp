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


void PlotItemManager::removePlot(PlotItem *plotItem) {
  if (!_plotLists.contains(plotItem->parentView()))
    return;

  QList<PlotItem*> list = _plotLists.value(plotItem->parentView());
  list.removeAll(plotItem);
  _plotLists.insert(plotItem->parentView(), list);
}


void PlotItemManager::addTiedZoomPlot(PlotItem *plotItem) {
  if (!_tiedZoomPlotLists.contains(plotItem->parentView())) {
    _tiedZoomPlotLists.insert(plotItem->parentView(), QList<PlotItem*>() << plotItem);
  } else {
    QList<PlotItem*> list = _tiedZoomPlotLists.value(plotItem->parentView());
    list << plotItem;
    _tiedZoomPlotLists.insert(plotItem->parentView(), list);
  }
}


void PlotItemManager::removeTiedZoomPlot(PlotItem *plotItem) {
  if (!_tiedZoomPlotLists.contains(plotItem->parentView()))
    return;

  QList<PlotItem*> list = _tiedZoomPlotLists.value(plotItem->parentView());
  list.removeAll(plotItem);
  _tiedZoomPlotLists.insert(plotItem->parentView(), list);
}


QList<PlotItem*> PlotItemManager::plotsForView(View *view) {
  if (PlotItemManager::self()->_plotLists.contains(view)) {
    return PlotItemManager::self()->_plotLists.value(view);
  }
  return QList<PlotItem*>();
}


QList<PlotItem*> PlotItemManager::tiedZoomPlotsForView(View *view) {
  if (PlotItemManager::self()->_tiedZoomPlotLists.contains(view)) {
    return PlotItemManager::self()->_tiedZoomPlotLists.value(view);
  }
  return QList<PlotItem*>();
}

}

// vim: ts=2 sw=2 et
