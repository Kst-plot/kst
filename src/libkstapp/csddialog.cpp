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
}


CSDTab::~CSDTab() {
}


void CSDTab::selectionChanged() {
  emit optionsChanged();
}


VectorPtr CSDTab::vector() const {
  return _vector->selectedVector();
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

  connect(_CSDTab, SIGNAL(optionsChanged()), this, SLOT(updateButtons()));
  updateButtons();
}


CSDDialog::~CSDDialog() {
}


QString CSDDialog::tagString() const {
  return DataDialog::tagString();
}


void CSDDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_CSDTab->vector());
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

  csd->writeLock();
  csd->update(0);
  csd->unlock();

  tag = _document->objectStore()->suggestObjectTag<Image>(csd->tag().tagString(), ObjectTag::globalTagContext);
  ImagePtr image = _document->objectStore()->createObject<Image>(tag);
  image->changeToColorOnly(csd->outputMatrix(), 0, 1, true, _CSDTab->colorPalette()->selectedPalette());

  image->writeLock();
  image->update(0);
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
  //TODO  Adam, is this the correct way to draw an image?  It runs very slow.
  renderItem->addRelation(kst_cast<Relation>(image));
  plotItem->update();

  return ObjectPtr(image.data());
}


ObjectPtr CSDDialog::editExistingDataObject() const {
  if (CSDPtr csd = kst_cast<CSD>(dataObject())) {
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

    csd->update(0);
    csd->unlock();
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
