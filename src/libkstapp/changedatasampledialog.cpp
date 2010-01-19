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

#include "changedatasampledialog.h"

#include "datacollection.h"
#include "datavector.h"
#include "objectstore.h"
#include "document.h"
#include "mainwindow.h"
#include "application.h"
#include "dialogdefaults.h"
#include "updatemanager.h"

namespace Kst {

ChangeDataSampleDialog::ChangeDataSampleDialog(QWidget *parent)
  : QDialog(parent) {
   setupUi(this);

  if (MainWindow *mw = qobject_cast<MainWindow*>(parent)) {
    _store = mw->document()->objectStore();
  } else {
    // FIXME: we need the object store
    qFatal("ERROR: can't construct a ChangeDataSampleDialog without the object store");
  }

  connect(_add, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
  connect(_remove, SIGNAL(clicked()), this, SLOT(removeButtonClicked()));
  connect(_removeAll, SIGNAL(clicked()), this, SLOT(removeAll()));
  connect(_addAll, SIGNAL(clicked()), this, SLOT(addAll()));

  connect(_vectorList, SIGNAL(itemDoubleClicked ( QListWidgetItem * )), this, SLOT(availableDoubleClicked(QListWidgetItem *)));
  connect(_selectedVectorList, SIGNAL(itemDoubleClicked ( QListWidgetItem * )), this, SLOT(selectedDoubleClicked(QListWidgetItem *)));

  connect(_vectorList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
  connect(_selectedVectorList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));

  connect(_dataRange, SIGNAL(modified()), this, SLOT(modified()));

  connect(_buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
  connect(_buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(OKClicked()));
  connect(_buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));

  initializeEntries();
  updateButtons();
}


ChangeDataSampleDialog::~ChangeDataSampleDialog() {
}


void ChangeDataSampleDialog::show() {
  updateCurveListDialog();
  updateButtons();
  QDialog::show();
}


void ChangeDataSampleDialog::removeButtonClicked() {
  foreach (QListWidgetItem* item, _selectedVectorList->selectedItems()) {
    _vectorList->addItem(_selectedVectorList->takeItem(_selectedVectorList->row(item)));
  }

  _vectorList->clearSelection();
  updateButtons();
}


void ChangeDataSampleDialog::selectedDoubleClicked(QListWidgetItem * item) {
  if (item) {
    _vectorList->addItem(_selectedVectorList->takeItem(_selectedVectorList->row(item)));
    _vectorList->clearSelection();
    updateButtons();
  }
}


void ChangeDataSampleDialog::addButtonClicked() {
  foreach (QListWidgetItem* item, _vectorList->selectedItems()) {
    _selectedVectorList->addItem(_vectorList->takeItem(_vectorList->row(item)));
  }
  _selectedVectorList->clearSelection();
  updateButtons();
}


void ChangeDataSampleDialog::availableDoubleClicked(QListWidgetItem * item) {
  if (item) {
    _selectedVectorList->addItem(_vectorList->takeItem(_vectorList->row(item)));
    _selectedVectorList->clearSelection();
    updateButtons();
  }
}


void ChangeDataSampleDialog::updateButtons() {
  bool valid = _selectedVectorList->count() > 0;
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(valid);
  _add->setEnabled(_vectorList->selectedItems().count() > 0);
  _addAll->setEnabled(_vectorList->count() > 0);
  _remove->setEnabled(_selectedVectorList->selectedItems().count() > 0);
  _removeAll->setEnabled(_selectedVectorList->count() > 0);
}


void ChangeDataSampleDialog::modified() {
  updateButtons();
}

void ChangeDataSampleDialog::updateCurveListDialog() {

  DataVectorList dataVectors = _store->getObjects<DataVector>();
  _vectorList->blockSignals(true);

  _vectorList->clearSelection();
  _selectedVectorList->clearSelection();

  // make sure all items in _vectorList exist in the store; remove if they don't.
  for (int i_item = 0; i_item < _vectorList->count(); i_item++) {
    bool exists=false;
    for (int i_vector = 0; i_vector<dataVectors.count(); i_vector++) {
      if (dataVectors.at(i_vector)->Name() == _vectorList->item(i_item)->text()) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      QListWidgetItem *item = _vectorList->takeItem(i_item);
      delete item;
    }
  }

  // make sure all items in _selectedVectorList exist in the store; remove if they don't.
  for (int i_item = 0; i_item<_selectedVectorList->count(); i_item++) {
    bool exists=false;
    for (int i_vector = 0; i_vector<dataVectors.count(); i_vector++) {
      if (dataVectors.at(i_vector)->Name() == _selectedVectorList->item(i_item)->text()) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      QListWidgetItem *item = _selectedVectorList->takeItem(i_item);
      delete item;
    }
  }

  // insert into _vectorList all items in store not in one of the lists.
  for (int i_vector = 0; i_vector<dataVectors.count(); i_vector++) {
    bool listed = false;
    for (int i_item = 0; i_item<_selectedVectorList->count(); i_item++) {
      if (dataVectors.at(i_vector)->Name() == _selectedVectorList->item(i_item)->text()) {
        _selectedVectorList->item(i_item)->setToolTip(dataVectors.at(i_vector)->descriptionTip());
        listed = true;
        break;
      }
    }
    for (int i_item = 0; i_item<_vectorList->count(); i_item++) {
      if (dataVectors.at(i_vector)->Name() == _vectorList->item(i_item)->text()) {
        _vectorList->item(i_item)->setToolTip(dataVectors.at(i_vector)->descriptionTip());
        listed = true;
        break;
      }
    }
    if (!listed) {
      QListWidgetItem *wi = new QListWidgetItem(dataVectors.at(i_vector)->Name());
      _vectorList->addItem(wi);
      wi->setToolTip(dataVectors.at(i_vector)->descriptionTip());
    }
  }
  _vectorList->blockSignals(false);
}


void ChangeDataSampleDialog::addAll() {
  _vectorList->selectAll();
  addButtonClicked();
}


void ChangeDataSampleDialog::removeAll() {
  _selectedVectorList->selectAll();
  removeButtonClicked();
}


void ChangeDataSampleDialog::initializeEntries() {
  _dataRange->setCountFromEnd(_dialogDefaults->value("vector/countFromEnd",false).toBool());
  _dataRange->setStart(_dialogDefaults->value("vector/start", 0).toInt());
  _dataRange->setReadToEnd(_dialogDefaults->value("vector/readToEnd",true).toBool());
  _dataRange->setRange(_dialogDefaults->value("vector/range", 1).toInt());
  _dataRange->setSkip(_dialogDefaults->value("vector/skip", 0).toInt());
  _dataRange->setDoSkip(_dialogDefaults->value("vector/doSkip", false).toBool());
  _dataRange->setDoFilter(_dialogDefaults->value("vector/doAve",false).toBool());
}


void ChangeDataSampleDialog::updateDefaults(QListWidgetItem* item) {
  if (!item) {
    return;
  }

  if (DataVectorPtr vector = kst_cast<DataVector>(_store->retrieveObject(item->text()))) {
    vector->readLock();

    _dataRange->setCountFromEnd(vector->countFromEOF());
    _dataRange->setStart(vector->countFromEOF() ? 0 : vector->reqStartFrame());
    _dataRange->setReadToEnd(vector->readToEOF());
    _dataRange->setRange(vector->readToEOF() ? 0 : vector->reqNumFrames());
    _dataRange->setSkip(vector->skip());
    _dataRange->setDoSkip(vector->doSkip());
    _dataRange->setDoFilter(vector->doAve());

    vector->unlock();
  }
}


void ChangeDataSampleDialog::OKClicked() {
  apply();
  accept();
}


void ChangeDataSampleDialog::apply() {
  _selectedVectorList->selectAll();
  QList<QListWidgetItem*> selectedItems = _selectedVectorList->selectedItems();
  for (int i = 0; i < selectedItems.size(); ++i) {
    if (DataVectorPtr vector = kst_cast<DataVector>(_store->retrieveObject(selectedItems.at(i)->text()))) {
      vector->writeLock();
      vector->changeFrames( (_dataRange->countFromEnd() ? -1 : int(_dataRange->start())),
                            (_dataRange->readToEnd() ? -1 : int(_dataRange->range())),
                            _dataRange->skip(),
                            _dataRange->doSkip(),
                            _dataRange->doFilter());
      vector->registerChange();
      vector->unlock();
    }
  }

  UpdateManager::self()->doUpdates(true);

  _dialogDefaults->setValue("vector/range", _dataRange->range());
  _dialogDefaults->setValue("vector/start", _dataRange->start());
  _dialogDefaults->setValue("vector/countFromEnd", _dataRange->countFromEnd());
  _dialogDefaults->setValue("vector/readToEnd", _dataRange->readToEnd());
  _dialogDefaults->setValue("vector/skip", _dataRange->skip());
  _dialogDefaults->setValue("vector/doSkip", _dataRange->doSkip());
  _dialogDefaults->setValue("vector/doAve", _dataRange->doFilter());

  updateCurveListDialog();
  kstApp->mainWindow()->document()->setChanged(true);
}


}

// vim: ts=2 sw=2 et
