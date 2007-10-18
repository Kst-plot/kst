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

#include "histogram.h"

#include "view.h"
#include "plotitem.h"
#include "tabwidget.h"
#include "mainwindow.h"
#include "application.h"
#include "plotrenderitem.h"
#include "curve.h"

#include "defaultnames.h"
#include "datacollection.h"
#include "dataobjectcollection.h"

namespace Kst {

HistogramTab::HistogramTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Histogram"));

  connect(AutoBin, SIGNAL(clicked()), this, SLOT(generateAutoBin()));
  connect(_realTimeAutoBin, SIGNAL(clicked()), this, SLOT(updateButtons()));

  _curvePlacement->setExistingPlots(Data::self()->plotList());
  _curveAppearance->setValue(false, false, true, _curveAppearance->color(), 0, 0, 0, 1, 0);
}


HistogramTab::~HistogramTab() {
}


void HistogramTab::generateAutoBin() {

  VectorPtr selectedVector = vector();

  selectedVector->readLock(); // Hmm should we really lock here?  AutoBin should I think
  int n;
  double max, min;
  Histogram::AutoBin(selectedVector, &n, &max, &min);
  selectedVector->unlock();

  _numBins->setValue(n);
  _min->setText(QString::number(min));
  _max->setText(QString::number(max));
}


void HistogramTab::updateButtons() {
  if (_realTimeAutoBin->isChecked()) {
    generateAutoBin();
  }

  _min->setEnabled(!_realTimeAutoBin->isChecked());
  _max->setEnabled(!_realTimeAutoBin->isChecked());
  _numBins->setEnabled(!_realTimeAutoBin->isChecked());
  AutoBin->setEnabled(!_realTimeAutoBin->isChecked());
}


VectorPtr HistogramTab::vector() const {
  return _vector->selectedVector();
}


double HistogramTab::min() const {
  return _min->text().toDouble();
}


double HistogramTab::max() const {
  return _max->text().toDouble();
}


int HistogramTab::bins() const {
  return _numBins->text().toInt();
}


HsNormType HistogramTab::normalizationType() const {
  HsNormType normalization = KST_HS_NUMBER;

  if (_normIsFraction) {
    normalization = KST_HS_FRACTION;
  } else if (_normIsPercent) {
    normalization = KST_HS_PERCENT;
  } else if (_normPeakIs1) {
    normalization = KST_HS_MAX_ONE;
  }
  return normalization;
}

CurveAppearance* HistogramTab::curveAppearance() const {
  return _curveAppearance;
}


CurvePlacement* HistogramTab::curvePlacement() const {
  return _curvePlacement;
}


HistogramDialog::HistogramDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Histogram"));
  else
    setWindowTitle(tr("New Histogram"));

  _histogramTab = new HistogramTab(this);
  addDataTab(_histogramTab);

  //FIXME need to do validation to enable/disable ok button...
}


HistogramDialog::~HistogramDialog() {
}


QString HistogramDialog::tagName() const {
  return DataDialog::tagName();
}


ObjectPtr HistogramDialog::createNewDataObject() const {
  //FIXME Eli, how should I construct this tag??
  HistogramPtr histogram = new Histogram(tagName(),
                                     _histogramTab->vector(),
                                     _histogramTab->min(),
                                     _histogramTab->max(),
                                     _histogramTab->bins(),
                                     _histogramTab->normalizationType());

  histogram->writeLock();
  histogram->update(0);
  histogram->unlock();

  //FIXME this should be a command...
  //FIXME need some smart placement...

  CurvePtr curve = new Curve(suggestCurveName(histogram->tag(), true),
                                     histogram->vX(),
                                     histogram->vY(),
                                     0L, 0L, 0L, 0L,
                                     _histogramTab->curveAppearance()->color());

  curve->setHasPoints(_histogramTab->curveAppearance()->showPoints());
  curve->setHasLines(_histogramTab->curveAppearance()->showLines());
  curve->setHasBars(_histogramTab->curveAppearance()->showBars());
  curve->setLineWidth(_histogramTab->curveAppearance()->lineWidth());
  curve->setLineStyle(_histogramTab->curveAppearance()->lineStyle());
  curve->pointType = _histogramTab->curveAppearance()->pointType();
  curve->setPointDensity(_histogramTab->curveAppearance()->pointDensity());
  curve->setBarStyle(_histogramTab->curveAppearance()->barStyle());

  curve->writeLock();
  curve->update(0);
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
      CreatePlotForCurve *cmd = new CreatePlotForCurve(
        _histogramTab->curvePlacement()->createLayout(),
        _histogramTab->curvePlacement()->appendToLayout());
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

  return ObjectPtr(histogram.data());
}


ObjectPtr HistogramDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
