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
#include "updatemanager.h"

#include <QPushButton>

namespace Kst {

EquationTab::EquationTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);

  populateFunctionList();

  setTabTitle(tr("Equation"));

  _curvePlacement->setExistingPlots(Data::self()->plotList());

  _xVectors->setIsX(true);

  TextLabel1_11->setBuddy(_equation);
  _xVectorLabel->setBuddy(_xVectors->_vector);
  connect(_xVectors, SIGNAL(selectionChanged(QString)), this, SLOT(selectionChanged()));
  connect(_equation, SIGNAL(textChanged(const QString &)), this, SLOT(selectionChanged()));

  connect(_xVectors, SIGNAL(contentChanged()), this, SLOT(updateVectorCombos()));

  connect(_xVectors, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));
  connect(_equation, SIGNAL(textChanged(const QString &)), this, SIGNAL(modified()));
  connect(_doInterpolation, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_curvePlacement->_noPlot, SIGNAL(toggled(bool)), _curveAppearance, SLOT(setDisabled(bool)));


  TextLabel1_11->setProperty("si","Eq&uation:");
  _xVectorLabel->setProperty("si","&X vector:");
  _doInterpolation->setProperty("si","Inte&rpolate to highest resolution vector");
}


EquationTab::~EquationTab() {
}


void EquationTab::selectionChanged() {
  emit optionsChanged();
}


void EquationTab::equationUpdate(const QString& string) {
  QString cleanString = string;
  cleanString.remove('[').remove(']'); // HACK: '[' in descriptive names mess up parser.  Remove them.
  _equation->insert('[' + cleanString + ']');
}


void EquationTab::equationOperatorUpdate(const QString& string) {
  QString equation = _equation->text();
  equation += string;
  _equation->setText(equation); 
}


void EquationTab::populateFunctionList() {
  QList<QString> Operators;
  Operators.clear();
  Operators.push_back("+");
  Operators.push_back("-");
  Operators.push_back("*");
  Operators.push_back("/");
  Operators.push_back("%");
  Operators.push_back("^");
  Operators.push_back("&");
  Operators.push_back("|");
  Operators.push_back("&&");
  Operators.push_back("||");
  Operators.push_back("!");
  Operators.push_back("<");
  Operators.push_back("<=");
  Operators.push_back("==");
  Operators.push_back(">=");
  Operators.push_back(">");
  Operators.push_back("!=");
  Operators.push_back("STEP()");
  Operators.push_back("ABS()");
  Operators.push_back("SQRT()");
  Operators.push_back("CBRT()");
  Operators.push_back("SIN()");
  Operators.push_back("COS()");
  Operators.push_back("TAN()");
  Operators.push_back("ASIN()");
  Operators.push_back("ACOS()");
  Operators.push_back("ATAN()");
  Operators.push_back("SEC()");
  Operators.push_back("CSC()");
  Operators.push_back("COT()");
  Operators.push_back("SINH()");
  Operators.push_back("COSH()");
  Operators.push_back("TANH()");
  Operators.push_back("EXP()");
  Operators.push_back("LN()");
  Operators.push_back("LOG()");
  Operators.push_back("PLUGIN()");

  QList<CompletionCase> data;
  data.push_back(CompletionCase(""));
  data.back().push_back(Category("Operators"));
  data.push_back(CompletionCase(""));
  data.back().push_back(Category("Functions"));
  for(int i=0;i<Operators.count();i++) {
      data[Operators.at(i).contains("()")?1:0][0]<<Operators.at(i);
  }
  for(int i=0;i<data[1][0].size();i++) {
      data[1][0][i].chop(1);
  }
  data.push_back(CompletionCase("\\["));    //block escaped brackets
  _equation->init(data);

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
  _equation->setObjectStore(store);
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
  _equation->fillKstObjects();
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
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(dialogValid());
}


bool EquationDialog::dialogValid() const {
  bool valid = (_equationTab->xVectorSelected()) || (editMode() == EditMultiple);
  return (valid);
}


