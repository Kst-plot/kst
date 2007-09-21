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

#include "gridtab.h"

namespace Kst {

GridTab::GridTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Grid"));

  connect(_showGrid, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_snapToGrid, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_gridHorSpacing, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_gridVerSpacing, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
}


GridTab::~GridTab() {
}


bool GridTab::showGrid() const {
  return _showGrid->isChecked();
}


void GridTab::setShowGrid(bool showGrid) {
  _showGrid->setChecked(showGrid);
}


bool GridTab::snapToGrid() const {
  return _snapToGrid->isChecked();
}


void GridTab::setSnapToGrid(bool snapToGrid) {
  _snapToGrid->setChecked(snapToGrid);
}


qreal GridTab::gridHorizontalSpacing() const {
  return _gridHorSpacing->value();
}


void GridTab::setGridHorizontalSpacing(qreal spacing) {
  _gridHorSpacing->setValue(spacing);
}


qreal GridTab::gridVerticalSpacing() const {
  return _gridVerSpacing->value();
}


void GridTab::setGridVerticalSpacing(qreal spacing) {
  _gridVerSpacing->setValue(spacing);
}

}

// vim: ts=2 sw=2 et
