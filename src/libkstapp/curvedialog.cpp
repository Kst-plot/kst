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

#include "kstvcurve.h"

#include "view.h"
#include "plotitem.h"
#include "tabwidget.h"
#include "mainwindow.h"
#include "application.h"
#include "vectorcurverenderitem.h"

#include "kstdefaultnames.h"
#include "kstdatacollection.h"
#include "kstdataobjectcollection.h"

namespace Kst {

CurveTab::CurveTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Curve"));
}


CurveTab::~CurveTab() {
}


KstVectorPtr CurveTab::xVector() const {
  return _xVector->selectedVector();
}


void CurveTab::setXVector(KstVectorPtr vector) {
  _xVector->setSelectedVector(vector);
}


KstVectorPtr CurveTab::yVector() const {
  return _yVector->selectedVector();
}


void CurveTab::setYVector(KstVectorPtr vector) {
  _yVector->setSelectedVector(vector);
}


KstVectorPtr CurveTab::xError() const {
  return _xError->selectedVector();
}


void CurveTab::setXError(KstVectorPtr vector) {
  _xError->setSelectedVector(vector);
}


KstVectorPtr CurveTab::yError() const {
  return _yError->selectedVector();
}


void CurveTab::setYError(KstVectorPtr vector) {
  _yError->setSelectedVector(vector);
}


KstVectorPtr CurveTab::xMinusError() const {
  return _xMinusError->selectedVector();
}


void CurveTab::setXMinusError(KstVectorPtr vector) {
  _xMinusError->setSelectedVector(vector);
}


KstVectorPtr CurveTab::yMinusError() const {
  return _yMinusError->selectedVector();
}


void CurveTab::setYMinusError(KstVectorPtr vector) {
  _yMinusError->setSelectedVector(vector);
}


CurveDialog::CurveDialog(KstObjectPtr dataObject, QWidget *parent)
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


KstObjectPtr CurveDialog::createNewDataObject() const {
  //FIXME Eli, how should I construct this tag??
  KstVCurvePtr curve = new KstVCurve(tagName(),
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
  PlotItem *plotItem = new PlotItem(kstApp->mainWindow()->tabWidget()->currentView());
  plotItem->setPos(0, 0);
  plotItem->setViewRect(0.0, 0.0, 100.0, 100.0);
  plotItem->parentView()->scene()->addItem(plotItem);
  plotItem->setZValue(1);

  KstRelationList relationList;
  relationList.append(kst_cast<KstRelation>(curve));

  VectorCurveRenderItem *test = new VectorCurveRenderItem("cartesiantest", plotItem);
  test->setRelationList(relationList);

  plotItem->addRenderItem(test);

  return KstObjectPtr(curve.data());
}


KstObjectPtr CurveDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
