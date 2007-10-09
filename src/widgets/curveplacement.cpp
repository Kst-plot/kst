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

#include "curveplacement.h"

#include "plotiteminterface.h"

namespace Kst {

CurvePlacement::CurvePlacement(QWidget *parent)
  : QWidget(parent) {
  setupUi(this);

  connect(_existingPlot, SIGNAL(toggled(bool)), _plotList, SLOT(setEnabled(bool)));
}


CurvePlacement::~CurvePlacement() {
}


CurvePlacement::Place CurvePlacement::place() const {
  if (_noPlot->isChecked())
    return NoPlot;
  else if (_existingPlot->isChecked())
    return ExistingPlot;
  else
    return NewPlot;
}


void CurvePlacement::setPlace(CurvePlacement::Place place) {
  switch (place) {
  case NoPlot:
    _noPlot->setChecked(true);
    break;
  case ExistingPlot:
    _existingPlot->setChecked(true);
    break;
  case NewPlot:
    _newPlot->setChecked(true);
    break;
  default:
    break;
  }
}


PlotItemInterface *CurvePlacement::existingPlot() const {
  return qVariantValue<PlotItemInterface*>(_plotList->itemData(_plotList->currentIndex()));
}


void CurvePlacement::setExistingPlots(const QList<PlotItemInterface*> &existingPlots) {
  foreach (PlotItemInterface *plot, existingPlots) {
    _plotList->addItem(plot->plotName(), qVariantFromValue(plot));
  }
}


bool CurvePlacement::createLayout() const {
  return _createLayout->isChecked();
}


void CurvePlacement::setCreateLayout(bool createLayout) {
  _createLayout->setChecked(createLayout);
}


bool CurvePlacement::appendToLayout() const {
  return _appendToLayout->isChecked();
}


void CurvePlacement::setAppendToLayout(bool appendToLayout) {
  _appendToLayout->setChecked(appendToLayout);
}

}

// vim: ts=2 sw=2 et
