//
// C++ Interface: rangetab
//
// Description: 
//
//
// Author: Barth Netterfield <netterfield@physics.utoronto.ca>, (C) 2008

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "rangetab.h"

#include <QDebug>

namespace Kst {
RangeTab::RangeTab(PlotItem* plotItem, QWidget *parent) 
    : DialogTab(parent), _plotItem(plotItem) {
  setupUi(this);
  setTabTitle(tr("Range"));

  connect(_xAuto, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_xMean, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_xBorder, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_xSpike, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_xFixed, SIGNAL(toggled(bool)), this, SIGNAL(modified()));

  connect(_yAuto, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_yMean, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_yBorder, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_ySpike, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_yFixed, SIGNAL(toggled(bool)), this, SIGNAL(modified()));

  connect(_xMin, SIGNAL(textEdited(const QString &)), this, SIGNAL(modified()));
  connect(_xMax, SIGNAL(textEdited(const QString &)), this, SIGNAL(modified()));
  connect(_xRange, SIGNAL(textEdited(const QString &)), this, SIGNAL(modified()));

  connect(_yMin, SIGNAL(textEdited(const QString &)), this, SIGNAL(modified()));
  connect(_yMax, SIGNAL(textEdited(const QString &)), this, SIGNAL(modified()));
  connect(_yRange, SIGNAL(textEdited(const QString &)), this, SIGNAL(modified()));

  connect(this, SIGNAL(modified()), this, SLOT(updateButtons()));
}


RangeTab::~RangeTab() {
}


void RangeTab::setupRange() {
  Q_ASSERT(_plotItem);

  double xmax = _plotItem->xMax();
  double xmin = _plotItem->xMin();
  double ymax = _plotItem->yMax();
  double ymin = _plotItem->yMin();
  if (_plotItem->xAxis()->axisLog()) {
    xmax = exp10(xmax);
    xmin = exp10(xmin);
  }
  if (_plotItem->yAxis()->axisLog()) {
    ymax = exp10(ymax);
    ymin = exp10(ymin);
  }

  _xRange->setText(QString::number(fabs(xmax - xmin),'g', 13));
  _xMin->setText(QString::number(xmin,'g', 13));
  _xMax->setText(QString::number(xmax,'g', 13));

  _yRange->setText(QString::number(fabs(ymax - ymin),'g', 13));
  _yMin->setText(QString::number(ymin,'g', 13));
  _yMax->setText(QString::number(ymax,'g', 13));

  switch (_plotItem->xAxis()->axisZoomMode()) {
    case PlotAxis::Auto:
      _xAuto->setChecked(true);
      break;
    case PlotAxis::AutoBorder: 
      _xBorder->setChecked(true);
      break;
    case PlotAxis::FixedExpression:
      _xFixed->setChecked(true);
      break;
    case PlotAxis::SpikeInsensitive:
      _xSpike->setChecked(true);
      break;
    case PlotAxis::MeanCentered:
      _xMean->setChecked(true);
      break;
    default:
      break;
  }
  switch (_plotItem->yAxis()->axisZoomMode()) {
    case PlotAxis::Auto:
      _yAuto->setChecked(true);
      break;
    case PlotAxis::AutoBorder: 
      _yBorder->setChecked(true);
      break;
    case PlotAxis::FixedExpression:
      _yFixed->setChecked(true);
      break;
    case PlotAxis::SpikeInsensitive:
      _ySpike->setChecked(true);
      break;
    case PlotAxis::MeanCentered:
      _yMean->setChecked(true);
      break;
    default:
      break;
  }
}


void RangeTab::modified() {
  emit tabModified();
}


void RangeTab::clearTabValues() {

  _xHidden->setChecked(true);
  _yHidden->setChecked(true);
  _xRange->clear();
  _xMin->clear();
  _xMax->clear();

  _yRange->clear();
  _yMin->clear();
  _yMax->clear();
}


void RangeTab::updateButtons() {
  if (!_xHidden->isChecked()) {
    label->setEnabled(_xMean->isChecked());
    _xRange->setEnabled(_xMean->isChecked());

    label_2->setEnabled(_xFixed->isChecked());
    _xMin->setEnabled(_xFixed->isChecked());
    label_3->setEnabled(_xFixed->isChecked());
    _xMax->setEnabled(_xFixed->isChecked());
  } else {
    label->setEnabled(true);
    _xRange->setEnabled(true);

    label_2->setEnabled(true);
    _xMin->setEnabled(true);
    label_3->setEnabled(true);
    _xMax->setEnabled(true);
  }

  if (!_yHidden->isChecked()) {
    label_4->setEnabled(_yMean->isChecked());
    _yRange->setEnabled(_yMean->isChecked());

    label_5->setEnabled(_yFixed->isChecked());
    _yMin->setEnabled(_yFixed->isChecked());
    label_6->setEnabled(_yFixed->isChecked());
    _yMax->setEnabled(_yFixed->isChecked());
  } else {
    label_4->setEnabled(true);
    _yRange->setEnabled(true);

    label_5->setEnabled(true);
    _yMin->setEnabled(true);
    label_6->setEnabled(true);
    _yMax->setEnabled(true);
  }

}


bool RangeTab::xModeDirty() const {
  return (!_xHidden->isChecked());
}


bool RangeTab::xRangeDirty() const {
  return (!_xRange->text().isEmpty());
}


bool RangeTab::xMinDirty() const {
  return (!_xMin->text().isEmpty());
}


bool RangeTab::xMaxDirty() const {
  return (!_xMax->text().isEmpty());
}


bool RangeTab::yModeDirty() const {
  return (!_yHidden->isChecked());
}


bool RangeTab::yRangeDirty() const {
  return (!_yRange->text().isEmpty());
}


bool RangeTab::yMinDirty() const {
  return (!_yMin->text().isEmpty());
}


bool RangeTab::yMaxDirty() const {
  return (!_yMax->text().isEmpty());
}

}