void EquationDialog::configureTab(ObjectPtr object) {
  if (!object) {
    _equationTab->curveAppearance()->loadWidgetDefaults();
    _equationTab->setToLastX();
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
  equation->registerChange();
  equation->unlock();

  CurvePtr curve = _document->objectStore()->createObject<Curve>();
  Q_ASSERT(curve);

  curve->setXVector(equation->vX());
  curve->setYVector(equation->vY());
  curve->setColor(_equationTab->curveAppearance()->color());
  curve->setHeadColor(_equationTab->curveAppearance()->headColor());
  curve->setHasPoints(_equationTab->curveAppearance()->showPoints());
  curve->setHasLines(_equationTab->curveAppearance()->showLines());
  curve->setHasBars(_equationTab->curveAppearance()->showBars());
  curve->setHasHead(_equationTab->curveAppearance()->showHead());
  curve->setLineWidth(_equationTab->curveAppearance()->lineWidth());
  curve->setPointSize(_equationTab->curveAppearance()->pointSize());
  curve->setLineStyle(_equationTab->curveAppearance()->lineStyle());
  curve->setPointType(_equationTab->curveAppearance()->pointType());
  curve->setHeadType(_equationTab->curveAppearance()->headType());
  curve->setPointDensity(_equationTab->curveAppearance()->pointDensity());
  //curve->setBarStyle(_equationTab->curveAppearance()->barStyle());

  curve->writeLock();
  curve->registerChange();
  curve->unlock();

  _equationTab->curveAppearance()->setWidgetDefaults();

  if(editMode()==New) {
      PlotItem *plotItem = 0;
      switch (_equationTab->curvePlacement()->place()) {
      case CurvePlacement::NoPlot:
          break;
      case CurvePlacement::ExistingPlot:
      {
          plotItem = static_cast<PlotItem*>(_equationTab->curvePlacement()->existingPlot());
          break;
      }
      case CurvePlacement::NewPlotNewTab:
          _document->createView();
          // fall through to case NewPlot.
      case CurvePlacement::NewPlot:
      {
          CreatePlotForCurve *cmd = new CreatePlotForCurve();
          cmd->createItem();

          plotItem = static_cast<PlotItem*>(cmd->item());
          if (_equationTab->curvePlacement()->scaleFonts()) {
              plotItem->view()->resetPlotFontSizes(plotItem);
              plotItem->view()->configurePlotFontDefaults(plotItem); // copy plots already in window
          }
          break;
      }
      default:
          break;
      }

      if (_equationTab->curvePlacement()->place() != CurvePlacement::NoPlot) {
          PlotRenderItem *renderItem = plotItem->renderItem(PlotRenderItem::Cartesian);
          renderItem->addRelation(kst_cast<Relation>(curve));
          plotItem->update();

          if (_equationTab->curvePlacement()->place() != CurvePlacement::ExistingPlot) {
              plotItem->view()->appendToLayout(_equationTab->curvePlacement()->layout(), plotItem, _equationTab->curvePlacement()->gridColumns());
              if (_equationTab->curvePlacement()->layout() == CurvePlacement::Custom) {
                plotItem->createCustomLayout(_equationTab->curvePlacement()->gridColumns());
              }
          }
      }
  }

  UpdateManager::self()->doUpdates(true);

  return ObjectPtr(equation.data());
}


ObjectPtr EquationDialog::editExistingDataObject() const {
  if (EquationPtr equation = kst_cast<Equation>(dataObject())) {
    if (editMode() == EditMultiple) {
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (const QString &objectName, objects) {
        EquationPtr equation = kst_cast<Equation>(_document->objectStore()->retrieveObject(objectName));
        if (equation) {
          VectorPtr xVector = _equationTab->xVectorDirty() ? _equationTab->xVector() : equation->vXIn();
          const QString equationString = _equationTab->equationDirty() ? _equationTab->equation() : equation->equation();
          const bool doInterpolation = _equationTab->doInterpolationDirty() ?  _equationTab->doInterpolation() : equation->doInterp();

          equation->writeLock();
          equation->setEquation(equationString);
          equation->setExistingXVector(xVector, doInterpolation);

          equation->registerChange();
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
      equation->registerChange();
      equation->unlock();
    }
  }
  UpdateManager::self()->doUpdates(true);

  return dataObject();
}

}

// vim: ts=2 sw=2 et
