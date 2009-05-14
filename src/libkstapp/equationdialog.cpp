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
#include "editmultiplewidget.h"

#include "datacollection.h"
#include "equation.h"
#include "plotitem.h"
#include "plotrenderitem.h"
#include "curve.h"
#include "document.h"
#include "objectstore.h"

#include <QPushButton>

namespace Kst {

EquationTab::EquationTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Equation"));

  populateFunctionList();

  _curvePlacement->setExistingPlots(Data::self()->plotList());

  _xVectorLabel->setBuddy(_xVectors->_vector);
  _scalarsLabel->setBuddy(_scalars->_scalar);
  _vectorsLabel->setBuddy(_vectors->_vector);
  connect(_xVectors, SIGNAL(selectionChanged(QString)), this, SLOT(selectionChanged()));
  connect(_equation, SIGNAL(textChanged(const QString &)), this, SLOT(selectionChanged()));
  connect(Operators, SIGNAL(activated(QString)), this, SLOT(equationOperatorUpdate(const QString&)));
  connect(_vectors, SIGNAL(selectionChanged(QString)), this, SLOT(equationUpdate(const QString&)));
  connect(_scalars, SIGNAL(selectionChanged(QString)), this, SLOT(equationUpdate(const QString&)));

  connect(_xVectors, SIGNAL(contentChanged()), this, SLOT(updateVectorCombos()));
  connect(_vectors, SIGNAL(contentChanged()), this, SLOT(updateVectorCombos()));

  connect(_xVectors, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));
  connect(_equation, SIGNAL(textChanged(const QString &)), this, SIGNAL(modified()));
  connect(_doInterpolation, SIGNAL(clicked()), this, SIGNAL(modified()));
}


EquationTab::~EquationTab() {
}


void EquationTab::selectionChanged() {
  emit optionsChanged();
}


void EquationTab::equationUpdate(const QString& string) {
  QString equation = _equation->text();
  equation += "[" + string + "]";
  _equation->setText(equation); 
}


void EquationTab::equationOperatorUpdate(const QString& string) {
  QString equation = _equation->text();
  equation += string;
  _equation->setText(equation); 
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


bool EquationTab::xVectorDirty() const {
  return _xVectors->selectedVectorDirty();
}


void EquationTab::setXVector(VectorPtr vector) {
  _xVectors->setSelectedVector(vector);
}


QString EquationTab::equation() const {
  return _equation->text();
}


bool EquationTab::equationDirty() const {
  return (!_equation->text().isEmpty());
}


void EquationTab::setEquation(const QString &equation) {
  _equation->setText(equation);
}


bool EquationTab::doInterpolation() const {
  return _doInterpolation->isChecked();
}


bool EquationTab::doInterpolationDirty() const {
  return _doInterpolation->checkState() != Qt::PartiallyChecked;
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


void EquationTab::setObjectStore(ObjectStore *store) {
  _vectors->setObjectStore(store);
  _scalars->setObjectStore(store);
  _xVectors->setObjectStore(store);
}


void EquationTab::hideCurveOptions() {
  _curvePlacement->setVisible(false);
  _curveAppearance->setVisible(false);
}


void EquationTab::clearTabValues() {
  _xVectors->clearSelection();
  _equation->clear();
  _doInterpolation->setCheckState(Qt::PartiallyChecked);
}


void EquationTab::updateVectorCombos() {
  _xVectors->fillVectors();
  _vectors->fillVectors();
  _scalars->fillScalars();
}

EquationDialog::EquationDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Equation"));
  else
    setWindowTitle(tr("New Equation"));

  _equationTab = new EquationTab(this);
  addDataTab(_equationTab);

  _equationTab->setEquation("");

  if (editMode() == Edit) {
    configureTab(dataObject);
  } else {
    configureTab(0);
  }

  connect(_equationTab, SIGNAL(optionsChanged()), this, SLOT(updateButtons()));
  connect(this, SIGNAL(editMultipleMode()), this, SLOT(editMultipleMode()));
  connect(this, SIGNAL(editSingleMode()), this, SLOT(editSingleMode()));

  connect(_equationTab, SIGNAL(modified()), this, SLOT(modified()));
  updateButtons();

}


EquationDialog::~EquationDialog() {
}


// QString EquationDialog::tagString() const {
//   return DataDialog::tagString();
// }


void EquationDialog::editMultipleMode() {
  _equationTab->clearTabValues();
}


void EquationDialog::editSingleMode() {
   configureTab(dataObject());
}


void EquationDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled((_equationTab->xVector() && !_equationTab->equation().isEmpty()) || (editMode() == EditMultiple));
}


