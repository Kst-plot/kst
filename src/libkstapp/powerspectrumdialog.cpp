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

#include "powerspectrumdialog.h"

#include "dialogpage.h"

#include "psd.h"

#include "view.h"
#include "plotitem.h"
#include "tabwidget.h"
#include "mainwindow.h"
#include "application.h"
#include "plotrenderitem.h"
#include "curve.h"
#include "document.h"
#include "objectstore.h"

#include "defaultnames.h"
#include "datacollection.h"
#include "dataobjectcollection.h"
#include "dialogdefaults.h"

#include <QPushButton>

namespace Kst {

PowerSpectrumTab::PowerSpectrumTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Power Spectrum"));

  connect(_vector, SIGNAL(selectionChanged(QString)), this, SLOT(selectionChanged()));
}


PowerSpectrumTab::~PowerSpectrumTab() {
}


VectorPtr PowerSpectrumTab::vector() const {
  return _vector->selectedVector();
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
  setMaximumHeight(235);
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
  updateButtons();
}


PowerSpectrumDialog::~PowerSpectrumDialog() {
}


QString PowerSpectrumDialog::tagString() const {
  return DataDialog::tagString();
}


void PowerSpectrumDialog::configureTab(ObjectPtr object) {
  if (!object) {
    _powerSpectrumTab->FFTOptionsWidget()->setSampleRate(Kst::dialogDefaults->value("spectrum/freq",100.0).toDouble());
    _powerSpectrumTab->FFTOptionsWidget()->setInterleavedAverage(Kst::dialogDefaults->value("spectrum/average",true).toBool());
    _powerSpectrumTab->FFTOptionsWidget()->setFFTLength(Kst::dialogDefaults->value("spectrum/len",12).toInt());
    _powerSpectrumTab->FFTOptionsWidget()->setApodize(Kst::dialogDefaults->value("spectrum/apodize",true).toBool());
    _powerSpectrumTab->FFTOptionsWidget()->setRemoveMean(Kst::dialogDefaults->value("spectrum/removeMean",true).toBool());
    _powerSpectrumTab->FFTOptionsWidget()->setVectorUnits(Kst::dialogDefaults->value("spectrum/vUnits","V").toString());
    _powerSpectrumTab->FFTOptionsWidget()->setRateUnits(Kst::dialogDefaults->value("spectrum/rUnits","Hz").toString());
    _powerSpectrumTab->FFTOptionsWidget()->setApodizeFunction(ApodizeFunction(Kst::dialogDefaults->value("spectrum/apodizeFxn",WindowOriginal).toInt()));
    _powerSpectrumTab->FFTOptionsWidget()->setSigma(Kst::dialogDefaults->value("spectrum/gaussianSigma",1.0).toDouble());
    _powerSpectrumTab->FFTOptionsWidget()->setOutput(PSDType(Kst::dialogDefaults->value("spectrum/output",PSDPowerSpectralDensity).toInt()));
    _powerSpectrumTab->FFTOptionsWidget()->setInterpolateOverHoles(Kst::dialogDefaults->value("spectrum/interpolateHoles",true).toInt());

//xxxxxxxxxx fill the dialog from the settings...
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
  }
}


void PowerSpectrumDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_powerSpectrumTab->vector());
}


ObjectPtr PowerSpectrumDialog::createNewDataObject() const {
  Q_ASSERT(_document && _document->objectStore());
  ObjectTag tag = _document->objectStore()->suggestObjectTag<PSD>(tagString(), ObjectTag::globalTagContext);
  PSDPtr powerspectrum = _document->objectStore()->createObject<PSD>(tag);
  Q_ASSERT(powerspectrum);

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

  powerspectrum->update(0);
  powerspectrum->unlock();
  setSpectrumDefaults(powerspectrum);

  //FIXME this should be a command...
  //FIXME need some smart placement...

  tag = _document->objectStore()->suggestObjectTag<Curve>(powerspectrum->tag().tagString(), ObjectTag::globalTagContext);
  CurvePtr curve = _document->objectStore()->createObject<Curve>(tag);
  Q_ASSERT(curve);

  curve->setXVector(powerspectrum->vX());
  curve->setYVector(powerspectrum->vY());
  curve->setColor(_powerSpectrumTab->curveAppearance()->color());
  curve->setHasPoints(_powerSpectrumTab->curveAppearance()->showPoints());
  curve->setHasLines(_powerSpectrumTab->curveAppearance()->showLines());
  curve->setHasBars(_powerSpectrumTab->curveAppearance()->showBars());
  curve->setLineWidth(_powerSpectrumTab->curveAppearance()->lineWidth());
  curve->setLineStyle(_powerSpectrumTab->curveAppearance()->lineStyle());
  curve->setPointType(_powerSpectrumTab->curveAppearance()->pointType());
  curve->setPointDensity(_powerSpectrumTab->curveAppearance()->pointDensity());
  curve->setBarStyle(_powerSpectrumTab->curveAppearance()->barStyle());

  curve->writeLock();
  curve->update(0);
  curve->unlock();

  PlotItem *plotItem = 0;
  switch (_powerSpectrumTab->curvePlacement()->place()) {
  case CurvePlacement::NoPlot:
    break;
  case CurvePlacement::ExistingPlot:
    {
      plotItem = static_cast<PlotItem*>(_powerSpectrumTab->curvePlacement()->existingPlot());
      break;
    }
  case CurvePlacement::NewPlot:
    {
      CreatePlotForCurve *cmd = new CreatePlotForCurve(
        _powerSpectrumTab->curvePlacement()->createLayout(),
        _powerSpectrumTab->curvePlacement()->appendToLayout());
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

  return ObjectPtr(powerspectrum.data());
}


ObjectPtr PowerSpectrumDialog::editExistingDataObject() const {
  if (PSDPtr powerspectrum = kst_cast<PSD>(dataObject())) {
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

    powerspectrum->update(0);
    powerspectrum->unlock();

    setSpectrumDefaults(powerspectrum);

  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
