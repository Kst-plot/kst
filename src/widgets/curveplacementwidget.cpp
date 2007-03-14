/***************************************************************************
                   curveplacementwidget.cpp
                             -------------------
    begin                : 02/27/07
    copyright            : (C) 2007 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "curveplacementwidget.h"

#include <kstdatacollection.h>

CurvePlacementWidget::CurvePlacementWidget(QWidget *parent)
    : QWidget(parent) {
  setupUi(this);

  connect(_plotWindow, SIGNAL(activated(int)), this, SLOT(updatePlotList()));

  connect(_newWindow, SIGNAL(clicked()), this, SLOT(newWindow()));

  connect(_inPlot, SIGNAL(clicked()), this, SLOT(updateEnabled()));

  connect(_newPlot, SIGNAL(clicked()), this, SLOT(updateEnabled()));

  connect(_dontPlace, SIGNAL(clicked()), this, SLOT(updateEnabled()));

  connect(_plotWindow, SIGNAL(textChanged(const QString&)), this, SLOT(updateGrid()));

  connect(_reGrid, SIGNAL(clicked()), this, SLOT(updateEnabled()));
}


CurvePlacementWidget::~CurvePlacementWidget() {}


bool CurvePlacementWidget::existingPlot() {
  return _inPlot->isChecked();
}


bool CurvePlacementWidget::newPlot() {
  return _newPlot->isChecked();
}


void CurvePlacementWidget::setExistingPlot(bool existingPlot) {
  _inPlot->setChecked(existingPlot);
}


void CurvePlacementWidget::setNewPlot(bool newPlot) {
  _newPlot->setChecked(newPlot);
}


QString CurvePlacementWidget::plotName() {
  return _plotList->currentText();
}


int CurvePlacementWidget::columns() {
  return _plotColumns->value();
}


void CurvePlacementWidget::setCols(int c) {
  _plotColumns->setValue(c);
}


void CurvePlacementWidget::setCurrentPlot(const QString &p) {
  _plotList->setItemText(_plotList->currentIndex(), p);
}


void CurvePlacementWidget::newWindow() {
  KstData::self()->newWindow(this);
  update();
}


void CurvePlacementWidget::update() {
  _plotWindow->clear();
  QStringList windows = KstData::self()->windowList();
  for (QStringList::ConstIterator i = windows.begin(); i != windows.end(); ++i) {
    _plotWindow->addItem(*i);
  }
  QString cur = KstData::self()->currentWindow();
  if (!cur.isEmpty()) {
    _plotWindow->setItemText(_plotWindow->currentIndex(), cur);
  }

  updatePlotList();

  updateEnabled();

  updateGrid();
}


void CurvePlacementWidget::updatePlotList() {
  QString old;
  if (_plotList->count()) {
    old = _plotList->currentText();
  }

  QStringList plots = KstData::self()->plotList(_plotWindow->currentText());
  _plotList->clear();
  for (QStringList::ConstIterator i = plots.begin(); i != plots.end(); ++i) {
    _plotList->addItem(*i);
  }

  if (!old.isNull() && plots.contains(old)) {
    _plotList->setItemText(_plotList->currentIndex(), old);
  }
}


void CurvePlacementWidget::updateEnabled() {
  _plotWindow->setEnabled(_plotWindow->count() > 0);

  _inPlot->setEnabled(_plotList->count() > 0 );

  _plotList->setEnabled(_inPlot->isChecked());
  _reGrid->setEnabled(_newPlot->isChecked());
  _plotColumns->setEnabled(_newPlot->isChecked() && _reGrid->isChecked());
}


void CurvePlacementWidget::updateGrid() {
  int cols = KstData::self()->columns(_plotWindow->currentText());
  _reGrid->setChecked(cols > -1);
  if (cols > -1) {
    _plotColumns->setValue(cols);
  }
}


bool CurvePlacementWidget::reGrid() {
  return _reGrid->isChecked();
}

#include "curveplacementwidget.moc"

// vim: ts=2 sw=2 et
