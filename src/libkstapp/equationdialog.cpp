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

#include "equationdialog.h"

#include "dialogpage.h"

#include "datacollection.h"
#include "dataobjectcollection.h"
#include "equation.h"
#include "plotitem.h"
#include "vectorcurverenderitem.h"
#include "curve.h"
#include "defaultnames.h"

namespace Kst {

EquationTab::EquationTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Equation"));

  populateFunctionList();
}


EquationTab::~EquationTab() {
}


void EquationTab::populateFunctionList() {
  Operators->clear();
  Operators->addItem("+");
  Operators->addItem("-");
  Operators->addItem("*");
  Operators->addItem("/");
  Operators->addItem("%");
  Operators->addItem("^");
  Operators->addItem("&");
  Operators->addItem("|");
  Operators->addItem("&&");
  Operators->addItem("||");
  Operators->addItem("!");
  Operators->addItem("<");
  Operators->addItem("<=");
  Operators->addItem("==");
  Operators->addItem(">=");
  Operators->addItem(">");
  Operators->addItem("!=");
  Operators->addItem("PI");
  Operators->addItem("e");
  Operators->addItem("STEP()");
  Operators->addItem("ABS()");
  Operators->addItem("SQRT()");
  Operators->addItem("CBRT()");
  Operators->addItem("SIN()");
  Operators->addItem("COS()");
  Operators->addItem("TAN()");
  Operators->addItem("ASIN()");
  Operators->addItem("ACOS()");
  Operators->addItem("ATAN()");
  Operators->addItem("SEC()");
  Operators->addItem("CSC()");
  Operators->addItem("COT()");
  Operators->addItem("SINH()");
  Operators->addItem("COSH()");
  Operators->addItem("TANH()");
  Operators->addItem("EXP()");
  Operators->addItem("LN()");
  Operators->addItem("LOG()");
  Operators->addItem("PLUGIN()");
}


VectorPtr EquationTab::xVector() const {
  return _xVectors->selectedVector();
}


void EquationTab::setXVector(VectorPtr vector) {
  _xVectors->setSelectedVector(vector);
}


QString EquationTab::equation() const {
  return _equation->text();
}


void EquationTab::setEquation(const QString &equation) {
  _equation->setText(equation);
}


bool EquationTab::doInterpolation() const {
  return _doInterpolation->isChecked();
}


void EquationTab::setDoInterpolation(bool doInterpolation) {
  _doInterpolation->setChecked(doInterpolation);
}


EquationDialog::EquationDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Equation"));
  else
    setWindowTitle(tr("New Equation"));

  _equationTab = new EquationTab(this);
  addDataTab(_equationTab);

  //FIXME need to do validation to enable/disable ok button...
}


EquationDialog::~EquationDialog() {
}


QString EquationDialog::tagName() const {
  return DataDialog::tagName();
}


ObjectPtr EquationDialog::createNewDataObject() const {
  //FIXME Eli, how should I construct this tag??
  EquationPtr equation = new Equation(tagName(),
                                     _equationTab->equation(),
                                     _equationTab->xVector(),
                                     _equationTab->doInterpolation());

  equation->writeLock();
  equation->update(0);
  equation->unlock();

  //FIXME assume new plot for now...
  //FIXME this should be a command...
  //FIXME need some smart placement...
  //FIXME need to hook up appearance and placement...

  CurvePtr curve = new Curve(suggestCurveName(equation->tag(), true),
                                     equation->vX(),
                                     equation->vY(),
                                     0L, 0L, 0L, 0L,
                                     QColor(Qt::red));

  curve->writeLock();
  curve->update(0);
  curve->unlock();

  CreatePlotForCurve *cmd = new CreatePlotForCurve;
  cmd->createItem();

  PlotItem *plotItem = static_cast<PlotItem*>(cmd->item());

  RelationList relationList;
  relationList.append(kst_cast<Relation>(curve));

  VectorCurveRenderItem *vectorCurve = new VectorCurveRenderItem(plotItem);
  vectorCurve->setRelationList(relationList);

  plotItem->addRenderItem(vectorCurve);

  return ObjectPtr(equation.data());
}


ObjectPtr EquationDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
