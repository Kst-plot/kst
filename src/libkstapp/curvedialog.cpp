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
#include "vectorcurverenderitem.h"

#include "defaultnames.h"
#include "datacollection.h"
#include "dataobjectcollection.h"

namespace Kst {

CurveTab::CurveTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Curve"));

  _xError->setAllowEmptySelection(true);
  _yError->setAllowEmptySelection(true);

  _xMinusError->setAllowEmptySelection(true);
  _yMinusError->setAllowEmptySelection(true);
}


CurveTab::~CurveTab() {
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


CurveDialog::CurveDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Curve"));
  else
    setWindowTitle(tr("New Curve"));

  _curveTab = new CurveTab(this);
  addDataTab(_curveTab);

  //FIXME need to do validation to enable/disable ok button...
}


CurveDialog::~CurveDialog() {
}


QString CurveDialog::tagName() const {
  return DataDialog::tagName();
}


ObjectPtr CurveDialog::createNewDataObject() const {
  //FIXME Eli, how should I construct this tag??
  CurvePtr curve = new Curve(tagName(),
                                     _curveTab->xVector(),
                                     _curveTab->yVector(),
                                     _curveTab->xError(),
                                     _curveTab->yError(),
                                     _curveTab->xMinusError(),
                                     _curveTab->yMinusError(),
                                     QColor(Qt::red));

  curve->writeLock();
  curve->update(0);
  curve->unlock();

  //FIXME assume new plot for now...
  //FIXME this should be a command...
  //FIXME need some smart placement...
  //FIXME need to hook up appearance and placement...
  CreatePlotForCurve *cmd = new CreatePlotForCurve;
  cmd->createItem();

  PlotItem *plotItem = static_cast<PlotItem*>(cmd->item());

  RelationList relationList;
  relationList.append(kst_cast<Relation>(curve));

  VectorCurveRenderItem *vectorCurve = new VectorCurveRenderItem(plotItem);
  vectorCurve->setRelationList(relationList);

  plotItem->addRenderItem(vectorCurve);

  return ObjectPtr(curve.data());
}


ObjectPtr CurveDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
