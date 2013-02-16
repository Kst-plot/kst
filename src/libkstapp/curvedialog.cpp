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

#include "curvedialog.h"

#include "dialogpage.h"
#include "editmultiplewidget.h"

#include "curve.h"

#include "view.h"
#include "plotitem.h"
#include "tabwidget.h"
#include "mainwindow.h"
#include "application.h"
#include "plotrenderitem.h"

#include "datacollection.h"
#include "document.h"
#include "objectstore.h"
#include "updatemanager.h"

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

  _xVectorLabel->setBuddy(_xVector->_vector);
  _yVectorLabel->setBuddy(_yVector->_vector);
  _xErrorLabel->setBuddy(_xError->_vector);
  _yErrorLabel->setBuddy(_yError->_vector);
  _xMinusErrorLabel->setBuddy(_xMinusError->_vector);
  _yMinusErrorLabel->setBuddy(_yMinusError->_vector);

  _xVector->setIsX(true);

  connect(_xVector, SIGNAL(selectionChanged(QString)), this, SIGNAL(vectorsChanged()));
  connect(_yVector, SIGNAL(selectionChanged(QString)), this, SIGNAL(vectorsChanged()));
  connect(_xMinusSameAsPlus, SIGNAL(toggled(bool)), this, SLOT(xCheckboxClicked()));
  connect(_yMinusSameAsPlus, SIGNAL(toggled(bool)), this, SLOT(yCheckboxClicked()));

  connect(_xError, SIGNAL(selectionChanged(QString)), this, SLOT(xErrorChanged()));
  connect(_yError, SIGNAL(selectionChanged(QString)), this, SLOT(xErrorChanged()));

  connect(_xVector, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));
  connect(_yVector, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));
  connect(_xError, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));
  connect(_yError, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));
  connect(_xMinusError, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));
  connect(_yMinusError, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));

  // if the content of any of the vector combos is changed (new or edit), update them all.
  connect(_xVector, SIGNAL(contentChanged()), this, SLOT(updateVectorCombos()));
  connect(_yVector, SIGNAL(contentChanged()), this, SLOT(updateVectorCombos()));
  connect(_xError, SIGNAL(contentChanged()), this, SLOT(updateVectorCombos()));
  connect(_yError, SIGNAL(contentChanged()), this, SLOT(updateVectorCombos()));
  connect(_xMinusError, SIGNAL(contentChanged()), this, SLOT(updateVectorCombos()));
  connect(_yMinusError, SIGNAL(contentChanged()), this, SLOT(updateVectorCombos()));

  connect(_curveAppearance, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_ignoreAutoScale, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_xMinusSameAsPlus, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_yMinusSameAsPlus, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));

  _ignoreAutoScale->setProperty("si","I&gnore in automatic axes range calculations");
  _xVectorLabel->setProperty("si","&X-axis vector:");
  _yVectorLabel->setProperty("si","&Y-axis vector:");
  _xErrorLabel->setProperty("si","+X e&rror bar:");
  _yErrorLabel->setProperty("si","&+Y error bar:");
  _xMinusErrorLabel->setProperty("si","-X error bar:");
  _yMinusErrorLabel->setProperty("si","&-Y error bar:");
  _xMinusSameAsPlus->setProperty("si","&Use +X error bar for -X");
  _yMinusSameAsPlus->setProperty("si","U&se +Y error bar for -Y");
}


CurveTab::~CurveTab() {
}


VectorPtr CurveTab::xVector() const {
  return _xVector->selectedVector();
}


bool CurveTab::xVectorDirty() const {
  return _xVector->selectedVectorDirty();
}


void CurveTab::setXVector(VectorPtr vector) {
  _xVector->setSelectedVector(vector);
}


bool CurveTab::xVectorSelected() const {
  return _xVector->vectorSelected();
}


VectorPtr CurveTab::yVector() const {
  return _yVector->selectedVector();
}


bool CurveTab::yVectorDirty() const {
  return _yVector->selectedVectorDirty();
}


bool CurveTab::yVectorSelected() const {
  return _yVector->vectorSelected();
}


void CurveTab::setYVector(VectorPtr vector) {
  _yVector->setSelectedVector(vector);
}


VectorPtr CurveTab::xError() const {
  return _xError->selectedVector();
}


