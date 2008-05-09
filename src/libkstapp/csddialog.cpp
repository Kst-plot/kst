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

#include "csddialog.h"

#include "dialogpage.h"
#include "editmultiplewidget.h"

#include "psd.h"

#include "view.h"
#include "plotitem.h"
#include "tabwidget.h"
#include "mainwindow.h"
#include "document.h"
#include "objectstore.h"
#include "application.h"
#include "plotrenderitem.h"
#include "curve.h"
#include "palette.h"
#include "image.h"

#include "defaultnames.h"
#include "datacollection.h"
#include "dataobjectcollection.h"

#include <QPushButton>

namespace Kst {

CSDTab::CSDTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Cumulative Spectral Decay"));
  connect(_vector, SIGNAL(selectionChanged(QString)), this, SLOT(selectionChanged()));
  connect(_FFTOptions, SIGNAL(modified()), this, SIGNAL(modified()));
  connect(_vector, SIGNAL(selectionChanged(QString)), this, SIGNAL(modified()));
  connect(_windowSize, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
}


CSDTab::~CSDTab() {
}


void CSDTab::selectionChanged() {
  emit optionsChanged();
}


VectorPtr CSDTab::vector() const {
  return _vector->selectedVector();
}


bool CSDTab::vectorDirty() const {
  return _vector->selectedVectorDirty();
}


void CSDTab::setVector(const VectorPtr vector) {
  _vector->setSelectedVector(vector);
}


CurvePlacement* CSDTab::curvePlacement() const {
  return _curvePlacement;
}


FFTOptions* CSDTab::FFTOptionsWidget() const {
  return _FFTOptions;
}


ColorPalette* CSDTab::colorPalette() const {
  return _colorPalette;
}


int CSDTab::windowSize() const {
  return _windowSize->value();
}


bool CSDTab::windowSizeDirty() const {
  return !_windowSize->text().isEmpty();
}


void CSDTab::setWindowSize(const int windowSize) {
  _windowSize->setValue(windowSize);
}


void CSDTab::setObjectStore(ObjectStore *store) {
  _vector->setObjectStore(store);
}


void CSDTab::hideImageOptions() {
  _imageOptionsGroup->setVisible(false);
  _curvePlacement->setVisible(false);
  setMaximumHeight(250);
}


void CSDTab::clearTabValues() {
  _vector->clearSelection();
  _windowSize->clear();
  _FFTOptions->clearValues();
  _FFTOptions->disableInterpolateOverHoles();
}


CSDDialog::CSDDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Cumulative Spectral Decay"));
  else
    setWindowTitle(tr("New Cumulative Spectral Decay"));

  _CSDTab = new CSDTab(this);
  addDataTab(_CSDTab);

  if (editMode() == Edit) {
    configureTab(dataObject);
  }

  _CSDTab->FFTOptionsWidget()->disableInterpolateOverHoles();

  connect(_CSDTab, SIGNAL(optionsChanged()), this, SLOT(updateButtons()));  
  connect(this, SIGNAL(editMultipleMode()), this, SLOT(editMultipleMode()));
  connect(this, SIGNAL(editSingleMode()), this, SLOT(editSingleMode()));

  connect(_CSDTab, SIGNAL(modified()), this, SLOT(modified()));
  updateButtons();
}


CSDDialog::~CSDDialog() {
}


QString CSDDialog::tagString() const {
  return DataDialog::tagString();
}


void CSDDialog::editMultipleMode() {
  _CSDTab->clearTabValues();
}


void CSDDialog::editSingleMode() {
   configureTab(dataObject());
}


void CSDDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_CSDTab->vector() || (editMode() == EditMultiple));
}


void CSDDialog::setVector(VectorPtr vector) {
  _CSDTab->setVector(vector);
}


void CSDDialog::configureTab(ObjectPtr object) {
  if (CSDPtr csd = kst_cast<CSD>(object)) {
    _CSDTab->setVector(csd->vector());
    _CSDTab->setWindowSize(csd->windowSize());
    _CSDTab->FFTOptionsWidget()->setSampleRate(csd->frequency());
    _CSDTab->FFTOptionsWidget()->setInterleavedAverage(csd->average());
    _CSDTab->FFTOptionsWidget()->setFFTLength(csd->length());
    _CSDTab->FFTOptionsWidget()->setApodize(csd->apodize());
    _CSDTab->FFTOptionsWidget()->setRemoveMean(csd->removeMean());
    _CSDTab->FFTOptionsWidget()->setVectorUnits(csd->vectorUnits());
    _CSDTab->FFTOptionsWidget()->setRateUnits(csd->rateUnits());
    _CSDTab->FFTOptionsWidget()->setApodizeFunction(csd->apodizeFxn());
    _CSDTab->FFTOptionsWidget()->setSigma(csd->gaussianSigma());
    _CSDTab->FFTOptionsWidget()->setOutput(csd->output());
    _CSDTab->hideImageOptions();
    if (_editMultipleWidget) {
      QStringList objectList;
      CSDList objects = _document->objectStore()->getObjects<CSD>();
      foreach(CSDPtr object, objects) {
        objectList.append(object->tag().displayString());
      }
      _editMultipleWidget->addObjects(objectList);
    }
  }
}


