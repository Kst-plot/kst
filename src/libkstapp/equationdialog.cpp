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
#include "plotrenderitem.h"
#include "curve.h"
#include "defaultnames.h"

namespace Kst {

EquationTab::EquationTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Equation"));

  populateFunctionList();

  _curvePlacement->setExistingPlots(Data::self()->plotList());
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


CurveAppearance* EquationTab::curveAppearance() const {
  return _curveAppearance;
}


CurvePlacement* EquationTab::curvePlacement() const {
  return _curvePlacement;
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
  //FIXME need to hook up placement...

  CurvePtr curve = new Curve(suggestCurveName(equation->tag(), true),
                                     equation->vX(),
                                     equation->vY(),
                                     0L, 0L, 0L, 0L,
                                     _equationTab->curveAppearance()->color());

  curve->setHasPoints(_equationTab->curveAppearance()->showPoints());
  curve->setHasLines(_equationTab->curveAppearance()->showLines());
  curve->setHasBars(_equationTab->curveAppearance()->showBars());
  curve->setLineWidth(_equationTab->curveAppearance()->lineWidth());
  curve->setLineStyle(_equationTab->curveAppearance()->lineStyle());
  curve->pointType = _equationTab->curveAppearance()->pointType();
  curve->setPointDensity(_equationTab->curveAppearance()->pointDensity());
  curve->setBarStyle(_equationTab->curveAppearance()->barStyle());

  curve->writeLock();
  curve->update(0);
  curve->unlock();

  PlotItem *plotItem = 0;
  switch (_equationTab->curvePlacement()->place()) {
  case CurvePlacement::NoPlot:
    break;
  case CurvePlacement::ExistingPlot:
    {
      plotItem = static_cast<PlotItem*>(_equationTab->curvePlacement()->existingPlot());
      break;
    }
  case CurvePlacement::NewPlot:
    {
      CreatePlotForCurve *cmd = new CreatePlotForCurve(
        _equationTab->curvePlacement()->createLayout(),
        _equationTab->curvePlacement()->appendToLayout());
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

  return ObjectPtr(equation.data());
}


ObjectPtr EquationDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
