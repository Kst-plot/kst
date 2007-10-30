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

namespace Kst {

CSDTab::CSDTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Cumulative Spectral Decay"));

}


CSDTab::~CSDTab() {
}


VectorPtr CSDTab::vector() const {
  return _vector->selectedVector();
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

CSDDialog::CSDDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Cumulative Spectral Decay"));
  else
    setWindowTitle(tr("New Cumulative Spectral Decay"));

  _CSDTab = new CSDTab(this);
  addDataTab(_CSDTab);

  //FIXME need to do validation to enable/disable ok button...
}


CSDDialog::~CSDDialog() {
}


QString CSDDialog::tagString() const {
  return DataDialog::tagString();
}


ObjectPtr CSDDialog::createNewDataObject() const {
  Q_ASSERT(_document && _document->objectStore());
  ObjectTag tag = _document->objectStore()->suggestObjectTag<CSD>(tagString(), ObjectTag::globalTagContext);
  CSDPtr csd = _document->objectStore()->createObject<CSD>(tag);
  csd->setVector(_CSDTab->vector());
  csd->setFreq(_CSDTab->FFTOptionsWidget()->sampleRate());
  csd->setAverage(_CSDTab->FFTOptionsWidget()->interleavedAverage());
  csd->setRemoveMean(_CSDTab->FFTOptionsWidget()->removeMean());
  csd->setApodize(_CSDTab->FFTOptionsWidget()->apodize());
  csd->setApodizeFxn(_CSDTab->FFTOptionsWidget()->apodizeFunction());
  csd->setWindowSize(_CSDTab->windowSize());
  csd->setLength(_CSDTab->FFTOptionsWidget()->FFTLength());
  csd->setGaussianSigma(_CSDTab->FFTOptionsWidget()->sigma());
  csd->setOutput(_CSDTab->FFTOptionsWidget()->output());
  csd->setVectorUnits(_CSDTab->FFTOptionsWidget()->vectorUnits());
  csd->setRateUnits(_CSDTab->FFTOptionsWidget()->rateUnits());

  csd->writeLock();
  csd->update(0);
  csd->unlock();

  Palette* newPalette = new Palette(_CSDTab->colorPalette()->selectedPalette());

  tag = _document->objectStore()->suggestObjectTag<Image>(csd->tag().tagString(), ObjectTag::globalTagContext);
  ImagePtr image = _document->objectStore()->createObject<Image>(tag);
  image->changeToColorOnly(csd->outputMatrix(), 0, 1, true, newPalette->paletteData());

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
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