ObjectPtr CSDDialog::createNewDataObject() const {
  Q_ASSERT(_document && _document->objectStore());
  ObjectTag tag = _document->objectStore()->suggestObjectTag<CSD>(tagString(), ObjectTag::globalTagContext);
  CSDPtr csd = _document->objectStore()->createObject<CSD>(tag);
  csd->change(_CSDTab->vector(),
              _CSDTab->FFTOptionsWidget()->sampleRate(), 
              _CSDTab->FFTOptionsWidget()->interleavedAverage(),
              _CSDTab->FFTOptionsWidget()->removeMean(),
              _CSDTab->FFTOptionsWidget()->apodize(),
              _CSDTab->FFTOptionsWidget()->apodizeFunction(),
              _CSDTab->windowSize(), 
              _CSDTab->FFTOptionsWidget()->FFTLength(), 
              _CSDTab->FFTOptionsWidget()->sigma(),
              _CSDTab->FFTOptionsWidget()->output(), 
              _CSDTab->FFTOptionsWidget()->vectorUnits(),
              _CSDTab->FFTOptionsWidget()->rateUnits());
  csd->setDescriptiveName(DataDialog::tagString().replace(defaultTagString(), QString()));

  csd->writeLock();
  csd->update();
  csd->unlock();

  tag = _document->objectStore()->suggestObjectTag<Image>(csd->tag().tagString(), ObjectTag::globalTagContext);
  ImagePtr image = _document->objectStore()->createObject<Image>(tag);
  image->changeToColorOnly(csd->outputMatrix(), 0, 1, true, _CSDTab->colorPalette()->selectedPalette());

  image->writeLock();
  image->update();
  image->unlock();

  PlotItem *plotItem = 0;
  switch (_CSDTab->curvePlacement()->place()) {
  case CurvePlacement::NoPlot:
    break;
  case CurvePlacement::ExistingPlot:
    {
      plotItem = static_cast<PlotItem*>(_CSDTab->curvePlacement()->existingPlot());
      break;
    }
  case CurvePlacement::NewPlot:
    {
      CreatePlotForCurve *cmd = new CreatePlotForCurve(
        _CSDTab->curvePlacement()->createLayout(),
        _CSDTab->curvePlacement()->appendToLayout());
      cmd->createItem();

      plotItem = static_cast<PlotItem*>(cmd->item());
      break;
    }
  default:
    break;
  }

  PlotRenderItem *renderItem = plotItem->renderItem(PlotRenderItem::Cartesian);
  renderItem->addRelation(kst_cast<Relation>(image));
  plotItem->update();

  return ObjectPtr(image.data());
}


ObjectPtr CSDDialog::editExistingDataObject() const {
  if (CSDPtr csd = kst_cast<CSD>(dataObject())) {
    if (editMode() == EditMultiple) {
      const FFTOptions *options = _CSDTab->FFTOptionsWidget();
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (QString objectTag, objects) {
        CSDPtr csd = kst_cast<CSD>(_document->objectStore()->retrieveObject(ObjectTag::fromString(objectTag)));
        if (csd) {
          VectorPtr vector = _CSDTab->vectorDirty() ? _CSDTab->vector() : csd->vector();
          const double frequency = options->sampleRateDirty() ? options->sampleRate() : csd->frequency();
          const double sigma = options->sigmaDirty() ? options->sigma() : csd->gaussianSigma();
          const bool apodize = options->apodizeDirty() ? options->apodize() : csd->apodize();
          const bool removeMean = options->removeMeanDirty() ? options->removeMean() : csd->removeMean();
          const bool interleavedAverage = options->interleavedAverageDirty() ? options->interleavedAverage() : csd->average();
          const int FFTLength = options->FFTLengthDirty() ? options->FFTLength() : csd->length();
          const ApodizeFunction apodizeFunction = options->apodizeFunctionDirty() ? options->apodizeFunction() : csd->apodizeFxn();
          const PSDType output = options->outputDirty() ? options->output() : csd->output();
          const QString vectorUnits = options->vectorUnitsDirty() ? options->vectorUnits() : csd->vectorUnits();
          const QString rateUnits = options->rateUnitsDirty() ? options->rateUnits() : csd->rateUnits();
          const int windowSize = _CSDTab->windowSizeDirty() ? _CSDTab->windowSize() : csd->windowSize();

          csd->writeLock();
          csd->change(vector,
                      frequency,
                      interleavedAverage,
                      removeMean,
                      apodize,
                      apodizeFunction,
                      windowSize,
                      FFTLength,
                      sigma,
                      output,
                      vectorUnits,
                      rateUnits);

          csd->update();
          csd->unlock();
        }
      }
    } else {
      csd->writeLock();
      csd->change(_CSDTab->vector(),
                  _CSDTab->FFTOptionsWidget()->sampleRate(), 
                  _CSDTab->FFTOptionsWidget()->interleavedAverage(),
                  _CSDTab->FFTOptionsWidget()->removeMean(),
                  _CSDTab->FFTOptionsWidget()->apodize(),
                  _CSDTab->FFTOptionsWidget()->apodizeFunction(),
                  _CSDTab->windowSize(), 
                  _CSDTab->FFTOptionsWidget()->FFTLength(), 
                  _CSDTab->FFTOptionsWidget()->sigma(),
                  _CSDTab->FFTOptionsWidget()->output(), 
                  _CSDTab->FFTOptionsWidget()->vectorUnits(),
                  _CSDTab->FFTOptionsWidget()->rateUnits());
      csd->setDescriptiveName(DataDialog::tagString().replace(defaultTagString(), QString()));

      csd->update();
      csd->unlock();
    }
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
