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
#include "curve.h"
#include "colorsequence.h"

#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"
#include "application.h"

namespace Kst {

DifferentiateCurvesDialog::DifferentiateCurvesDialog(QWidget *parent)
  : QDialog(parent) {
   setupUi(this);

  if (MainWindow *mw = qobject_cast<MainWindow*>(parent)) {
    _store = mw->document()->objectStore();
  } else {
    // FIXME: we need the object store
    qFatal("ERROR: can't construct a DifferentiateCurvesDialog without the object store");
  }

  resetLists();

  connect(_buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
  connect(_buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(OKClicked()));
  connect(_buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));

  connect(_add, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
  connect(_remove, SIGNAL(clicked()), this, SLOT(removeButtonClicked()));
  connect(_up, SIGNAL(clicked()), this, SLOT(upButtonClicked()));
  connect(_down, SIGNAL(clicked()), this, SLOT(downButtonClicked()));
  connect(_availableListBox, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
  connect(_selectedListBox, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));

// TODO Icons required.
//  _up->setIcon(QPixmap(":kst_uparrow.png"));
  _up->setText("Up");
//  _down->setIcon(QPixmap(":kst_downarrow.png"));
  _down->setText("Down");
//  _add->setIcon(QPixmap(":kst_rightarrow.png"));
  _add->setText("Add");
//  _remove->setIcon(QPixmap(":kst_leftarrow.png"));
  _remove->setText("Remove");

  _maxLineWidth->setMaximum(LINEWIDTH_MAX);
}


DifferentiateCurvesDialog::~DifferentiateCurvesDialog() {
}


void DifferentiateCurvesDialog::show() {
  updateButtons();
  QDialog::show();
}


void DifferentiateCurvesDialog::resetLists() {
  _availableListBox->clear();
  _selectedListBox->clear();
  _availableListBox->addItem(tr("Line Color"));
  _availableListBox->addItem(tr("Point Style"));
  _availableListBox->addItem(tr("Line Style"));
  _availableListBox->addItem(tr("Line Width"));

  _maxLineWidth->setValue(1);
  _pointDensity->setCurrentIndex(0);
}


void DifferentiateCurvesDialog::updateButtons() {

  QList<QListWidgetItem *> selectedItems = _selectedListBox->selectedItems();
  QListWidgetItem *selectedItem = 0;

  if (selectedItems.count() > 0)
    selectedItem = selectedItems.first();

  _remove->setEnabled(selectedItems.count() > 0);

  _up->setEnabled(_selectedListBox->row(selectedItem) > 0);
  _down->setEnabled(_selectedListBox->row(selectedItem) >= 0 && _selectedListBox->row(selectedItem) < (int)_selectedListBox->count() - 1);

  _add->setEnabled(_availableListBox->selectedItems().count() > 0);

  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(_selectedListBox->selectedItems().count() > 0);
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


void DifferentiateCurvesDialog::OKClicked() {
  if (_buttonBox->button(QDialogButtonBox::Apply)->isEnabled()) {
    apply();
  }
  accept();
}


void DifferentiateCurvesDialog::apply() {
  bool lineColorOrder  = !_selectedListBox->findItems(tr("Line Color"), Qt::MatchExactly).empty();
  bool pointStyleOrder = !_selectedListBox->findItems(tr("Point Style"), Qt::MatchExactly).empty();
  bool lineStyleOrder  = !_selectedListBox->findItems(tr("Line Style"), Qt::MatchExactly).empty();
  bool lineWidthOrder  = !_selectedListBox->findItems(tr("Line Width"), Qt::MatchExactly).empty();

  int maxLineWidth = _maxLineWidth->value();
  int pointDensity = _pointDensity->currentIndex();

  int sequenceNum = 0;
  CurveList curveList = _store->getObjects<Curve>();
  for (CurveList::iterator curve_iter = curveList.begin(); curve_iter != curveList.end(); ++curve_iter)
  {
    CurvePtr curve = kst_cast<Curve>(*curve_iter);
    curve->writeLock();
    if (lineColorOrder) {
      curve->setColor(ColorSequence::entry(sequenceNum));
    }
    if (pointStyleOrder) {
      curve->setPointType(sequenceNum % KSTPOINT_MAXTYPE);
      curve->setHasPoints(true);
      curve->setPointDensity(pointDensity);
    }
    if (lineStyleOrder) {
      curve->setLineStyle(sequenceNum % LINESTYLE_MAXTYPE);
    }
    if (lineWidthOrder) {
      curve->setLineWidth((sequenceNum + 1) % maxLineWidth);
    }

    curve->processUpdate(curve);
    curve->unlock();
    ++sequenceNum;
  }
  resetLists();
  kstApp->mainWindow()->document()->setChanged(true);
}

}

// vim: ts=2 sw=2 et