bool CurveTab::xErrorDirty() const {
  return _xError->selectedVectorDirty();
}


void CurveTab::setXError(VectorPtr vector) {
  _xError->setSelectedVector(vector);
}


VectorPtr CurveTab::yError() const {
  return _yError->selectedVector();
}


bool CurveTab::yErrorDirty() const {
  return _yError->selectedVectorDirty();
}


void CurveTab::setYError(VectorPtr vector) {
  _yError->setSelectedVector(vector);
}


VectorPtr CurveTab::xMinusError() const {
  return _xMinusError->selectedVector();
}


bool CurveTab::xMinusErrorDirty() const {
  return _xMinusError->selectedVectorDirty();
}


void CurveTab::setXMinusError(VectorPtr vector) {
  _xMinusError->setSelectedVector(vector);
}


VectorPtr CurveTab::yMinusError() const {
  return _yMinusError->selectedVector();
}


bool CurveTab::yMinusErrorDirty() const {
  return _yMinusError->selectedVectorDirty();
}


void CurveTab::setYMinusError(VectorPtr vector) {
  _yMinusError->setSelectedVector(vector);
}


void CurveTab::xCheckboxClicked() {
  _xMinusError->setEnabled(!_xMinusSameAsPlus->isChecked());
  _xMinusErrorLabel->setEnabled(!_xMinusSameAsPlus->isChecked());
  xErrorChanged();
}


void CurveTab::yCheckboxClicked() {
  _yMinusError->setEnabled(!_yMinusSameAsPlus->isChecked());
  _yMinusErrorLabel->setEnabled(!_yMinusSameAsPlus->isChecked());
  yErrorChanged();
}


void CurveTab::xErrorChanged() {
  if (_xMinusSameAsPlus->isChecked()) {
    _xMinusError->setSelectedVector(_xError->selectedVector());
  }
}


void CurveTab::yErrorChanged() {
  if (_yMinusSameAsPlus->isChecked()) {
    _yMinusError->setSelectedVector(_yError->selectedVector());
  }
}


void CurveTab::setObjectStore(ObjectStore *store) {
  _xVector->setObjectStore(store);
  _yVector->setObjectStore(store);
  _xError->setObjectStore(store);
  _yError->setObjectStore(store);
  _xMinusError->setObjectStore(store);
  _yMinusError->setObjectStore(store);
}


void CurveTab::hidePlacementOptions() {
  _curvePlacement->setVisible(false);
}



CurveAppearance* CurveTab::curveAppearance() const {
  return _curveAppearance;
}


CurvePlacement* CurveTab::curvePlacement() const {
  return _curvePlacement;
}


bool CurveTab::ignoreAutoScale() const {
  return _ignoreAutoScale->isChecked();
}


bool CurveTab::ignoreAutoScaleDirty() const {
  return _ignoreAutoScale->checkState() != Qt::PartiallyChecked;
}


void CurveTab::setIgnoreAutoScale(bool ignoreAutoScale) {
  _ignoreAutoScale->setChecked(ignoreAutoScale);
}


void CurveTab::clearTabValues() {
  _xVector->clearSelection();
  _yVector->clearSelection();
  _xError->clearSelection();
  _yError->clearSelection();
  _xMinusError->clearSelection();
  _yMinusError->clearSelection();
  _ignoreAutoScale->setCheckState(Qt::PartiallyChecked);
  _curveAppearance->clearValues();
}

void CurveTab::updateVectorCombos() {
  _xVector->fillVectors();
  _yVector->fillVectors();
  _xError->fillVectors();
  _yError->fillVectors();
  _xMinusError->fillVectors();
  _yMinusError->fillVectors();
}


CurveDialog::CurveDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Curve"));
  else
    setWindowTitle(tr("New Curve"));

  _curveTab = new CurveTab(this);
  addDataTab(_curveTab);

  if (editMode() == Edit) {
    configureTab(dataObject);
  } else {
    configureTab(0);
  }

  connect(_curveTab, SIGNAL(vectorsChanged()), this, SLOT(updateButtons()));
  connect(this, SIGNAL(editMultipleMode()), this, SLOT(editMultipleMode()));
  connect(this, SIGNAL(editSingleMode()), this, SLOT(editSingleMode()));
  connect(_curveTab, SIGNAL(modified()), this, SLOT(modified()));
  updateButtons();
}


