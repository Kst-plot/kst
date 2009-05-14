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

#include "histogramdialog.h"

#include "dialogpage.h"
#include "editmultiplewidget.h"

#include "histogram.h"

#include "view.h"
#include "plotitem.h"
#include "tabwidget.h"
#include "mainwindow.h"
#include "application.h"
#include "plotrenderitem.h"
#include "curve.h"
#include "document.h"
#include "objectstore.h"

#include "datacollection.h"
#include "dialogdefaults.h"

namespace Kst {

HistogramTab::HistogramTab(QWidget *parent)
  : DataTab(parent), _normalizationDirty(false) {

  setupUi(this);
  setTabTitle(tr("Histogram"));

  connect(AutoBin, SIGNAL(clicked()), this, SLOT(generateAutoBin()));
  connect(_realTimeAutoBin, SIGNAL(clicked()), this, SLOT(updateButtons()));
  connect(_vector, SIGNAL(selectionChanged(QString)), this, SLOT(selectionChanged()));

  connect(_vector, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));
  connect(_min, SIGNAL(textChanged(const QString &)), this, SIGNAL(modified()));
  connect(_max, SIGNAL(textChanged(const QString &)), this, SIGNAL(modified()));
  connect(_numberOfBins, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_realTimeAutoBin, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_normalizationIsNumber, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_normalizationIsFraction, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_normalizationIsPercent, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_normalizationMaximumOne, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_normalizationIsNumber, SIGNAL(clicked()), this, SLOT(normalizationChanged()));
  connect(_normalizationIsFraction, SIGNAL(clicked()), this, SLOT(normalizationChanged()));
  connect(_normalizationIsPercent, SIGNAL(clicked()), this, SLOT(normalizationChanged()));
  connect(_normalizationMaximumOne, SIGNAL(clicked()), this, SLOT(normalizationChanged()));

  _vectorLabel->setBuddy(_vector->_vector);

  _curvePlacement->setExistingPlots(Data::self()->plotList());
  _curveAppearance->setShowLines(false);
  _curveAppearance->setShowPoints(false);
  _curveAppearance->setShowBars(true);
  _curveAppearance->setColor(_curveAppearance->color());
  _curveAppearance->setPointType(0);
  _curveAppearance->setLineWidth(0);
  _curveAppearance->setLineStyle(0);
  _curveAppearance->setBarStyle(1);
  _curveAppearance->setPointDensity(0);
}


HistogramTab::~HistogramTab() {
}


void HistogramTab::normalizationChanged() {
  _normalizationDirty = true;
}


void HistogramTab::resetNormalizationDirty() {
  _normalizationDirty = false;
}


void HistogramTab::selectionChanged() {
  emit vectorChanged();
}


void HistogramTab::generateAutoBin() {

  VectorPtr selectedVector = vector();

  if (selectedVector) {
    selectedVector->readLock(); // Hmm should we really lock here?  AutoBin should I think
    int n;
    double max, min;
    Histogram::AutoBin(selectedVector, &n, &max, &min);
    selectedVector->unlock();

    _numberOfBins->setValue(n);
    _min->setText(QString::number(min));
    _max->setText(QString::number(max));
  }
}


void HistogramTab::updateButtons() {
  if (_realTimeAutoBin->isChecked()) {
    generateAutoBin();
  }

  _min->setEnabled(!_realTimeAutoBin->isChecked());
  _max->setEnabled(!_realTimeAutoBin->isChecked());
  _numberOfBins->setEnabled(!_realTimeAutoBin->isChecked());
  AutoBin->setEnabled(!_realTimeAutoBin->isChecked());
}


VectorPtr HistogramTab::vector() const {
  return _vector->selectedVector();
}


bool HistogramTab::vectorDirty() const {
  return _vector->selectedVectorDirty();
}


void HistogramTab::setVector(const VectorPtr vector) {
  _vector->setSelectedVector(vector);
}


double HistogramTab::min() const {
  return _min->text().toDouble();
}


bool HistogramTab::minDirty() const {
  return !_min->text().isEmpty();
}


void HistogramTab::setMin(const double min) {
  _min->setText(QString::number(min));
}


double HistogramTab::max() const {
  return _max->text().toDouble();
}


bool HistogramTab::maxDirty() const {
  return !_max->text().isEmpty();
}


void HistogramTab::setMax(const double max) {
  _max->setText(QString::number(max));
}


int HistogramTab::bins() const {
  return _numberOfBins->text().toInt();
}


bool HistogramTab::binsDirty() const {
  return !_numberOfBins->text().isEmpty();
}


void HistogramTab::setBins(const int bins) {
  _numberOfBins->setValue(bins);
}


