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
#include <QDebug>
#include "rangetab.h"

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
}

RangeTab::~RangeTab() {
}

void RangeTab::setupRange() {
  Q_ASSERT(_plotItem);

  _xRange->setText(QString::number(fabs(_plotItem->xMax() - _plotItem->xMin())));
  _xMin->setText(QString::number(_plotItem->xMin()));
  _xMax->setText(QString::number(_plotItem->xMax()));

  _yRange->setText(QString::number(fabs(_plotItem->yMax() - _plotItem->yMin())));
  _yMin->setText(QString::number(_plotItem->yMin()));
  _yMax->setText(QString::number(_plotItem->yMax()));

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

}