CurveDialog::~CurveDialog() {
}


// QString CurveDialog::tagString() const {
//   return DataDialog::tagString();
// }
// 

void CurveDialog::editMultipleMode() {
  _curveTab->clearTabValues();
}

void CurveDialog::editSingleMode() {
   configureTab(dataObject());
}


void CurveDialog::configureTab(ObjectPtr object) {
  if (!object) {
    _curveTab->curveAppearance()->loadWidgetDefaults();
    _curveTab->setToLastX();
  } else if (CurvePtr curve = kst_cast<Curve>(object)) {
    _curveTab->curveAppearance()->loadWidgetDefaults();
    _curveTab->setXVector(curve->xVector());
    _curveTab->setYVector(curve->yVector());
    if (curve->hasXError()) {
      _curveTab->setXError(curve->xErrorVector());
    }
    if (curve->hasYError()) {
    _curveTab->setYError(curve->yErrorVector());
    }
    if (curve->hasXMinusError()) {
    _curveTab->setXMinusError(curve->xMinusErrorVector());
    }
    if (curve->hasYMinusError()) {
      _curveTab->setYMinusError(curve->yMinusErrorVector());
    }
    _curveTab->setIgnoreAutoScale(curve->ignoreAutoScale());
    _curveTab->curveAppearance()->setColor(curve->color());
    _curveTab->curveAppearance()->setHeadColor(curve->headColor());
    _curveTab->curveAppearance()->setShowPoints(curve->hasPoints());
    _curveTab->curveAppearance()->setShowLines(curve->hasLines());
    _curveTab->curveAppearance()->setShowBars(curve->hasBars());
    _curveTab->curveAppearance()->setShowHead(curve->hasHead());
    _curveTab->curveAppearance()->setLineWidth(curve->lineWidth());
    _curveTab->curveAppearance()->setPointSize(curve->pointSize());
    _curveTab->curveAppearance()->setLineStyle(curve->lineStyle());
    _curveTab->curveAppearance()->setPointType(curve->pointType());
    _curveTab->curveAppearance()->setPointDensity(curve->pointDensity());
    _curveTab->curveAppearance()->setBarFillColor(curve->barFillColor());
    _curveTab->curveAppearance()->setHeadType(curve->headType());
    _curveTab->hidePlacementOptions();
    if (_editMultipleWidget) {
      CurveList objects = _document->objectStore()->getObjects<Curve>();
      _editMultipleWidget->clearObjects();
      foreach(CurvePtr object, objects) {
        _editMultipleWidget->addObject(object->Name(), object->descriptionTip());
      }
    }
  }
}


void CurveDialog::setVector(VectorPtr vector) {
  _curveTab->setYVector(vector);
}


void CurveDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(dialogValid());
}


bool CurveDialog::dialogValid() const {
  bool valid = (_curveTab->xVectorSelected() && _curveTab->yVectorSelected()) || (editMode() == EditMultiple);
  return (valid);
}


