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

  _single = true;

  _up->setIcon(QPixmap(":kst_uparrow.png"));
  _down->setIcon(QPixmap(":kst_downarrow.png"));
  _add->setIcon(QPixmap(":kst_rightarrow.png"));
  _remove->setIcon(QPixmap(":kst_leftarrow.png"));
  _up->setToolTip(i18n("Raise in list order: Alt+Up"));
  _down->setToolTip(i18n("Lower in list order: Alt+Down"));
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

  connect(_availableRelationList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
  connect(_displayedRelationList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
  connect(_availableRelationList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(addButtonClicked()));
  connect(_displayedRelationList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(removeButtonClicked()));

  connect(_autoContents, SIGNAL(stateChanged(int)), this, SLOT(updateActive()));
  connect(_autoContents, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_displayVertically, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));

  connect(_title, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_fontSize, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_bold, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_underline, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_italic, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_family, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));

  _displayedRelationList->setSortingEnabled(false);
  _availableRelationList->setSortingEnabled(false);
}


LegendTab::~LegendTab() {
}


void LegendTab::updateActive() {
  _contentGroupBox->setEnabled((!_autoContents->isChecked()) && _single);
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
  //_displayedRelationList->setFocus();

  for (int i=1; i<_displayedRelationList->count(); i++) {
    if (_displayedRelationList->item(i) && _displayedRelationList->item(i)->isSelected()) {
      QListWidgetItem *item = _displayedRelationList->takeItem(i);
      _displayedRelationList->insertItem(i-1, item);
      item->setSelected(true);
    }
  }
  updateButtons();
}


void LegendTab::downButtonClicked() {

  for (int i=_displayedRelationList->count()-2; i>=0; --i) {
    if (_displayedRelationList->item(i) && _displayedRelationList->item(i)->isSelected()) {
      QListWidgetItem *item = _displayedRelationList->takeItem(i);
      _displayedRelationList->insertItem(i+1, item);
      item->setSelected(true);
    }
  }
  updateButtons();

}


void LegendTab::setDisplayedRelations(const QStringList& displayedRelations, const QStringList& displayedRelationTips) {
  _displayedRelationList->clear();
  _displayedRelationList->addItems(displayedRelations);
  for (int i=0; i<_displayedRelationList->count(); i++) {
    _displayedRelationList->item(i)->setToolTip(displayedRelationTips.at(i));
  }
}


void LegendTab::setAvailableRelations(const QStringList& availableRelations, const QStringList& availableRelationTips) {
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

QFont LegendTab::font(QFont ref_font) const {
  QString family = (_family->currentIndex() == -1) ?
                   ref_font.family() : _family->currentFont().family();
  QFont font(family);
  font.setItalic(_italic->isChecked());
  font.setBold(_bold->isChecked());
  font.setUnderline(_underline->isChecked());
  return font;
}

//FIXME: handle tristate bool/italics, etc.
bool LegendTab::fontDirty() const {
  return true;
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

bool LegendTab::fontScaleDirty() const {
  return _fontSize->text().isEmpty();
}

bool LegendTab::autoContents() const {
  return _autoContents->isChecked();
}


void LegendTab::setAutoContents(const bool autoContents) {
  _autoContents->setChecked(autoContents);
}

bool LegendTab::autoContentsDirty() const {
  return _autoContents->checkState() != Qt::PartiallyChecked;
}

bool LegendTab::verticalDisplay() const {
  return _displayVertically->isChecked();
}


void LegendTab::setVerticalDisplay(const bool vertical) {
  _displayVertically->setChecked(vertical);
}

bool LegendTab::verticalDisplayDirty() const {
  return _displayVertically->checkState() != Qt::PartiallyChecked;
}


QString LegendTab::title() const {
  return _title->text();
}


void LegendTab::setTitle(const QString& title) {
  _title->setText(title);
}

bool LegendTab::titleDirty() const {
  return _title->text().isEmpty();
}

void LegendTab::clearTabValues() {
  _autoContents->setCheckState(Qt::PartiallyChecked);
  _fontSize->clear();
  _family->setCurrentIndex(-1);
  _displayVertically->setCheckState(Qt::PartiallyChecked);
}

void LegendTab::setSingle(bool single) {
  _single = single;
  updateActive();
  if (single) {
    _autoContents->setTristate(false);
    _displayVertically->setTristate(false);
  }
}

}

// vim: ts=2 sw=2 et
