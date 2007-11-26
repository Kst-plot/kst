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

#include "contenttab.h"

namespace Kst {

ContentTab::ContentTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);

  //TODO Need icons.
  _add->setText("Add");
  _remove->setText("Remove");
  _up->setText("Up");
  _down->setText("Down");

  connect(_add, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
  connect(_remove, SIGNAL(clicked()), this, SLOT(removeButtonClicked()));
  connect(_up, SIGNAL(clicked()), this, SLOT(upButtonClicked()));
  connect(_down, SIGNAL(clicked()), this, SLOT(downButtonClicked()));

  connect(_add, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_remove, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_up, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_down, SIGNAL(clicked()), this, SIGNAL(modified()));

  connect(_availableCurveList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
  connect(_displayedCurveList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
}


ContentTab::~ContentTab() {
}


void ContentTab::updateButtons() {

  QList<QListWidgetItem *> displayedItems = _displayedCurveList->selectedItems();
  QListWidgetItem *displayedItem = 0;

  if (displayedItems.count() > 0)
    displayedItem = displayedItems.first();

  _remove->setEnabled(displayedItems.count() > 0);

  _up->setEnabled(_displayedCurveList->row(displayedItem) > 0);
  _down->setEnabled(_displayedCurveList->row(displayedItem) >= 0 && _displayedCurveList->row(displayedItem) < (int)_displayedCurveList->count() - 1);

  _add->setEnabled(_availableCurveList->selectedItems().count() > 0);
}


void ContentTab::removeButtonClicked() {
  for (int i = 0; i < _displayedCurveList->count(); i++) {
    if (_displayedCurveList->item(i) && _displayedCurveList->item(i)->isSelected()) {
      _availableCurveList->addItem(_displayedCurveList->takeItem(i));
      _availableCurveList->clearSelection();
      _availableCurveList->item(_availableCurveList->count() - 1)->setSelected(true);
    }
  }
  updateButtons();
}


void ContentTab::addButtonClicked() {
  for (int i = 0; i < _availableCurveList->count(); i++) {
    if (_availableCurveList->item(i) && _availableCurveList->item(i)->isSelected()) {
      _displayedCurveList->addItem(_availableCurveList->takeItem(i));
      _displayedCurveList->clearSelection();
      _displayedCurveList->item(_displayedCurveList->count() - 1)->setSelected(true);
    }
  }
  updateButtons();
}


void ContentTab::upButtonClicked() {
  int i = _displayedCurveList->currentRow();
  if (i != -1) {
    QListWidgetItem *item = _displayedCurveList->takeItem(i);
    _displayedCurveList->insertItem(i-1, item);
    _displayedCurveList->clearSelection();
    item->setSelected(true);
    updateButtons();
  }
}


void ContentTab::downButtonClicked() {
  // move item down
  int i = _displayedCurveList->currentRow();
  if (i != -1) {
    QListWidgetItem *item = _displayedCurveList->takeItem(i);
    _displayedCurveList->insertItem(i+1, item);
    _displayedCurveList->clearSelection();
    item->setSelected(true);
    updateButtons();
  }
}


void ContentTab::setDisplayedCurves(QStringList displayedCurves) {
  _displayedCurveList->clear();
  _displayedCurveList->addItems(displayedCurves);
}


void ContentTab::setAvailableCurves(QStringList availableCurves) {
  _availableCurveList->clear();
  _availableCurveList->addItems(availableCurves);
}


QStringList ContentTab::displayedCurves() {
  QStringList curves;
  for (int i = 0; i < _displayedCurveList->count(); i++) {
    curves.append(_displayedCurveList->item(i)->text());
  }
  return curves;
}



}

// vim: ts=2 sw=2 et