ObjectPtr CurveDialog::createNewDataObject() {
  Q_ASSERT(_document && _document->objectStore());

  ObjectStore *os = _document->objectStore();
  CurvePtr curve = os->createObject<Curve>();

  curve->setXVector(_curveTab->xVector());
  curve->setYVector(_curveTab->yVector());
  curve->setXError(_curveTab->xError());
  curve->setYError(_curveTab->yError());
  curve->setXMinusError(_curveTab->xMinusError());
  curve->setYMinusError(_curveTab->yMinusError());
  curve->setColor(_curveTab->curveAppearance()->color());
  curve->setHeadColor(_curveTab->curveAppearance()->headColor());
  curve->setHasPoints(_curveTab->curveAppearance()->showPoints());
  curve->setHasLines(_curveTab->curveAppearance()->showLines());
  curve->setHasBars(_curveTab->curveAppearance()->showBars());
  curve->setHasHead(_curveTab->curveAppearance()->showHead());
  curve->setLineWidth(_curveTab->curveAppearance()->lineWidth());
  curve->setPointSize(_curveTab->curveAppearance()->pointSize());
  curve->setLineStyle(_curveTab->curveAppearance()->lineStyle());
  curve->setPointType(_curveTab->curveAppearance()->pointType());
  curve->setPointDensity(_curveTab->curveAppearance()->pointDensity());
  curve->setBarFillColor(_curveTab->curveAppearance()->barFillColor());
  curve->setHeadType(_curveTab->curveAppearance()->headType());
  curve->setIgnoreAutoScale(_curveTab->ignoreAutoScale());

  if (DataDialog::tagStringAuto()) {
     curve->setDescriptiveName(QString());
  } else {
     curve->setDescriptiveName(DataDialog::tagString());
  }

  curve->writeLock();
  curve->registerChange();
  curve->unlock();

  _curveTab->curveAppearance()->setWidgetDefaults();

  if(editMode()==New) {
      PlotItem *plotItem = 0;
      switch (_curveTab->curvePlacement()->place()) {
      case CurvePlacement::NoPlot:
          break;
      case CurvePlacement::ExistingPlot:
      {
          plotItem = static_cast<PlotItem*>(_curveTab->curvePlacement()->existingPlot());
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
          if (_curveTab->curvePlacement()->scaleFonts()) {
              plotItem->view()->resetPlotFontSizes(1);
              plotItem->view()->configurePlotFontDefaults(plotItem); // copy plots already in window
          }
          break;
      }
      default:
          break;
      }

      if (_curveTab->curvePlacement()->place() != CurvePlacement::NoPlot) {
          PlotRenderItem *renderItem = plotItem->renderItem(PlotRenderItem::Cartesian);
          renderItem->addRelation(kst_cast<Relation>(curve));
          plotItem->update();

          if (_curveTab->curvePlacement()->place() != CurvePlacement::ExistingPlot) {
              plotItem->view()->appendToLayout(_curveTab->curvePlacement()->layout(), plotItem, _curveTab->curvePlacement()->gridColumns());
              if (_curveTab->curvePlacement()->layout() == CurvePlacement::Custom) {
                plotItem->createCustomLayout(_curveTab->curvePlacement()->gridColumns());
              }
          }
      }
  }

  UpdateManager::self()->doUpdates(true);

  return ObjectPtr(curve.data());
}