bool HistogramTab::realTimeAutoBin() const {
  return _realTimeAutoBin->isChecked();
}


bool HistogramTab::realTimeAutoBinDirty() const {
  return _realTimeAutoBin->checkState() != Qt::PartiallyChecked;
}


void HistogramTab::setRealTimeAutoBin(const bool autoBin) {
  _realTimeAutoBin->setChecked(autoBin);
}


Histogram::NormalizationType HistogramTab::normalizationType() const {
  Histogram::NormalizationType normalization = Histogram::Number;

  if (_normalizationIsFraction->isChecked()) {
    normalization = Histogram::Fraction;
  } else if (_normalizationIsPercent->isChecked()) {
    normalization = Histogram::Percent;
  } else if (_normalizationMaximumOne->isChecked()) {
    normalization = Histogram::MaximumOne;
  }
  return normalization;
}


bool HistogramTab::normalizationTypeDirty() const {
  return _normalizationDirty;
}


void HistogramTab::setNormalizationType(const Histogram::NormalizationType normalizationType) {
  switch (normalizationType) {
    case Histogram::Fraction:
      _normalizationIsFraction->setChecked(true);
      break;
    case Histogram::MaximumOne:
      _normalizationMaximumOne->setChecked(true);
      break;
    case Histogram::Number:
      _normalizationIsNumber->setChecked(true);
      break;
    case Histogram::Percent:
      _normalizationIsPercent->setChecked(true);
      break;
  }
  resetNormalizationDirty();
}



CurveAppearance* HistogramTab::curveAppearance() const {
  return _curveAppearance;
}


CurvePlacement* HistogramTab::curvePlacement() const {
  return _curvePlacement;
}


void HistogramTab::setObjectStore(ObjectStore *store) {
  _vector->setObjectStore(store);
}


void HistogramTab::hideCurveOptions() {
  _curvePlacement->setVisible(false);
  _curveAppearance->setVisible(false);
}


void HistogramTab::clearTabValues() {
  _vector->clearSelection();
  _min->clear();
  _max->clear();
  _numberOfBins->clear();
  _realTimeAutoBin->setCheckState(Qt::PartiallyChecked);
  _normalizationIsNumber->setChecked(true);
  resetNormalizationDirty();
}


HistogramDialog::HistogramDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Histogram"));
  else
    setWindowTitle(tr("New Histogram"));

  _histogramTab = new HistogramTab(this);
  addDataTab(_histogramTab);

  if (editMode() == Edit) {
    configureTab(dataObject);
  } else {
    configureTab(0);
  }

  connect(_histogramTab, SIGNAL(vectorChanged()), this, SLOT(updateButtons()));
  connect(this, SIGNAL(editMultipleMode()), this, SLOT(editMultipleMode()));
  connect(this, SIGNAL(editSingleMode()), this, SLOT(editSingleMode()));

  connect(_histogramTab, SIGNAL(modified()), this, SLOT(modified()));
  updateButtons();
}


HistogramDialog::~HistogramDialog() {
}


// QString HistogramDialog::tagString() const {
//   return DataDialog::tagString();
// }


void HistogramDialog::editMultipleMode() {
  _histogramTab->clearTabValues();
}


void HistogramDialog::editSingleMode() {
   configureTab(dataObject());
}


void HistogramDialog::configureTab(ObjectPtr object) {
  if (!object) {
    _histogramTab->setRealTimeAutoBin(_dialogDefaults->value("histogram/realTimeAutoBin", false).toBool());
    _histogramTab->setNormalizationType(Histogram::NormalizationType(_dialogDefaults->value("histogram/normalizationType",Histogram::Number).toInt()));
  } else if (HistogramPtr histogram = kst_cast<Histogram>(object)) {
    _histogramTab->setVector(histogram->vector());
    _histogramTab->setMin(histogram->xMin());
    _histogramTab->setMax(histogram->xMax());
    _histogramTab->setBins(histogram->numberOfBins());
    _histogramTab->setRealTimeAutoBin(histogram->realTimeAutoBin());
    _histogramTab->setNormalizationType(histogram->normalizationType());
    _histogramTab->hideCurveOptions();
    if (_editMultipleWidget) {
      HistogramList objects = _document->objectStore()->getObjects<Histogram>();
      _editMultipleWidget->clearObjects();
      foreach(HistogramPtr object, objects) {
        _editMultipleWidget->addObject(object->Name(), object->descriptionTip());
      }
    }
  }
}


void HistogramDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_histogramTab->vector() || (editMode() == EditMultiple));
}


void HistogramDialog::setVector(VectorPtr vector) {
  _histogramTab->setVector(vector);
}