void EquationDialog::configureTab(ObjectPtr object) {
  if (!object) {
    _equationTab->curveAppearance()->loadWidgetDefaults();
  } else if (EquationPtr equation = kst_cast<Equation>(object)) {
    _equationTab->setXVector(equation->vXIn());
    _equationTab->setEquation(equation->equation());
    _equationTab->setDoInterpolation(equation->doInterp());
    _equationTab->hideCurveOptions();
    if (_editMultipleWidget) {
      EquationList objects = _document->objectStore()->getObjects<Equation>();
      _editMultipleWidget->clearObjects();
      foreach(EquationPtr object, objects) {
        _editMultipleWidget->addObject(object->Name(), object->descriptionTip());
      }
    }
  }
}


ObjectPtr EquationDialog::createNewDataObject() {
  Q_ASSERT(_document && _document->objectStore());

  EquationPtr equation = _document->objectStore()->createObject<Equation>();
  Q_ASSERT(equation);

  equation->setEquation(_equationTab->equation());
  equation->setExistingXVector(_equationTab->xVector(), _equationTab->doInterpolation());
  if (DataDialog::tagStringAuto()) {
     equation->setDescriptiveName(QString());
  } else {
     equation->setDescriptiveName(DataDialog::tagString());
  }

  equation->writeLock();
  equation->update();
  equation->unlock();

  CurvePtr curve = _document->objectStore()->createObject<Curve>();
  Q_ASSERT(curve);

  curve->setXVector(equation->vX());
  curve->setYVector(equation->vY());
  curve->setColor(_equationTab->curveAppearance()->color());
  curve->setHasPoints(_equationTab->curveAppearance()->showPoints());
  curve->setHasLines(_equationTab->curveAppearance()->showLines());
  curve->setHasBars(_equationTab->curveAppearance()->showBars());
  curve->setLineWidth(_equationTab->curveAppearance()->lineWidth());
  curve->setLineStyle(_equationTab->curveAppearance()->lineStyle());
  curve->setPointType(_equationTab->curveAppearance()->pointType());
  curve->setPointDensity(_equationTab->curveAppearance()->pointDensity());
  curve->setBarStyle(_equationTab->curveAppearance()->barStyle());

  curve->writeLock();
  curve->update();
  curve->unlock();

  _equationTab->curveAppearance()->setWidgetDefaults();

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
      CreatePlotForCurve *cmd = new CreatePlotForCurve();
      cmd->createItem();

      plotItem = static_cast<PlotItem*>(cmd->item());
      break;
    }
  default:
    break;
  }

  if (_equationTab->curvePlacement()->place() != CurvePlacement::NoPlot) {
    PlotRenderItem *renderItem = plotItem->renderItem(PlotRenderItem::Cartesian);
    renderItem->addRelation(kst_cast<Relation>(curve));
    plotItem->update();

    plotItem->parentView()->appendToLayout(_equationTab->curvePlacement()->layout(), plotItem, _equationTab->curvePlacement()->gridColumns());
  }

  return ObjectPtr(equation.data());
}


ObjectPtr EquationDialog::editExistingDataObject() const {
  if (EquationPtr equation = kst_cast<Equation>(dataObject())) {
    if (editMode() == EditMultiple) {
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (QString objectName, objects) {
        EquationPtr equation = kst_cast<Equation>(_document->objectStore()->retrieveObject(objectName));
        if (equation) {
          VectorPtr xVector = _equationTab->xVectorDirty() ? _equationTab->xVector() : equation->vXIn();
          const QString equationString = _equationTab->equationDirty() ? _equationTab->equation() : equation->equation();
          const bool doInterpolation = _equationTab->doInterpolationDirty() ?  _equationTab->doInterpolation() : equation->doInterp();

          equation->writeLock();
          equation->setEquation(equationString);
          equation->setExistingXVector(xVector, doInterpolation);

          equation->inputObjectUpdated(equation);
          equation->unlock();
        }
      }
    } else {
      equation->writeLock();
      equation->setEquation(_equationTab->equation());
      equation->setExistingXVector(_equationTab->xVector(), _equationTab->doInterpolation());
      if (DataDialog::tagStringAuto()) {
         equation->setDescriptiveName(QString());
      } else {
         equation->setDescriptiveName(DataDialog::tagString());
      }
      equation->inputObjectUpdated(equation);
      equation->unlock();
    }
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
