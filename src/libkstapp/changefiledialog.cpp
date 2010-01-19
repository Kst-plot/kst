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

#include "changefiledialog.h"

#include "datacollection.h"
#include "datavector.h"
#include "datamatrix.h"
#include "datascalar.h"
#include "datastring.h"
#include "vscalar.h"

#include "plotitem.h"

#include "objectstore.h"
#include "document.h"
#include "mainwindow.h"
#include "application.h"
#include "updatemanager.h"

#include <QDir>
#include <QMessageBox>
#include <QThreadPool>

namespace Kst {

ChangeFileDialog::ChangeFileDialog(QWidget *parent)
  : QDialog(parent), _dataSource(0), _requestID(0) {
   setupUi(this);

  if (MainWindow *mw = qobject_cast<MainWindow*>(parent)) {
    _store = mw->document()->objectStore();
  } else {
     // FIXME: we need the object store
    qFatal("ERROR: can't construct a ChangeFileDialog without the object store");
  }

  connect(_add, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
  connect(_remove, SIGNAL(clicked()), this, SLOT(removeButtonClicked()));
  connect(_removeAll, SIGNAL(clicked()), this, SLOT(removeAll()));
  connect(_addAll, SIGNAL(clicked()), this, SLOT(addAll()));

  connect(_changeFilePrimitiveList, SIGNAL(itemDoubleClicked ( QListWidgetItem * )), this, SLOT(availableDoubleClicked(QListWidgetItem *)));
  connect(_selectedFilePrimitiveList, SIGNAL(itemDoubleClicked ( QListWidgetItem * )), this, SLOT(selectedDoubleClicked(QListWidgetItem *)));

  connect(_allFromFile, SIGNAL(clicked()), this, SLOT(selectAllFromFile()));

  connect(_changeFilePrimitiveList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
  connect(_selectedFilePrimitiveList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));

  connect(_duplicateSelected, SIGNAL(toggled(bool)), _duplicateDependents, SLOT(setEnabled(bool)));
  connect(_dataFile, SIGNAL(changed(const QString &)), this, SLOT(fileNameChanged(const QString &)));

  connect(_buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
  connect(_buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(OKClicked()));
  connect(_buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));

  _dataFile->setFile(QDir::currentPath());
  updateButtons();
}


ChangeFileDialog::~ChangeFileDialog() {
}


void ChangeFileDialog::show() {
  updatePrimitiveList();
  QDialog::show();
}


void ChangeFileDialog::removeButtonClicked() {
  foreach (QListWidgetItem* item, _selectedFilePrimitiveList->selectedItems()) {
    _changeFilePrimitiveList->addItem(_selectedFilePrimitiveList->takeItem(_selectedFilePrimitiveList->row(item)));
  }

  _changeFilePrimitiveList->clearSelection();
  updateButtons();
}


void ChangeFileDialog::selectedDoubleClicked(QListWidgetItem * item) {
  if (item) {
    _changeFilePrimitiveList->addItem(_selectedFilePrimitiveList->takeItem(_selectedFilePrimitiveList->row(item)));
    _changeFilePrimitiveList->clearSelection();
    updateButtons();
  }
}


void ChangeFileDialog::addButtonClicked() {
  foreach (QListWidgetItem* item, _changeFilePrimitiveList->selectedItems()) {
    _selectedFilePrimitiveList->addItem(_changeFilePrimitiveList->takeItem(_changeFilePrimitiveList->row(item)));
  }
  _selectedFilePrimitiveList->clearSelection();
  updateButtons();
}


void ChangeFileDialog::availableDoubleClicked(QListWidgetItem * item) {
  if (item) {
    _selectedFilePrimitiveList->addItem(_changeFilePrimitiveList->takeItem(_changeFilePrimitiveList->row(item)));
    _selectedFilePrimitiveList->clearSelection();
    updateButtons();
  }
}


void ChangeFileDialog::sourceValid(QString filename, int requestID) {
  if (_requestID != requestID) {
    return;
  }
  _dataSource = DataSource::findOrLoadSource(_store, filename);
  updateButtons();
}


void ChangeFileDialog::fileNameChanged(const QString &file) {
  _dataSource = 0;
  updateButtons();

  _requestID += 1;
  ValidateDataSourceThread *validateDSThread = new ValidateDataSourceThread(file, _requestID);
  connect(validateDSThread, SIGNAL(dataSourceValid(QString, int)), this, SLOT(sourceValid(QString, int)));
  QThreadPool::globalInstance()->start(validateDSThread);
}


void ChangeFileDialog::updatePrimitiveList() {
  /* Make list of data primitives */
  DataVectorList dataVectorList = _store->getObjects<DataVector>();
  DataMatrixList dataMatrixList = _store->getObjects<DataMatrix>();
  DataScalarList dataScalarList = _store->getObjects<DataScalar>();
  DataStringList dataStringList = _store->getObjects<DataString>();
  VScalarList vScalarList = _store->getObjects<VScalar>();
  QStringList fileNameList;

  ObjectList<Primitive> primitives;

  foreach (DataVectorPtr V, dataVectorList) {
    PrimitivePtr P = kst_cast<Primitive>(V);
    primitives.append(P);
    fileNameList.append(V->filename());
  }
  foreach (DataMatrixPtr M, dataMatrixList) {
    PrimitivePtr P = kst_cast<Primitive>(M);
    primitives.append(P);
    fileNameList.append(M->filename());
  }
  foreach (DataScalarPtr S, dataScalarList) {
    PrimitivePtr P = kst_cast<Primitive>(S);
    primitives.append(P);
    fileNameList.append(S->filename());
  }
  foreach (DataStringPtr S, dataStringList) {
    PrimitivePtr P = kst_cast<Primitive>(S);
    primitives.append(P);
    fileNameList.append(S->filename());
  }
  foreach (VScalarPtr V, vScalarList) {
    PrimitivePtr P = kst_cast<Primitive>(V);
    primitives.append(P);
    fileNameList.append(V->filename());
  }

  // make sure all items in _changeFilePrimitiveList exist in the store; remove if they don't.
  for (int i_item = 0; i_item < _changeFilePrimitiveList->count(); i_item++) {
    bool exists=false;
    for (int i_primitive = 0; i_primitive<primitives.count(); i_primitive++) {
      if (primitives.at(i_primitive)->Name() == _changeFilePrimitiveList->item(i_item)->text()) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      QListWidgetItem *item = _changeFilePrimitiveList->takeItem(i_item);
      delete item;
    }
  }

  // make sure all items in _selectedFilePrimitiveList exist in the store; remove if they don't.
  for (int i_item = 0; i_item < _selectedFilePrimitiveList->count(); i_item++) {
    bool exists=false;
    for (int i_primitive = 0; i_primitive<primitives.count(); i_primitive++) {
      if (primitives.at(i_primitive)->Name() == _selectedFilePrimitiveList->item(i_item)->text()) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      QListWidgetItem *item = _selectedFilePrimitiveList->takeItem(i_item);
      delete item;
    }
  }

  // insert into _changeFilePrimitiveList all items in store not in one of the lists.
  for (int i_primitive = 0; i_primitive<primitives.count(); i_primitive++) {
    bool listed = false;
    for (int i_item = 0; i_item<_changeFilePrimitiveList->count(); i_item++) {
      if (primitives.at(i_primitive)->Name() == _changeFilePrimitiveList->item(i_item)->text()) {
        _changeFilePrimitiveList->item(i_item)->setToolTip(primitives.at(i_primitive)->descriptionTip());
        listed = true;
        break;
      }
    }
    for (int i_item = 0; i_item<_selectedFilePrimitiveList->count(); i_item++) {
      if (primitives.at(i_primitive)->Name() == _selectedFilePrimitiveList->item(i_item)->text()) {
        _selectedFilePrimitiveList->item(i_item)->setToolTip(primitives.at(i_primitive)->descriptionTip());
        listed = true;
        break;
      }
    }
    if (!listed) {
      QListWidgetItem *wi = new QListWidgetItem(primitives.at(i_primitive)->Name());
      _changeFilePrimitiveList->addItem(wi);
      wi->setToolTip(primitives.at(i_primitive)->descriptionTip());
    }
  }

  // fill _files
  QString currentFile = _files->currentText();
  _files->clear();

  for (QStringList::Iterator it = fileNameList.begin(); it != fileNameList.end(); ++it) {
    if (_files->findText(*it) == -1) {
      _files->addItem(*it);
    }
  }

  _allFromFile->setEnabled(_files->count() > 0);
  _files->setEnabled(_files->count() > 0);
  updateButtons();
}


void ChangeFileDialog::updateButtons() {
  bool valid = _selectedFilePrimitiveList->count() > 0 && _dataSource;
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(valid);
  _add->setEnabled(_changeFilePrimitiveList->selectedItems().count() > 0);
  _addAll->setEnabled(_changeFilePrimitiveList->count() > 0);
  _remove->setEnabled(_selectedFilePrimitiveList->selectedItems().count() > 0);
  _removeAll->setEnabled(_selectedFilePrimitiveList->count() > 0);
}


void ChangeFileDialog::addAll() {
  _changeFilePrimitiveList->selectAll();
  addButtonClicked();
}


void ChangeFileDialog::removeAll() {
  _selectedFilePrimitiveList->selectAll();
  removeButtonClicked();
}


void ChangeFileDialog::selectAllFromFile() {
  if (_files->count() <= 0) {
    return;
  }

  _changeFilePrimitiveList->clearSelection();

  for (int i = 0; i < _changeFilePrimitiveList->count(); i++) {
    if (DataVectorPtr vector = kst_cast<DataVector>(_store->retrieveObject(_changeFilePrimitiveList->item(i)->text()))) {
      _changeFilePrimitiveList->item(i)->setSelected(vector->filename() == _files->currentText());
    } else if (DataMatrixPtr matrix = kst_cast<DataMatrix>(_store->retrieveObject(_changeFilePrimitiveList->item(i)->text()))) {
      _changeFilePrimitiveList->item(i)->setSelected(matrix->filename() == _files->currentText());
    }
  }
  addButtonClicked();
}


void ChangeFileDialog::OKClicked() {
  apply();
  accept();
}


void ChangeFileDialog::apply() {
  Q_ASSERT(_store);
  if (!_dataSource->isValid() || _dataSource->isEmpty()) {
    QMessageBox::critical(this, tr("Kst"), tr("The file could not be loaded or contains no data."), QMessageBox::Ok);
    return;
  }

  QMap<RelationPtr, RelationPtr> duplicatedRelations;
  DataSourceList oldSources;
  QString invalidSources;
  int invalid = 0;

  QMap<DataVectorPtr, DataVectorPtr> duplicatedVectors;
  QMap<DataMatrixPtr, DataMatrixPtr> duplicatedMatrices;

  _selectedFilePrimitiveList->selectAll();
  QList<QListWidgetItem*> selectedItems = _selectedFilePrimitiveList->selectedItems();
  for (int i = 0; i < selectedItems.size(); ++i) {
    if (DataVectorPtr vector = kst_cast<DataVector>(_store->retrieveObject(selectedItems[i]->text()))) {
      vector->writeLock();
      _dataSource->readLock();
      bool valid = _dataSource->isValidField(vector->field());
      _dataSource->unlock();
      if (!valid) {
        if (invalid > 0) {
          invalidSources += ", ";
          }
        invalidSources = vector->field();
        ++invalid;
      } else {
        if (_duplicateSelected->isChecked()) {
          DataVectorPtr newVector = vector->makeDuplicate();

          newVector->writeLock();
          newVector->changeFile(_dataSource);
          newVector->registerChange();
          newVector->unlock();

          if (_duplicateDependents->isChecked()) {
             duplicateDependents(VectorPtr(vector), VectorPtr(newVector), duplicatedRelations);
          }
        } else {
          if (!oldSources.contains(vector->dataSource())) {
            oldSources << vector->dataSource();
          }
          vector->writeLock();
          vector->changeFile(_dataSource);
          vector->registerChange();
          vector->unlock();
        }
      }
      vector->unlock();
    } else if (DataMatrixPtr matrix = kst_cast<DataMatrix>(_store->retrieveObject(selectedItems[i]->text()))) {
      matrix->writeLock();
      _dataSource->readLock();
      bool valid = _dataSource->isValidMatrix(matrix->field());
      _dataSource->unlock();
      if (!valid) {
        if (invalid > 0) {
          invalidSources += ", ";
          }
        invalidSources = matrix->field();
        ++invalid;
      } else {
        if (_duplicateSelected->isChecked()) {
          DataMatrixPtr newMatrix = matrix->makeDuplicate();

          newMatrix->writeLock();
          newMatrix->changeFile(_dataSource);
          newMatrix->registerChange();
          newMatrix->unlock();

          if (_duplicateDependents->isChecked()) {
            duplicateDependents(MatrixPtr(matrix), MatrixPtr(newMatrix), duplicatedRelations);
          }
        } else {
          if (!oldSources.contains(matrix->dataSource())) {
            oldSources << matrix->dataSource();
          }
          matrix->writeLock();
          matrix->changeFile(_dataSource);
          matrix->registerChange();
          matrix->unlock();
        }
      }
      matrix->unlock();
    }
  }

  // Plot the items. (Do we need to doUpdates before this?)
  foreach (PlotItemInterface *plot, Data::self()->plotList()) {
    PlotItem* plotItem = static_cast<PlotItem*>(plot);
    foreach (PlotRenderItem* renderItem, plotItem->renderItems()) {
      for (QMap<RelationPtr, RelationPtr>::ConstIterator iter = duplicatedRelations.begin(); iter != duplicatedRelations.end(); ++iter) {
        if (renderItem->relationList().contains(kst_cast<Relation>(iter.key())) && !renderItem->relationList().contains(kst_cast<Relation>(iter.value()))) {
          renderItem->addRelation(kst_cast<Relation>(iter.value()));
        }
      }
    }
  }

  // clean up unused data sources
  for (DataSourceList::Iterator it = oldSources.begin(); it != oldSources.end(); ++it) {
    if ((*it)->getUsage() == 1) {
      _store->removeObject(*it);
    }
  }

  if (!invalidSources.isEmpty()) {
    if (invalid == 1) {
      QMessageBox::warning(this, tr("Kst"), tr("The following field is not defined for the requested file:\n%1").arg(invalidSources), QMessageBox::Ok);
    } else {
      QMessageBox::warning(this, tr("Kst"), tr("The following fields are not defined for the requested file:\n%1").arg(invalidSources), QMessageBox::Ok);
    }
  } else {
    updatePrimitiveList();
  }

  UpdateManager::self()->doUpdates(true);
  kstApp->mainWindow()->document()->setChanged(true);
}


void ChangeFileDialog::duplicateDependents(VectorPtr oldVector, VectorPtr newVector, QMap<RelationPtr, RelationPtr> &duplicatedRelations) {
  RelationList relations = _store->getObjects<Relation>();
  foreach(RelationPtr relation, relations) {
    if (relation->uses(oldVector)){
      RelationPtr newRelation = relation->makeDuplicate(duplicatedRelations);
      if (newRelation) {
        newRelation->replaceDependency(oldVector, newVector);
      }
    }
  }

  DataObjectList dataObjects = _store->getObjects<DataObject>();
  foreach(DataObjectPtr dataObject, dataObjects) {
    if (dataObject->uses(oldVector)){
      DataObjectPtr newObject = dataObject->makeDuplicate();
      if (newObject) {
        newObject->replaceDependency(oldVector, newVector);
        dataObject->duplicateDependents(newObject, duplicatedRelations);
      }
    }
  }
}


void ChangeFileDialog::duplicateDependents(MatrixPtr oldMatrix, MatrixPtr newMatrix, QMap<RelationPtr, RelationPtr> &duplicatedRelations) {
  RelationList relations = _store->getObjects<Relation>();
  foreach(RelationPtr relation, relations) {
    if (relation->uses(oldMatrix)){
      RelationPtr newRelation = relation->makeDuplicate(duplicatedRelations);
      if (newRelation) {
        newRelation->replaceDependency(oldMatrix, newMatrix);
      }
    }
  }

  DataObjectList dataObjects = _store->getObjects<DataObject>();
  foreach(DataObjectPtr dataObject, dataObjects) {
    if (dataObject->uses(oldMatrix)){
      DataObjectPtr newObject = dataObject->makeDuplicate();
      if (newObject) {
        newObject->replaceDependency(oldMatrix, newMatrix);
        dataObject->duplicateDependents(newObject, duplicatedRelations);
      }
    }
  }
}

}
// vim: ts=2 sw=2 et
