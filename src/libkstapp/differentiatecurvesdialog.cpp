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

#include "differentiatecurvesdialog.h"
#include "linestyle.h"

namespace Kst {

DifferentiateCurvesDialog::DifferentiateCurvesDialog(QWidget *parent)
  : QDialog(parent) {
   setupUi(this);

  _availableListBox->clear();
  _selectedListBox->clear();
  _availableListBox->addItem(tr("Line Color"));
  _availableListBox->addItem(tr("Point Style"));
  _availableListBox->addItem(tr("Line Style"));
  _availableListBox->addItem(tr("Line Width"));

  connect(Cancel, SIGNAL(clicked()), this, SLOT(close()));
  connect(OK, SIGNAL(clicked()), this, SLOT(close()));

  connect(_add, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
  connect(_remove, SIGNAL(clicked()), this, SLOT(removeButtonClicked()));
  connect(_up, SIGNAL(clicked()), this, SLOT(upButtonClicked()));
  connect(_down, SIGNAL(clicked()), this, SLOT(downButtonClicked()));
  connect(_availableListBox, SIGNAL(currentRowChanged(int)), this, SLOT(updateButtons()));
  connect(_selectedListBox, SIGNAL(currentRowChanged(int)), this, SLOT(updateButtons()));

// TODO Icons required.
//  _up->setIcon(QPixmap(":kst_uparrow.png"));
  _up->setText("Up");
  _up->setEnabled(false);
//  _down->setIcon(QPixmap(":kst_downarrow.png"));
  _down->setText("Down");
  _down->setEnabled(false);
//  _add->setIcon(QPixmap(":kst_rightarrow.png"));
  _add->setText("Add");
  _add->setEnabled(false);
//  _remove->setIcon(QPixmap(":kst_leftarrow.png"));
  _remove->setText("Remove");
  _remove->setEnabled(false);

  _maxLineWidth->setMaximum(LINEWIDTH_MAX);

}


DifferentiateCurvesDialog::~DifferentiateCurvesDialog() {
}


void DifferentiateCurvesDialog::updateButtons() {

  QList<QListWidgetItem *> selectedItems = _selectedListBox->selectedItems();
  QListWidgetItem *selectedItem = 0;

  if (selectedItems.count() > 0)
    selectedItem = selectedItems.first();

  if (selectedItem) {
    _remove->setEnabled(true); 
  } else {
    _remove->setEnabled(false);
  }

  _up->setEnabled(_selectedListBox->row(selectedItem) > 0);
  _down->setEnabled(_selectedListBox->row(selectedItem) >= 0 && _selectedListBox->row(selectedItem) < (int)_selectedListBox->count() - 1);


  selectedItems = _availableListBox->selectedItems();
  selectedItem = 0;

  if (selectedItems.count() > 0) {
    selectedItem = selectedItems.first();
  }

   if (selectedItem) {
    _add->setEnabled(true); 
  } else {
   _add->setEnabled(false);
  }

}


void DifferentiateCurvesDialog::removeButtonClicked() {
  for (int i = 0; i < _selectedListBox->count(); i++) {
    if (_selectedListBox->item(i) && _selectedListBox->item(i)->isSelected()) {
      _availableListBox->addItem(_selectedListBox->takeItem(i));
      _availableListBox->clearSelection();
      _availableListBox->item(_availableListBox->count() - 1)->setSelected(true);
    }
  }
  updateButtons();
}


void DifferentiateCurvesDialog::addButtonClicked() {
  for (int i = 0; i < _availableListBox->count(); i++) {
    if (_availableListBox->item(i) && _availableListBox->item(i)->isSelected()) {
      _selectedListBox->addItem(_availableListBox->takeItem(i));
      _selectedListBox->clearSelection();
      _selectedListBox->item(_selectedListBox->count() - 1)->setSelected(true);
    }
  }
  updateButtons();
}


void DifferentiateCurvesDialog::upButtonClicked() {
  int i = _selectedListBox->currentRow();
  if (i != -1) {
    QListWidgetItem *item = _selectedListBox->takeItem(i);
    _selectedListBox->insertItem(i-1, item);
    _selectedListBox->clearSelection();
    item->setSelected(true);
    updateButtons();
  }
}


void DifferentiateCurvesDialog::downButtonClicked() {
  // move item down
  int i = _selectedListBox->currentRow();
  if (i != -1) {
    QListWidgetItem *item = _selectedListBox->takeItem(i);
    _selectedListBox->insertItem(i+1, item);
    _selectedListBox->clearSelection();
    item->setSelected(true);
    updateButtons();
  }
}

}

// vim: ts=2 sw=2 et