ObjectPtr HistogramDialog::createNewDataObject() {
  Q_ASSERT(_document && _document->objectStore());

  HistogramPtr histogram = _document->objectStore()->createObject<Histogram>();

  histogram->change(_histogramTab->vector(), _histogramTab->min(), _histogramTab->max(),
                  _histogramTab->bins(), _histogramTab->normalizationType(),
                  _histogramTab->realTimeAutoBin());

  if (DataDialog::tagStringAuto()) {
     histogram->setDescriptiveName(QString());
  } else {
     histogram->setDescriptiveName(DataDialog::tagString());
  }

  histogram->writeLock();
  histogram->update();
  histogram->unlock();

  setHistogramDefaults(histogram);

  CurvePtr curve = _document->objectStore()->createObject<Curve>();

  curve->setXVector(histogram->vX());
  curve->setYVector(histogram->vY());
  curve->setColor(_histogramTab->curveAppearance()->color());
  curve->setHasPoints(_histogramTab->curveAppearance()->showPoints());
  curve->setHasLines(_histogramTab->curveAppearance()->showLines());
  curve->setHasBars(_histogramTab->curveAppearance()->showBars());
  curve->setLineWidth(_histogramTab->curveAppearance()->lineWidth());
  curve->setLineStyle(_histogramTab->curveAppearance()->lineStyle());
  curve->setPointType(_histogramTab->curveAppearance()->pointType());
  curve->setPointDensity(_histogramTab->curveAppearance()->pointDensity());
  curve->setBarStyle(_histogramTab->curveAppearance()->barStyle());

  curve->writeLock();
  curve->update();
  curve->unlock();

  PlotItem *plotItem = 0;
  switch (_histogramTab->curvePlacement()->place()) {
  case CurvePlacement::NoPlot:
    break;
  case CurvePlacement::ExistingPlot:
    {
      plotItem = static_cast<PlotItem*>(_histogramTab->curvePlacement()->existingPlot());
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

  if (_histogramTab->curvePlacement()->place() != CurvePlacement::NoPlot) {
    PlotRenderItem *renderItem = plotItem->renderItem(PlotRenderItem::Cartesian);
    renderItem->addRelation(kst_cast<Relation>(curve));
    plotItem->update();

    plotItem->parentView()->appendToLayout(_histogramTab->curvePlacement()->layout(), plotItem, _histogramTab->curvePlacement()->gridColumns());
  }
  return ObjectPtr(histogram.data());
}


ObjectPtr HistogramDialog::editExistingDataObject() const {
  if (HistogramPtr histogram = kst_cast<Histogram>(dataObject())) {
    if (editMode() == EditMultiple) {
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (QString objectName, objects) {
        HistogramPtr histogram = kst_cast<Histogram>(_document->objectStore()->retrieveObject(objectName));
        if (histogram) {
          VectorPtr vector = _histogramTab->vectorDirty() ? _histogramTab->vector() : histogram->vector();
          const double min = _histogramTab->minDirty() ? _histogramTab->min() : histogram->xMin();
          const double max = _histogramTab->maxDirty() ? _histogramTab->max() : histogram->xMax();
          const int bins = _histogramTab->binsDirty() ? _histogramTab->bins() : histogram->numberOfBins();
          Histogram::NormalizationType normalizationType = _histogramTab->normalizationTypeDirty() ? _histogramTab->normalizationType() : histogram->normalizationType();
          const bool realTimeAutoBin = _histogramTab->realTimeAutoBinDirty() ?  _histogramTab->realTimeAutoBin() : histogram->realTimeAutoBin();

          histogram->writeLock();
          histogram->setVector(vector);
          histogram->setXRange(min, max);
          histogram->setNumberOfBins(bins);
          histogram->setNormalizationType(normalizationType);
          histogram->setRealTimeAutoBin(realTimeAutoBin);

          histogram->unlock();

          histogram->inputObjectUpdated(histogram);
        }
      }
    } else {
      histogram->writeLock();
      histogram->setVector(_histogramTab->vector());
      histogram->setXRange(_histogramTab->min(), _histogramTab->max());
      histogram->setNumberOfBins(_histogramTab->bins());
      histogram->setNormalizationType(_histogramTab->normalizationType());
      histogram->setRealTimeAutoBin(_histogramTab->realTimeAutoBin());
      if (DataDialog::tagStringAuto()) {
        histogram->setDescriptiveName(QString());
      } else {
        histogram->setDescriptiveName(DataDialog::tagString());
      }

      histogram->unlock();

      histogram->inputObjectUpdated(histogram);
      setHistogramDefaults(histogram);
    }
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
