/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "legendtab.h"
#include "objectstore.h"

#include <qdebug.h>

namespace Kst {

LegendTab::LegendTab(QWidget *parent)
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

  connect(_availableRelationList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
  connect(_displayedRelationList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));

  connect(_autoContents, SIGNAL(stateChanged(int)), this, SLOT(updateActive()));
  connect(_autoContents, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_displayVertically, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));

  connect(_title, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_fontSize, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_bold, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_underline, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_italic, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_family, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
}


LegendTab::~LegendTab() {
}


void LegendTab::updateActive() {
  _contentGroupBox->setEnabled(!_autoContents->isChecked());
}


void LegendTab::updateButtons() {

  QList<QListWidgetItem *> displayedItems = _displayedRelationList->selectedItems();
  QListWidgetItem *displayedItem = 0;

  if (displayedItems.count() > 0)
    displayedItem = displayedItems.first();

  _remove->setEnabled(displayedItems.count() > 0);

  _up->setEnabled(_displayedRelationList->row(displayedItem) > 0);
  _down->setEnabled(_displayedRelationList->row(displayedItem) >= 0 && _displayedRelationList->row(displayedItem) < (int)_displayedRelationList->count() - 1);

  _add->setEnabled(_availableRelationList->selectedItems().count() > 0);
}


void LegendTab::removeButtonClicked() {
  for (int i = 0; i < _displayedRelationList->count(); i++) {
    if (_displayedRelationList->item(i) && _displayedRelationList->item(i)->isSelected()) {
      _availableRelationList->addItem(_displayedRelationList->takeItem(i));
      _availableRelationList->clearSelection();
      _availableRelationList->item(_availableRelationList->count() - 1)->setSelected(true);
    }
  }
  updateButtons();
}


void LegendTab::addButtonClicked() {
  for (int i = 0; i < _availableRelationList->count(); i++) {
    if (_availableRelationList->item(i) && _availableRelationList->item(i)->isSelected()) {
      _displayedRelationList->addItem(_availableRelationList->takeItem(i));
      _displayedRelationList->clearSelection();
      _displayedRelationList->item(_displayedRelationList->count() - 1)->setSelected(true);
    }
  }
  updateButtons();
}


void LegendTab::upButtonClicked() {
  int i = _displayedRelationList->currentRow();
  if (i != -1) {
    QListWidgetItem *item = _displayedRelationList->takeItem(i);
    _displayedRelationList->insertItem(i-1, item);
    _displayedRelationList->clearSelection();
    item->setSelected(true);
    updateButtons();
  }
}


void LegendTab::downButtonClicked() {
  // move item down
  int i = _displayedRelationList->currentRow();
  if (i != -1) {
    QListWidgetItem *item = _displayedRelationList->takeItem(i);
    _displayedRelationList->insertItem(i+1, item);
    _displayedRelationList->clearSelection();
    item->setSelected(true);
    updateButtons();
  }
}


void LegendTab::setDisplayedRelations(QStringList displayedRelations, QStringList displayedRelationTips) {
  _displayedRelationList->clear();
  _displayedRelationList->addItems(displayedRelations);
  for (int i=0; i<_displayedRelationList->count(); i++) {
    _displayedRelationList->item(i)->setToolTip(displayedRelationTips.at(i));
  }
}


void LegendTab::setAvailableRelations(QStringList availableRelations, QStringList availableRelationTips) {
  _availableRelationList->clear();
  _availableRelationList->addItems(availableRelations);
  for (int i=0; i<_availableRelationList->count(); i++) {
    _availableRelationList->item(i)->setToolTip(availableRelationTips.at(i));
  }
}


QStringList LegendTab::displayedRelations() {
  QStringList relations;
  for (int i = 0; i < _displayedRelationList->count(); i++) {
    relations.append(_displayedRelationList->item(i)->text());
  }
  return relations;
}

QFont LegendTab::font() const {
  QFont font(_family->currentFont());
  font.setItalic(_italic->isChecked());
  font.setBold(_bold->isChecked());
  font.setUnderline(_underline->isChecked());
  return font;
}


void LegendTab::setFont(const QFont &font) {
  _family->setCurrentFont(font);
  _bold->setChecked(font.bold());
  _underline->setChecked(font.underline());
  _italic->setChecked(font.italic());
}


qreal LegendTab::fontScale() const {
  return _fontSize->value();
}


void LegendTab::setFontScale(const qreal scale) {
  _fontSize->setValue(scale);
}


bool LegendTab::autoContents() const {
  return _autoContents->isChecked();
}


void LegendTab::setAutoContents(const bool autoContents) {
  _autoContents->setChecked(autoContents);
}


bool LegendTab::verticalDisplay() const {
  return _displayVertically->isChecked();
}


void LegendTab::setVerticalDisplay(const bool vertical) {
  _displayVertically->setChecked(vertical);
}


QString LegendTab::title() const {
  return _title->text();
}


void LegendTab::setTitle(const QString& title) {
  _title->setText(title);
}

}

// vim: ts=2 sw=2 et
