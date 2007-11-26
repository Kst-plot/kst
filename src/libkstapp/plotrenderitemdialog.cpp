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

#include "plotrenderitemdialog.h"

#include "contenttab.h"
#include "dialogpage.h"
#include "application.h"
#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"

#include "curve.h"
#include "plotrenderitem.h"
#include "plotitem.h"

namespace Kst {

PlotRenderItemDialog::PlotRenderItemDialog(PlotRenderItem *item, QWidget *parent)
    : ViewItemDialog(item, parent), _plotItem(item) {

  _store = kstApp->mainWindow()->document()->objectStore();

  _contentTab = new ContentTab(this);
  connect(_contentTab, SIGNAL(apply()), this, SLOT(contentChanged()));

  DialogPage *page = new DialogPage(this);
  page->setPageTitle(tr("Content"));
  page->addDialogTab(_contentTab);
  addDialogPage(page);

  setupContent();
}


PlotRenderItemDialog::~PlotRenderItemDialog() {
}


void PlotRenderItemDialog::setupContent() {
  QStringList displayedCurves;
  QStringList availableCurves;

  CurveList curves = _store->getObjects<Curve>();

  foreach (RelationPtr relation, _plotItem->relationList()) {
    displayedCurves.append(relation->tag().displayString());
  }

  foreach (CurvePtr curve, curves) {
    if (!displayedCurves.contains(curve->tag().displayString())) {
      availableCurves.append(curve->tag().displayString());
    }
  }

  _contentTab->setDisplayedCurves(displayedCurves);
  _contentTab->setAvailableCurves(availableCurves);
}


void PlotRenderItemDialog::contentChanged() {
  QStringList currentCurves;
  foreach (RelationPtr relation, _plotItem->relationList()) {
    currentCurves.append(relation->tag().displayString());
  }

  QStringList displayedCurves = _contentTab->displayedCurves();

  foreach (QString curveTag, displayedCurves) {
    if (!currentCurves.contains(curveTag)) {
      CurvePtr curve = kst_cast<Curve>(_store->retrieveObject(ObjectTag::fromString(curveTag)));
      if (curve) {
        _plotItem->addRelation(curve);
        _plotItem->plotItem()->update();
      }
    }
  }
}

}

// vim: ts=2 sw=2 et
