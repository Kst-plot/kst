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
#include "curvedialog.h"
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

  addCurves();

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


void PlotRenderItemDialog::addCurves() {
  foreach (RelationPtr relation, _plotItem->relationList()) {
    CurvePtr curve = kst_cast<Curve>(relation);
    if (curve) {
      CurveTab* curveTab = new CurveTab(this);

      curveTab->setObjectStore(_store);
      curveTab->setXVector(curve->xVector());
      curveTab->setYVector(curve->yVector());
      if (curve->hasXError()) {
        curveTab->setXError(curve->xErrorVector());
      }
      if (curve->hasYError()) {
      curveTab->setYError(curve->yErrorVector());
      }
      if (curve->hasXMinusError()) {
      curveTab->setXMinusError(curve->xMinusErrorVector());
      }
      if (curve->hasYMinusError()) {
        curveTab->setYMinusError(curve->yMinusErrorVector());
      }
      curveTab->curveAppearance()->setColor(curve->color());
      curveTab->curveAppearance()->setShowPoints(curve->hasPoints());
      curveTab->curveAppearance()->setShowLines(curve->hasLines());
      curveTab->curveAppearance()->setShowBars(curve->hasBars());
      curveTab->curveAppearance()->setLineWidth(curve->lineWidth());
      curveTab->curveAppearance()->setLineStyle(curve->lineStyle());
      curveTab->curveAppearance()->setPointType(curve->pointType());
      curveTab->curveAppearance()->setPointDensity(curve->pointDensity());
      curveTab->curveAppearance()->setBarStyle(curve->barStyle());
      curveTab->hidePlacementOptions();

      DialogPage *curvePage = new DialogPage(this);
      curvePage->setPageTitle(curve->tag().displayString());
      curvePage->addDialogTab(curveTab);
      addDialogPage(curvePage);
      _curvePages.append(curvePage);
    }
  }
}


void PlotRenderItemDialog::updateCurves() {
  foreach(DialogPage* page, _curvePages) {
    removeDialogPage(page);
  }
  _curvePages.clear();

  addCurves();
}


void PlotRenderItemDialog::contentChanged() {
  curveChanged();

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
  updateCurves();
}


void PlotRenderItemDialog::curveChanged() {
  foreach(DialogPage* page, _curvePages) {
    CurvePtr curve = kst_cast<Curve>(_store->retrieveObject(ObjectTag::fromString(page->pageTitle())));
    if (curve) {
      CurveTab* curveTab = static_cast<CurveTab*>(page->currentWidget());
      if (curveTab) {
        curve->writeLock();
        curve->setXVector(curveTab->xVector());
        curve->setYVector(curveTab->yVector());
        curve->setXError(curveTab->xError());
        curve->setYError(curveTab->yError());
        curve->setXMinusError(curveTab->xMinusError());
        curve->setYMinusError(curveTab->yMinusError());
        curve->setColor(curveTab->curveAppearance()->color());
        curve->setHasPoints(curveTab->curveAppearance()->showPoints());
        curve->setHasLines(curveTab->curveAppearance()->showLines());
        curve->setHasBars(curveTab->curveAppearance()->showBars());
        curve->setLineWidth(curveTab->curveAppearance()->lineWidth());
        curve->setLineStyle(curveTab->curveAppearance()->lineStyle());
        curve->setPointType(curveTab->curveAppearance()->pointType());
        curve->setPointDensity(curveTab->curveAppearance()->pointDensity());
        curve->setBarStyle(curveTab->curveAppearance()->barStyle());

        curve->update(0);
        curve->unlock();
      }
    }
  }
}

}

// vim: ts=2 sw=2 et
