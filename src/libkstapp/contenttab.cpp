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

#include "contenttab.h"
#include "objectstore.h"

#include <qdebug.h>

namespace Kst {

ContentTab::ContentTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);

  _up->setIcon(QPixmap(":kst_uparrow.png"));
  _down->setIcon(QPixmap(":kst_downarrow.png"));
  _add->setIcon(QPixmap(":kst_rightarrow.png"));
  _remove->setIcon(QPixmap(":kst_leftarrow.png"));
  _up->setToolTip(i18n("Raise in plot order: Alt+Up"));
  _down->setToolTip(i18n("Lower in plot order: Alt+Down"));
  _add->setToolTip(i18n("Select: Alt+s"));
  _remove->setToolTip(i18n("Remove: Alt+r"));

  connect(_add, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
  connect(_remove, SIGNAL(clicked()), this, SLOT(removeButtonClicked()));
  connect(_up, SIGNAL(clicked()), this, SLOT(upButtonClicked()));
  connect(_down, SIGNAL(clicked()), this, SLOT(downButtonClicked()));

  connect(_add, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_remove, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_up, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_down, SIGNAL(clicked()), this, SIGNAL(modified()));

  connect(_availableRelationList, SIGNAL(itemDoubleClicked ( QListWidgetItem * )), this, SLOT(availableDoubleClicked(QListWidgetItem *)));
  connect(_displayedRelationList, SIGNAL(itemDoubleClicked ( QListWidgetItem * )), this, SLOT(displayedDoubleClicked(QListWidgetItem *)));

  connect(_availableRelationList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
  connect(_displayedRelationList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
}


ContentTab::~ContentTab() {
}


void ContentTab::updateButtons() {

  QList<QListWidgetItem *> displayedItems = _displayedRelationList->selectedItems();
  QListWidgetItem *displayedItem = 0;

  if (displayedItems.count() > 0)
    displayedItem = displayedItems.first();

  _remove->setEnabled(displayedItems.count() > 0);

  _up->setEnabled(_displayedRelationList->row(displayedItem) > 0);
  _down->setEnabled(_displayedRelationList->row(displayedItem) >= 0 && _displayedRelationList->row(displayedItem) < (int)_displayedRelationList->count() - 1);

  _add->setEnabled(_availableRelationList->selectedItems().count() > 0);
}


void ContentTab::removeButtonClicked() {
  foreach (QListWidgetItem* item, _displayedRelationList->selectedItems()) {
    _availableRelationList->addItem(_displayedRelationList->takeItem(_displayedRelationList->row(item)));
  }

  _availableRelationList->clearSelection();
  updateButtons();
}


void ContentTab::displayedDoubleClicked(QListWidgetItem * item) {
  if (item) {
    _availableRelationList->addItem(_displayedRelationList->takeItem(_displayedRelationList->row(item)));
    _availableRelationList->clearSelection();
    updateButtons();
  }
}


void ContentTab::addButtonClicked() {
  foreach (QListWidgetItem* item, _availableRelationList->selectedItems()) {
    _displayedRelationList->addItem(_availableRelationList->takeItem(_availableRelationList->row(item)));
  }
  _displayedRelationList->clearSelection();
  updateButtons();
}


void ContentTab::availableDoubleClicked(QListWidgetItem * item) {
  if (item) {
    _displayedRelationList->addItem(_availableRelationList->takeItem(_availableRelationList->row(item)));
    _displayedRelationList->clearSelection();
    updateButtons();
  }
}


void ContentTab::upButtonClicked() {
  _displayedRelationList->setFocus();

  int i = _displayedRelationList->currentRow();
  if (i != -1) {
    QListWidgetItem *item = _displayedRelationList->takeItem(i);
    _displayedRelationList->insertItem(i-1, item);
    _displayedRelationList->clearSelection();
    _displayedRelationList->setCurrentItem(item);
    //item->setSelected(true);
    updateButtons();
  }
}


void ContentTab::downButtonClicked() {
  _displayedRelationList->setFocus();
  // move item down
  int i = _displayedRelationList->currentRow();
  if (i != -1) {
    QListWidgetItem *item = _displayedRelationList->takeItem(i);
    _displayedRelationList->insertItem(i+1, item);
    _displayedRelationList->clearSelection();
    _displayedRelationList->setCurrentItem(item);
    //item->setSelected(true);
    updateButtons();
  }
}


void ContentTab::setDisplayedRelations(QStringList displayedRelations, QStringList displayedRelationTips) {
  _displayedRelationList->clear();
  _displayedRelationList->addItems(displayedRelations);
  for (int i=0; i<_displayedRelationList->count(); i++) {
    _displayedRelationList->item(i)->setToolTip(displayedRelationTips.at(i));
  }
}


void ContentTab::setAvailableRelations(QStringList availableRelations, QStringList availableRelationTips) {
  _availableRelationList->clear();
  _availableRelationList->addItems(availableRelations);
  for (int i=0; i<_availableRelationList->count(); i++) {
    _availableRelationList->item(i)->setToolTip(availableRelationTips.at(i));
  }
}


QStringList ContentTab::displayedRelations() {
  QStringList relations;
  for (int i = 0; i < _displayedRelationList->count(); i++) {
    relations.append(_displayedRelationList->item(i)->text());
  }
  return relations;
}

}

// vim: ts=2 sw=2 et
