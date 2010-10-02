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

#include "datagui.h"

#include "application.h"
#include "mainwindow.h"
#include "tabwidget.h"
#include "view.h"
#include "viewitem.h"
#include "viewgridlayout.h"
#include "plotitem.h"
#include "plotitemmanager.h"

namespace Kst {

DataGui::DataGui() {
}


DataGui::~DataGui() {
}


QList<PlotItemInterface*> DataGui::plotList() const {

  QList<PlotItemInterface*> plots;
  View *view = kstApp->mainWindow()->tabWidget()->currentView();
  if (!view)
    return plots;

  QList<PlotItem*> list = PlotItemManager::plotsForView(view);
  foreach (PlotItem *plotItem, list) {
    plots << plotItem;
  }
  return plots;
}


int DataGui::rows() const {
  return -1;
}


int DataGui::columns() const {
  return -1;
}


void DataGui::removeCurveFromPlots(Relation *relation) {
  View *view = kstApp->mainWindow()->tabWidget()->currentView();
  if (view) {
    QList<PlotItem*> list = PlotItemManager::plotsForView(view);
    foreach (PlotItem *plotItem, list) {
      bool plotUpdated = false;

      foreach (PlotRenderItem* renderItem, plotItem->renderItems()) {
        if (renderItem->relationList().contains(relation)) {
          renderItem->removeRelation(relation);
          plotUpdated = true;
        }
      }
      if (plotUpdated) {
        plotItem->update();
      }
    }
  }
}

}

// vim: ts=2 sw=2 et