ObjectPtr CurveDialog::editExistingDataObject() const {
  if (CurvePtr curve = kst_cast<Curve>(dataObject())) {
    if (editMode() == EditMultiple) {
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (const QString &objectName, objects) {
        CurvePtr curve = kst_cast<Curve>(_document->objectStore()->retrieveObject(objectName));
        if (curve) {
          VectorPtr xVector = _curveTab->xVectorDirty() ? _curveTab->xVector() : curve->xVector();
          VectorPtr yVector = _curveTab->yVectorDirty() ? _curveTab->yVector() : curve->yVector();

          VectorPtr xError = 0;
          if (_curveTab->xErrorDirty()) {
            xError = _curveTab->xError();
          } else if (curve->hasXError()) {
            xError = curve->xErrorVector();
          }
          VectorPtr yError = 0;
          if (_curveTab->yErrorDirty()) {
            yError = _curveTab->yError();
          } else if (curve->hasYError()) {
            yError = curve->yErrorVector();
          }
          VectorPtr xMinusError = 0;
          if (_curveTab->xMinusErrorDirty()) {
            xMinusError = _curveTab->xMinusError();
          } else if (curve->hasXMinusError()) {
            xMinusError = curve->xMinusErrorVector();
          }
          VectorPtr yMinusError = 0;
          if (_curveTab->yMinusErrorDirty()) {
            yMinusError = _curveTab->yMinusError();
          } else if (curve->hasYMinusError()) {
            yMinusError = curve->yMinusErrorVector();
          }

          QColor color = _curveTab->curveAppearance()->colorDirty() ? _curveTab->curveAppearance()->color() : curve->color();
          QColor headColor = _curveTab->curveAppearance()->headColorDirty() ? _curveTab->curveAppearance()->headColor() : curve->headColor();
          QColor barFillColor = _curveTab->curveAppearance()->barFillColorDirty() ? _curveTab->curveAppearance()->barFillColor() : curve->barFillColor();

          int lineWidth = _curveTab->curveAppearance()->lineWidthDirty() ?  _curveTab->curveAppearance()->lineWidth() : curve->lineWidth();
          double pointSize = _curveTab->curveAppearance()->pointSizeDirty() ?  _curveTab->curveAppearance()->pointSize() : curve->pointSize();
          int lineStyle = _curveTab->curveAppearance()->lineStyleDirty() ?  _curveTab->curveAppearance()->lineStyle() : curve->lineStyle();
          int pointType = _curveTab->curveAppearance()->pointTypeDirty() ?  _curveTab->curveAppearance()->pointType() : curve->pointType();
          int pointDensity = _curveTab->curveAppearance()->pointDensityDirty() ?  _curveTab->curveAppearance()->pointDensity() : curve->pointDensity();
          int headType = _curveTab->curveAppearance()->headTypeDirty() ?  _curveTab->curveAppearance()->headType() : curve->headType();

          bool showPoints = _curveTab->curveAppearance()->showPointsDirty() ?  _curveTab->curveAppearance()->showPoints() : curve->hasPoints();
          bool showLines = _curveTab->curveAppearance()->showLinesDirty() ?  _curveTab->curveAppearance()->showLines() : curve->hasLines();
          bool showBars = _curveTab->curveAppearance()->showBarsDirty() ?  _curveTab->curveAppearance()->showBars() : curve->hasBars();
          bool showHead = _curveTab->curveAppearance()->showHeadDirty() ?  _curveTab->curveAppearance()->showHead() : curve->hasHead();
          bool ignoreAutoScale = _curveTab->ignoreAutoScaleDirty() ?  _curveTab->ignoreAutoScale() : curve->ignoreAutoScale();

          curve->writeLock();
          curve->setXVector(xVector);
          curve->setYVector(yVector);
          curve->setXError(xError);
          curve->setYError(yError);
          curve->setXMinusError(xMinusError);
          curve->setYMinusError(yMinusError);
          curve->setColor(color);
          curve->setHeadColor(headColor);
          curve->setBarFillColor(barFillColor);
          curve->setHasPoints(showPoints);
          curve->setHasLines(showLines);
          curve->setHasBars(showBars);
          curve->setHasHead(showHead);
          curve->setLineWidth(lineWidth);
          curve->setPointSize(pointSize);
          curve->setLineStyle(lineStyle);
          curve->setPointType(pointType);
          curve->setPointDensity(pointDensity);
          curve->setHeadType(headType);
          curve->setIgnoreAutoScale(ignoreAutoScale);
          if (DataDialog::tagStringAuto()) {
             curve->setDescriptiveName(QString());
          } else {
             curve->setDescriptiveName(DataDialog::tagString());
          }

          curve->registerChange();
          curve->unlock();
        }
      }
    } else {
      curve->writeLock();
      curve->setXVector(_curveTab->xVector());
      curve->setYVector(_curveTab->yVector());
      curve->setXError(_curveTab->xError());
      curve->setYError(_curveTab->yError());
      curve->setXMinusError(_curveTab->xMinusError());
      curve->setYMinusError(_curveTab->yMinusError());
      curve->setColor(_curveTab->curveAppearance()->color());
      curve->setHeadColor(_curveTab->curveAppearance()->headColor());
      curve->setBarFillColor(_curveTab->curveAppearance()->barFillColor());
      curve->setHasPoints(_curveTab->curveAppearance()->showPoints());
      curve->setHasLines(_curveTab->curveAppearance()->showLines());
      curve->setHasBars(_curveTab->curveAppearance()->showBars());
      curve->setHasHead(_curveTab->curveAppearance()->showHead());
      curve->setLineWidth(_curveTab->curveAppearance()->lineWidth());
      curve->setPointSize(_curveTab->curveAppearance()->pointSize());
      curve->setLineStyle(_curveTab->curveAppearance()->lineStyle());
      curve->setPointType(_curveTab->curveAppearance()->pointType());
      curve->setPointDensity(_curveTab->curveAppearance()->pointDensity());
      curve->setHeadType(_curveTab->curveAppearance()->headType());
      curve->setIgnoreAutoScale(_curveTab->ignoreAutoScale());
      if (DataDialog::tagStringAuto()) {
         curve->setDescriptiveName(QString());
      } else {
         curve->setDescriptiveName(DataDialog::tagString());
      }

      curve->registerChange();
      curve->unlock();

      _curveTab->curveAppearance()->setWidgetDefaults(false);
    }
  }

  UpdateManager::self()->doUpdates(true);
  return dataObject();
}

}

// vim: ts=2 sw=2 et
