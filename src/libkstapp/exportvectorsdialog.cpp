/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2010 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "exportvectorsdialog.h"
#include "dialogdefaults.h"

#include "datavector.h"
#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"

namespace Kst {

ExportVectorsDialog::ExportVectorsDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    MainWindow::setWidgetFlags(this);

     _saveLocationLabel->setBuddy(_saveLocation->_fileEdit);
     _saveLocation->setFile(_dialogDefaults->value("vectorexport/filename",QDir::currentPath()).toString());

    if (MainWindow *mw = qobject_cast<MainWindow*>(parent)) {
      _store = mw->document()->objectStore();
    } else {
       // FIXME: we need the object store
      qFatal("ERROR: can't construct a ExportVectorsDialog without the object store");
    }

    connect(_add, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
    connect(_remove, SIGNAL(clicked()), this, SLOT(removeButtonClicked()));
    connect(_removeAll, SIGNAL(clicked()), this, SLOT(removeAll()));
    connect(_addAll, SIGNAL(clicked()), this, SLOT(addAll()));

    connect(_changeVectorList, SIGNAL(itemDoubleClicked ( QListWidgetItem * )), this, SLOT(availableDoubleClicked(QListWidgetItem *)));
    connect(_selectedVectorList, SIGNAL(itemDoubleClicked ( QListWidgetItem * )), this, SLOT(selectedDoubleClicked(QListWidgetItem *)));

    connect(_changeVectorList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
    connect(_selectedVectorList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));

    connect(_buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(OKClicked()));
    connect(_buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));

}

ExportVectorsDialog::~ExportVectorsDialog()
{
}

void ExportVectorsDialog::show() {
  updateVectorList();
  QDialog::show();
}

void ExportVectorsDialog::updateVectorList() {
  VectorList allVectors = _store->getObjects<Vector>();

  QStringList vectorNameList;

  ObjectList<Vector> vectors;

  foreach (VectorPtr P, allVectors) {
    vectors.append(P);
    vectorNameList.append(P->Name());
  }

  // make sure all items in _changeVectorList exist in the store; remove if they don't.
  for (int i_item = 0; i_item < _changeVectorList->count(); i_item++) {
    bool exists=false;
    for (int i_vector = 0; i_vector<vectors.count(); i_vector++) {
      if (vectors.at(i_vector)->Name() == _changeVectorList->item(i_item)->text()) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      QListWidgetItem *item = _changeVectorList->takeItem(i_item);
      delete item;
    }
  }

  // make sure all items in _selectedVectorList exist in the store; remove if they don't.
  for (int i_item = 0; i_item < _selectedVectorList->count(); i_item++) {
    bool exists=false;
    for (int i_vector = 0; i_vector<vectors.count(); i_vector++) {
      if (vectors.at(i_vector)->Name() == _selectedVectorList->item(i_item)->text()) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      QListWidgetItem *item = _selectedVectorList->takeItem(i_item);
      delete item;
    }
  }

  // insert into _changeVectorList all items in store not in one of the lists.
  for (int i_vector = 0; i_vector<vectors.count(); i_vector++) {
    bool listed = false;
    for (int i_item = 0; i_item<_changeVectorList->count(); i_item++) {
      if (vectors.at(i_vector)->Name() == _changeVectorList->item(i_item)->text()) {
        _changeVectorList->item(i_item)->setToolTip(vectors.at(i_vector)->descriptionTip());
        listed = true;
        break;
      }
    }
    for (int i_item = 0; i_item<_selectedVectorList->count(); i_item++) {
      if (vectors.at(i_vector)->Name() == _selectedVectorList->item(i_item)->text()) {
        _selectedVectorList->item(i_item)->setToolTip(vectors.at(i_vector)->descriptionTip());
        listed = true;
        break;
      }
    }
    if (!listed) {
      QListWidgetItem *wi = new QListWidgetItem(vectors.at(i_vector)->Name());
      _changeVectorList->addItem(wi);
      wi->setToolTip(vectors.at(i_vector)->descriptionTip());
    }
  }

  updateButtons();
}


void ExportVectorsDialog::removeButtonClicked() {
  foreach (QListWidgetItem* item, _selectedVectorList->selectedItems()) {
    _changeVectorList->addItem(_selectedVectorList->takeItem(_selectedVectorList->row(item)));
  }

  _changeVectorList->clearSelection();
  updateButtons();
}


void ExportVectorsDialog::selectedDoubleClicked(QListWidgetItem * item) {
  if (item) {
    _changeVectorList->addItem(_selectedVectorList->takeItem(_selectedVectorList->row(item)));
    _changeVectorList->clearSelection();
    updateButtons();
  }
}


void ExportVectorsDialog::addButtonClicked() {
  foreach (QListWidgetItem* item, _changeVectorList->selectedItems()) {
    _selectedVectorList->addItem(_changeVectorList->takeItem(_changeVectorList->row(item)));
  }
  _selectedVectorList->clearSelection();
  updateButtons();
}


void ExportVectorsDialog::availableDoubleClicked(QListWidgetItem * item) {
  if (item) {
    _selectedVectorList->addItem(_changeVectorList->takeItem(_changeVectorList->row(item)));
    _selectedVectorList->clearSelection();
    updateButtons();
  }
}

void ExportVectorsDialog::addAll() {
  _changeVectorList->selectAll();
  addButtonClicked();
}


void ExportVectorsDialog::removeAll() {
  _selectedVectorList->selectAll();
  removeButtonClicked();
}

void ExportVectorsDialog::updateButtons() {
  bool valid = _selectedVectorList->count();

  QFileInfo qfi(_saveLocation->file());

  if (qfi.isFile()) {
    valid &= qfi.isWritable();
  }

  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(valid);

  _add->setEnabled(_changeVectorList->selectedItems().count() > 0);
  _addAll->setEnabled(_changeVectorList->count() > 0);
  _remove->setEnabled(_selectedVectorList->selectedItems().count() > 0);
  _removeAll->setEnabled(_selectedVectorList->count() > 0);

}

void ExportVectorsDialog::OKClicked() {
  if (apply()) {
    accept();
  }
}


bool ExportVectorsDialog::apply() {
  QFile file(_saveLocation->file());
  VectorList vectors;
  QList<int> lengths;

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    return false;

  QTextStream out(&file);

  out << "#";
  int count = _selectedVectorList->count();
  for (int i = 0; i<count; i++) {
    VectorPtr V = kst_cast<Vector>(_store->retrieveObject(_selectedVectorList->item(i)->text()));
    if (V) {
      vectors.append(V);
      out << " " << V->descriptiveName();
      lengths << V->length();
    }
  }

  out << "\n";

  int maxLength = 0;
  for (int i=0; i<lengths.size(); i++) {
    if (lengths.at(i)>maxLength) {
      maxLength = lengths.at(i);
    }
  }

  out.setRealNumberPrecision(14);

  int ncols = vectors.size();
  for (int row = 0; row < maxLength; row++) {
    for (int col = 0; col < ncols; col++) {
      out << " " << vectors.at(col)->interpolate(row, maxLength);
    }
    out << "\n";
  }

  out.flush();

  file.close();
  _dialogDefaults->setValue("vectorexport/filename", _saveLocation->file());

  return(true);
}

}
