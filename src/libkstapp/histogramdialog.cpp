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
}


HistogramTab::~HistogramTab() {
}


void HistogramTab::generateAutoBin() {
  vectorList.lock();

  if (!vectorList.isEmpty()) {
    VectorList::Iterator i = vectorList.findTag(_vector->selectedVector()->tag());
    double max, min;
    int n;

    if (i == vectorList.end()) {
      qFatal("Bug in kst: the Vector field in dialog refers to a non existant vector...");
    }
    (*i)->readLock(); // Hmm should we really lock here?  AutoBin should I think
    Histogram::AutoBin(VectorPtr(*i), &n, &max, &min);
    (*i)->unlock();

    N->setValue(n);
    Min->setText(QString::number(min));
    Max->setText(QString::number(max));
  }
}


void HistogramTab::updateButtons() {
  if (_realTimeAutoBin->isChecked()) {
    generateAutoBin();
  }

  Min->setEnabled(!_realTimeAutoBin->isChecked());
  Max->setEnabled(!_realTimeAutoBin->isChecked());
  N->setEnabled(!_realTimeAutoBin->isChecked());
  AutoBin->setEnabled(!_realTimeAutoBin->isChecked());
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
  qDebug() << "createNewDataObject" << endl;
  return 0;
}


ObjectPtr HistogramDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
