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

#include "powerspectrumdialog.h"

#include "application.h"
#include "curve.h"
#include "datacollection.h"
#include "dialogdefaults.h"
#include "dialogpage.h"
#include "document.h"
#include "mainwindow.h"
#include "objectstore.h"
#include "plotitem.h"
#include "plotrenderitem.h"
#include "psd.h"
#include "tabwidget.h"
#include "view.h"
#include "editmultiplewidget.h"

#include <QPushButton>

namespace Kst {

PowerSpectrumTab::PowerSpectrumTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Power Spectrum"));

  _curvePlacement->setExistingPlots(Data::self()->plotList());

  connect(_vector, SIGNAL(selectionChanged(QString)), this, SLOT(selectionChanged()));
  connect(_FFTOptions, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_vector, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));

  _vectorLabel->setBuddy(_vector->_vector);

}


PowerSpectrumTab::~PowerSpectrumTab() {
}


VectorPtr PowerSpectrumTab::vector() const {
  return _vector->selectedVector();
}


bool PowerSpectrumTab::vectorDirty() const {
  return _vector->selectedVectorDirty();
}


void PowerSpectrumTab::setVector(const VectorPtr vector) {
  _vector->setSelectedVector(vector);
}


void PowerSpectrumTab::selectionChanged() {
  emit vectorChanged();
}


CurveAppearance* PowerSpectrumTab::curveAppearance() const {
  return _curveAppearance;
}


CurvePlacement* PowerSpectrumTab::curvePlacement() const {
  return _curvePlacement;
}


FFTOptions* PowerSpectrumTab::FFTOptionsWidget() const {
  return _FFTOptions;
}


void PowerSpectrumTab::setObjectStore(ObjectStore *store) {
  _vector->setObjectStore(store);
}


void PowerSpectrumTab::hideCurveOptions() {
  _curvePlacement->setVisible(false);
  _curveAppearance->setVisible(false);
}


void PowerSpectrumTab::clearTabValues() {
  _vector->clearSelection();
  _FFTOptions->clearValues();
}


PowerSpectrumDialog::PowerSpectrumDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Power Spectrum"));
  else
    setWindowTitle(tr("New Power Spectrum"));

  _powerSpectrumTab = new PowerSpectrumTab(this);
  addDataTab(_powerSpectrumTab);

  if (editMode() == Edit) {
    configureTab(dataObject);
  } else {
    configureTab(0);
  }

  connect(_powerSpectrumTab, SIGNAL(vectorChanged()), this, SLOT(updateButtons()));
  connect(this, SIGNAL(editMultipleMode()), this, SLOT(editMultipleMode()));
  connect(this, SIGNAL(editSingleMode()), this, SLOT(editSingleMode()));

  connect(_powerSpectrumTab, SIGNAL(modified()), this, SLOT(modified()));
  updateButtons();
}


PowerSpectrumDialog::~PowerSpectrumDialog() {
}


// QString PowerSpectrumDialog::tagString() const {
//   return DataDialog::tagString();
// }


void PowerSpectrumDialog::editMultipleMode() {
  _powerSpectrumTab->clearTabValues();
}


void PowerSpectrumDialog::editSingleMode() {
   configureTab(dataObject());
}


void PowerSpectrumDialog::setVector(VectorPtr vector) {
  _powerSpectrumTab->setVector(vector);
}



void PowerSpectrumDialog::configureTab(ObjectPtr object) {
  if (!object) {
    _powerSpectrumTab->FFTOptionsWidget()->loadWidgetDefaults();
    _powerSpectrumTab->curveAppearance()->loadWidgetDefaults();

  } else if (PSDPtr psd = kst_cast<PSD>(object)) {
    _powerSpectrumTab->setVector(psd->vector());

    _powerSpectrumTab->FFTOptionsWidget()->setSampleRate(psd->frequency());
    _powerSpectrumTab->FFTOptionsWidget()->setInterleavedAverage(psd->average());
    _powerSpectrumTab->FFTOptionsWidget()->setFFTLength(psd->length());
    _powerSpectrumTab->FFTOptionsWidget()->setApodize(psd->apodize());
    _powerSpectrumTab->FFTOptionsWidget()->setRemoveMean(psd->removeMean());
    _powerSpectrumTab->FFTOptionsWidget()->setVectorUnits(psd->vectorUnits());
    _powerSpectrumTab->FFTOptionsWidget()->setRateUnits(psd->rateUnits());
    _powerSpectrumTab->FFTOptionsWidget()->setApodizeFunction(psd->apodizeFxn());
    _powerSpectrumTab->FFTOptionsWidget()->setSigma(psd->gaussianSigma());
    _powerSpectrumTab->FFTOptionsWidget()->setOutput(psd->output());
    _powerSpectrumTab->FFTOptionsWidget()->setInterpolateOverHoles(psd->interpolateHoles());
    _powerSpectrumTab->hideCurveOptions();
    if (_editMultipleWidget) {
      PSDList objects = _document->objectStore()->getObjects<PSD>();
      _editMultipleWidget->clearObjects();
      foreach(const PSDPtr &object, objects) {
        _editMultipleWidget->addObject(object->Name(), object->descriptionTip());
      }
    }
  }
}


void PowerSpectrumDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(dialogValid());
}


bool PowerSpectrumDialog::dialogValid() const {
  bool valid = _powerSpectrumTab->vector() || (editMode() == EditMultiple);
  return valid;
}

ObjectPtr PowerSpectrumDialog::createNewDataObject() {
  Q_ASSERT(_document && _document->objectStore());
  PSDPtr powerspectrum = _document->objectStore()->createObject<PSD>();
  Q_ASSERT(powerspectrum);

  powerspectrum->writeLock();
  powerspectrum->change(_powerSpectrumTab->vector(),
                        _powerSpectrumTab->FFTOptionsWidget()->sampleRate(), 
                        _powerSpectrumTab->FFTOptionsWidget()->interleavedAverage(),
                        _powerSpectrumTab->FFTOptionsWidget()->FFTLength(),
                        _powerSpectrumTab->FFTOptionsWidget()->apodize(),
                        _powerSpectrumTab->FFTOptionsWidget()->removeMean(),
                        _powerSpectrumTab->FFTOptionsWidget()->vectorUnits(),
                        _powerSpectrumTab->FFTOptionsWidget()->rateUnits(),
                        _powerSpectrumTab->FFTOptionsWidget()->apodizeFunction(),
                        _powerSpectrumTab->FFTOptionsWidget()->sigma(),
                        _powerSpectrumTab->FFTOptionsWidget()->output(),
                        _powerSpectrumTab->FFTOptionsWidget()->interpolateOverHoles());

  if (DataDialog::tagStringAuto()) {
     powerspectrum->setDescriptiveName(QString());
  } else {
     powerspectrum->setDescriptiveName(DataDialog::tagString());
  }

  powerspectrum->registerChange();
  powerspectrum->unlock();

  _powerSpectrumTab->FFTOptionsWidget()->setWidgetDefaults();

  CurvePtr curve = _document->objectStore()->createObject<Curve>();
  Q_ASSERT(curve);

  curve->setXVector(powerspectrum->vX());
  curve->setYVector(powerspectrum->vY());
  curve->setColor(_powerSpectrumTab->curveAppearance()->color());
  curve->setHeadColor(_powerSpectrumTab->curveAppearance()->headColor());
  curve->setBarFillColor(_powerSpectrumTab->curveAppearance()->barFillColor());
  curve->setHasPoints(_powerSpectrumTab->curveAppearance()->showPoints());
  curve->setHasLines(_powerSpectrumTab->curveAppearance()->showLines());
  curve->setHasBars(_powerSpectrumTab->curveAppearance()->showBars());
  curve->setHasHead(_powerSpectrumTab->curveAppearance()->showHead());
  curve->setLineWidth(_powerSpectrumTab->curveAppearance()->lineWidth());
  curve->setLineStyle(_powerSpectrumTab->curveAppearance()->lineStyle());
  curve->setPointType(_powerSpectrumTab->curveAppearance()->pointType());
  curve->setHeadType(_powerSpectrumTab->curveAppearance()->headType());
  curve->setPointDensity(_powerSpectrumTab->curveAppearance()->pointDensity());

  curve->writeLock();
  curve->registerChange();
  curve->unlock();

  _powerSpectrumTab->curveAppearance()->setWidgetDefaults();

  PlotItem *plotItem = 0;
  switch (_powerSpectrumTab->curvePlacement()->place()) {
  case CurvePlacement::NoPlot:
    break;
  case CurvePlacement::ExistingPlot:
    {
      plotItem = static_cast<PlotItem*>(_powerSpectrumTab->curvePlacement()->existingPlot());
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
      if (_powerSpectrumTab->curvePlacement()->scaleFonts()) {
        plotItem->view()->resetPlotFontSizes();
      }
      break;
    }
  default:
    break;
  }

  if (_powerSpectrumTab->curvePlacement()->place() != CurvePlacement::NoPlot) {
    PlotRenderItem *renderItem = plotItem->renderItem(PlotRenderItem::Cartesian);
    renderItem->addRelation(kst_cast<Relation>(curve));
    plotItem->update();

    if (_powerSpectrumTab->curvePlacement()->place() != CurvePlacement::ExistingPlot) {
      plotItem->view()->appendToLayout(_powerSpectrumTab->curvePlacement()->layout(), plotItem, _powerSpectrumTab->curvePlacement()->gridColumns());
    }
  }

  return ObjectPtr(powerspectrum.data());
}


