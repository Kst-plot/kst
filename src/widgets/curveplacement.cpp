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

#include "curveplacement.h"

#include "plotiteminterface.h"

namespace Kst {

CurvePlacement::CurvePlacement(QWidget *parent)
  : QWidget(parent) {
  setupUi(this);

  connect(_existingPlot, SIGNAL(toggled(bool)), _plotList, SLOT(setEnabled(bool)));
  connect(_newPlot, SIGNAL(toggled(bool)), this, SLOT(updateButtons()));
  connect(_customGrid, SIGNAL(toggled(bool)), this, SLOT(updateButtons()));
}


CurvePlacement::~CurvePlacement() {
}


void CurvePlacement::updateButtons() {
  _layoutGroup->setEnabled(_newPlot->isChecked());
  _gridColumns->setEnabled(_customGrid->isChecked());
  _gridColumnsLabel->setEnabled(_customGrid->isChecked());
  _newTab->setEnabled(_newPlot->isChecked());
  if (!_newPlot->isChecked()) {
    _newTab->setChecked(false);
  }
}


CurvePlacement::Place CurvePlacement::place() const {
  if ((!isVisible()) || _noPlot->isChecked())
    return NoPlot;
  else if (_existingPlot->isChecked())
    return ExistingPlot;
  else if (_newTab->isChecked())
    return NewPlotNewTab;
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
  case NewPlotNewTab:
    _newPlot->setChecked(true);
    _newTab->setChecked(true);
  default:
    break;
  }
}

bool CurvePlacement::scaleFonts() const {
  return _scaleFonts->isChecked();
}

CurvePlacement::Layout CurvePlacement::layout() const {
  if (_autoLayout->isChecked())
    return Auto;
  else if (_customGrid->isChecked())
    return Custom;
  else
    return Protect;
}


void CurvePlacement::setLayout(CurvePlacement::Layout layout) {
  switch (layout) {
  case Auto:
    _autoLayout->setChecked(true);
    break;
  case Custom:
    _customGrid->setChecked(true);
    break;
  case Protect:
    _protectLayout->setChecked(true);
    break;
  default:
    break;
  }
}

PlotItemInterface *CurvePlacement::existingPlot() const {
  return qVariantValue<PlotItemInterface*>(_plotList->itemData(_plotList->currentIndex()));
}


void CurvePlacement::setExistingPlots(const QList<PlotItemInterface*> &existingPlots) {
  _plots.clear();

  _plots.append(existingPlots);

  updatePlotListCombo();
}

void CurvePlacement::updatePlotListCombo() {

  int current_index=-1;
  if (_plotList->count() > 0) {
    current_index = _plotList->currentIndex();
  }
  _plotList->clear();
  foreach (PlotItemInterface *plot, _plots) {
    _plotList->addItem(plot->plotSizeLimitedName(_plotList), qVariantFromValue(plot));
  }

  if ((current_index>0) && (current_index<_plotList->count())) {
    _plotList->setCurrentIndex(current_index);
  }
}

bool CurvePlacement::event(QEvent * event) {
  if ((event->type() == QEvent::Resize)) {
    updatePlotListCombo();
  }
  return QWidget::event(event);
}

void CurvePlacement::setCurrentPlot(const PlotItemInterface *currentPlot) {
  if (currentPlot) {
    int n = _plotList->count();
    for (int i=0; i<n; ++i ) {
      if (qVariantValue<PlotItemInterface*>(_plotList->itemData(i))->plotName() == currentPlot->plotName()) {
        _plotList->setCurrentIndex(i);
        return;
      }
    }
  }
}

int CurvePlacement::gridColumns() const {
  return _gridColumns->value();
}


void CurvePlacement::setGridColumns(int columns) {
  _gridColumns->setValue(columns);
}

}

// vim: ts=2 sw=2 et
