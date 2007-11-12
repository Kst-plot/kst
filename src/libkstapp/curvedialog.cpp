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

#include "curvedialog.h"

#include "dialogpage.h"

#include "curve.h"

#include "view.h"
#include "plotitem.h"
#include "tabwidget.h"
#include "mainwindow.h"
#include "application.h"
#include "plotrenderitem.h"

#include "defaultnames.h"
#include "datacollection.h"
#include "dataobjectcollection.h"
#include "document.h"
#include "objectstore.h"

#include <QPushButton>

namespace Kst {

CurveTab::CurveTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Curve"));

  _xError->setAllowEmptySelection(true);
  _yError->setAllowEmptySelection(true);

  _xMinusError->setAllowEmptySelection(true);
  _yMinusError->setAllowEmptySelection(true);

  _curvePlacement->setExistingPlots(Data::self()->plotList());

  connect(_xVector, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
  connect(_yVector, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
}


CurveTab::~CurveTab() {
}


void CurveTab::selectionChanged() {
  emit vectorsChanged();
}

VectorPtr CurveTab::xVector() const {
  return _xVector->selectedVector();
}


void CurveTab::setXVector(VectorPtr vector) {
  _xVector->setSelectedVector(vector);
}


VectorPtr CurveTab::yVector() const {
  return _yVector->selectedVector();
}


void CurveTab::setYVector(VectorPtr vector) {
  _yVector->setSelectedVector(vector);
}


VectorPtr CurveTab::xError() const {
  return _xError->selectedVector();
}


void CurveTab::setXError(VectorPtr vector) {
  _xError->setSelectedVector(vector);
}


VectorPtr CurveTab::yError() const {
  return _yError->selectedVector();
}


void CurveTab::setYError(VectorPtr vector) {
  _yError->setSelectedVector(vector);
}


VectorPtr CurveTab::xMinusError() const {
  return _xMinusError->selectedVector();
}


void CurveTab::setXMinusError(VectorPtr vector) {
  _xMinusError->setSelectedVector(vector);
}


VectorPtr CurveTab::yMinusError() const {
  return _yMinusError->selectedVector();
}


void CurveTab::setYMinusError(VectorPtr vector) {
  _yMinusError->setSelectedVector(vector);
}


void CurveTab::setObjectStore(ObjectStore *store) {
  _xVector->setObjectStore(store);
  _yVector->setObjectStore(store);
  _xError->setObjectStore(store);
  _yError->setObjectStore(store);
  _xMinusError->setObjectStore(store);
  _yMinusError->setObjectStore(store);
}


CurveAppearance* CurveTab::curveAppearance() const {
  return _curveAppearance;
}


CurvePlacement* CurveTab::curvePlacement() const {
  return _curvePlacement;
}


CurveDialog::CurveDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Curve"));
  else
    setWindowTitle(tr("New Curve"));

  _curveTab = new CurveTab(this);
  addDataTab(_curveTab);

  connect(_curveTab, SIGNAL(vectorsChanged()), this, SLOT(updateButtons()));
  updateButtons();
}


CurveDialog::~CurveDialog() {
}


QString CurveDialog::tagString() const {
  return DataDialog::tagString();
}


void CurveDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_curveTab->xVector() && _curveTab->yVector());
}


ObjectPtr CurveDialog::createNewDataObject() const {
  Q_ASSERT(_document && _document->objectStore());
  CurvePtr curve = _document->objectStore()->createObject<Curve>(ObjectTag::fromString(tagString()));
#if 0
  CurvePtr curve = new Curve(_document->objectStore(),
                             ObjectTag::fromString(tagString()),
                             _curveTab->xVector(),
                             _curveTab->yVector(),
                             _curveTab->xError(),
                             _curveTab->yError(),
                             _curveTab->xMinusError(),
                             _curveTab->yMinusError(),
                             _curveTab->curveAppearance()->color());
#endif

  curve->setXVector(_curveTab->xVector());
  curve->setYVector(_curveTab->yVector());
  curve->setXError(_curveTab->xError());
  curve->setYError(_curveTab->yError());
  curve->setXMinusError(_curveTab->xMinusError());
  curve->setYMinusError(_curveTab->yMinusError());
  curve->setColor(_curveTab->curveAppearance()->color());
  curve->setHasPoints(_curveTab->curveAppearance()->showPoints());
  curve->setHasLines(_curveTab->curveAppearance()->showLines());
  curve->setHasBars(_curveTab->curveAppearance()->showBars());
  curve->setLineWidth(_curveTab->curveAppearance()->lineWidth());
  curve->setLineStyle(_curveTab->curveAppearance()->lineStyle());
  curve->setPointType(_curveTab->curveAppearance()->pointType());
  curve->setPointDensity(_curveTab->curveAppearance()->pointDensity());
  curve->setBarStyle(_curveTab->curveAppearance()->barStyle());

  curve->writeLock();
  curve->update(0);
  curve->unlock();

  PlotItem *plotItem = 0;
  switch (_curveTab->curvePlacement()->place()) {
  case CurvePlacement::NoPlot:
    break;
  case CurvePlacement::ExistingPlot:
    {
      plotItem = static_cast<PlotItem*>(_curveTab->curvePlacement()->existingPlot());
      break;
    }
  case CurvePlacement::NewPlot:
    {
      CreatePlotForCurve *cmd = new CreatePlotForCurve(
        _curveTab->curvePlacement()->createLayout(),
        _curveTab->curvePlacement()->appendToLayout());
      cmd->createItem();

      plotItem = static_cast<PlotItem*>(cmd->item());
      break;
    }
  default:
    break;
  }

  PlotRenderItem *renderItem = plotItem->renderItem(PlotRenderItem::Cartesian);
  renderItem->addRelation(kst_cast<Relation>(curve));
  plotItem->update();

  return ObjectPtr(curve.data());
}


ObjectPtr CurveDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