ObjectPtr PowerSpectrumDialog::editExistingDataObject() const {
  if (PSDPtr powerspectrum = kst_cast<PSD>(dataObject())) {
    if (editMode() == EditMultiple) {
      const FFTOptions *options = _powerSpectrumTab->FFTOptionsWidget();
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (const QString &objectName, objects) {
        PSDPtr powerspectrum = kst_cast<PSD>(_document->objectStore()->retrieveObject(objectName));
        if (powerspectrum) {
          VectorPtr vector = _powerSpectrumTab->vectorDirty() ? _powerSpectrumTab->vector() : powerspectrum->vector();
          const double frequency = options->sampleRateDirty() ? options->sampleRate() : powerspectrum->frequency();
          const double sigma = options->sigmaDirty() ? options->sigma() : powerspectrum->gaussianSigma();

          const bool apodize = options->apodizeDirty() ? options->apodize() : powerspectrum->apodize();
          const bool removeMean = options->removeMeanDirty() ? options->removeMean() : powerspectrum->removeMean();
          const bool interpolateOverHoles = options->interpolateOverHolesDirty() ? options->interpolateOverHoles() : powerspectrum->interpolateHoles();
          const bool interleavedAverage = options->interleavedAverageDirty() ? options->interleavedAverage() : powerspectrum->average();
          const int FFTLength = options->FFTLengthDirty() ? options->FFTLength() : powerspectrum->length();
          const ApodizeFunction apodizeFunction = options->apodizeFunctionDirty() ? options->apodizeFunction() : powerspectrum->apodizeFxn();
          const PSDType output = options->outputDirty() ? options->output() : powerspectrum->output();
          const QString vectorUnits = options->vectorUnitsDirty() ? options->vectorUnits() : powerspectrum->vectorUnits();
          const QString rateUnits = options->rateUnitsDirty() ? options->rateUnits() : powerspectrum->rateUnits();

          powerspectrum->writeLock();
          powerspectrum->setVector(vector);
          powerspectrum->setFrequency(frequency);
          powerspectrum->setAverage(interleavedAverage);
          powerspectrum->setLength(FFTLength);
          powerspectrum->setApodize(apodize);
          powerspectrum->setRemoveMean(removeMean);
          powerspectrum->setVectorUnits(vectorUnits);
          powerspectrum->setRateUnits(rateUnits);
          powerspectrum->setApodizeFxn(apodizeFunction);
          powerspectrum->setGaussianSigma(sigma);
          powerspectrum->setOutput(output);
          powerspectrum->setInterpolateHoles(interpolateOverHoles);

          powerspectrum->registerChange();
          powerspectrum->unlock();
        }
      }
    } else {
      powerspectrum->writeLock();
      powerspectrum->setVector(_powerSpectrumTab->vector());
      powerspectrum->setFrequency(_powerSpectrumTab->FFTOptionsWidget()->sampleRate());
      powerspectrum->setAverage(_powerSpectrumTab->FFTOptionsWidget()->interleavedAverage());
      powerspectrum->setLength(_powerSpectrumTab->FFTOptionsWidget()->FFTLength());
      powerspectrum->setApodize(_powerSpectrumTab->FFTOptionsWidget()->apodize());
      powerspectrum->setRemoveMean(_powerSpectrumTab->FFTOptionsWidget()->removeMean());
      powerspectrum->setVectorUnits(_powerSpectrumTab->FFTOptionsWidget()->vectorUnits());
      powerspectrum->setRateUnits(_powerSpectrumTab->FFTOptionsWidget()->rateUnits());
      powerspectrum->setApodizeFxn(_powerSpectrumTab->FFTOptionsWidget()->apodizeFunction());
      powerspectrum->setGaussianSigma(_powerSpectrumTab->FFTOptionsWidget()->sigma());
      powerspectrum->setOutput(_powerSpectrumTab->FFTOptionsWidget()->output());
      powerspectrum->setInterpolateHoles(_powerSpectrumTab->FFTOptionsWidget()->interpolateOverHoles());
      if (DataDialog::tagStringAuto()) {
        powerspectrum->setDescriptiveName(QString());
      } else {
        powerspectrum->setDescriptiveName(DataDialog::tagString());
      }

      powerspectrum->registerChange();
      powerspectrum->unlock();

      _powerSpectrumTab->FFTOptionsWidget()->setWidgetDefaults();
    }
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
