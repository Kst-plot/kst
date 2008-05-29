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

#include "plotitem.h"

#include "objectstore.h"
#include "document.h"
#include "mainwindow.h"

#include <QDir>
#include <QMessageBox>

namespace Kst {

ChangeFileDialog::ChangeFileDialog(QWidget *parent)
  : QDialog(parent) {
   setupUi(this);

  if (MainWindow *mw = qobject_cast<MainWindow*>(parent)) {
    _store = mw->document()->objectStore();
  } else {
     // FIXME: we need the object store
    qFatal("ERROR: can't construct a ChangeDataSampleDialog without the object store");
  }

  // TODO Need Icon.
  _clearFilter->setText("Clear Filter");

  connect(_buttonBox->button(QDialogButtonBox::Cancel), SIGNAL(clicked()), this, SLOT(reject()));
  connect(_buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(OKClicked()));
  connect(_buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));

  connect(_changeFileClear, SIGNAL(clicked()), _filter, SLOT(clear()));
  connect(_changeFileClear, SIGNAL(clicked()), _changeFilePrimitiveList, SLOT(clearSelection()));
  connect(_changeFileSelectAll, SIGNAL(clicked()), _filter, SLOT(clear()));
  connect(_changeFileSelectAll, SIGNAL(clicked()), this, SLOT(selectAll()));
  connect(_clearFilter, SIGNAL(clicked()), _filter, SLOT(clear()));
  connect(_clearFilter, SIGNAL(clicked()), _changeFilePrimitiveList, SLOT(clearSelection()));

  connect(_allFromFile, SIGNAL(clicked()), _filter, SLOT(clear()));
  connect(_allFromFile, SIGNAL(clicked()), this, SLOT(selectAllFromFile()));

  connect(_duplicateSelected, SIGNAL(toggled(bool)), _duplicateDependents, SLOT(setEnabled(bool)));

  connect(_filter, SIGNAL(textChanged(const QString&)), this, SLOT(updateSelection(const QString&)));
  connect(_changeFilePrimitiveList, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));

  _dataFile->setFile(QDir::currentPath());
  updateButtons();
}


ChangeFileDialog::~ChangeFileDialog() {
}


void ChangeFileDialog::exec() {
  updatePrimitiveList();
  QDialog::exec();
}


void ChangeFileDialog::updatePrimitiveList() {
  DataVectorList dataVectorList = _store->getObjects<DataVector>();
  DataMatrixList dataMatrixList = _store->getObjects<DataMatrix>();
  QStringList fileNameList;
  int i;

  _changeFilePrimitiveList->clear();

  for (i = 0; i < (int)dataVectorList.count(); i++) {
    dataVectorList[i]->readLock();
    _changeFilePrimitiveList->addItem(dataVectorList[i]->Name());
    fileNameList.push_back(dataVectorList[i]->filename());
    dataVectorList[i]->unlock();
  }

  for (i = 0; i < (int)dataMatrixList.count(); i++) {
    dataMatrixList[i]->readLock();
    _changeFilePrimitiveList->addItem(dataMatrixList[i]->Name());
    fileNameList.push_back(dataMatrixList[i]->filename());
    dataMatrixList[i]->unlock();
  }

  QString currentFile = _files->currentText();
  _files->clear();

  for (QStringList::Iterator it = fileNameList.begin(); it != fileNameList.end(); ++it) {
    _files->addItem(*it);
  }

  _allFromFile->setEnabled(_files->count() > 0);
  _files->setEnabled(_files->count() > 0);
}


void ChangeFileDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(_changeFilePrimitiveList->selectedItems().count() > 0);
  _buttonBox->button(QDialogButtonBox::Apply)->setEnabled(_changeFilePrimitiveList->selectedItems().count() > 0);
}


void ChangeFileDialog::selectAll() {
  _changeFilePrimitiveList->selectAll();
}


void ChangeFileDialog::updateSelection(const QString& txt) {
  _changeFilePrimitiveList->clearSelection();
  QRegExp re(txt, Qt::CaseSensitive, QRegExp::Wildcard);
  for (int i = 0; i < _changeFilePrimitiveList->count(); ++i) {
    _changeFilePrimitiveList->item(i)->setSelected(re.exactMatch(_changeFilePrimitiveList->item(i)->text()));
  }
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
}


void ChangeFileDialog::OKClicked() {
  apply();
  accept();
}


void ChangeFileDialog::apply() {
  Q_ASSERT(_store);
  DataSourcePtr dataSource = DataSource::findOrLoadSource(_store, _dataFile->file());
  if (!dataSource || !dataSource->isValid() || dataSource->isEmpty()) {
    QMessageBox::critical(this, tr("Kst"), tr("The file could not be loaded or contains no data."), QMessageBox::Ok);
    return;
  }

  QMap<RelationPtr, RelationPtr> duplicatedRelations;
  DataSourceList oldSources;
  QString invalidSources;
  int invalid = 0;

  QMap<DataVectorPtr, DataVectorPtr> duplicatedVectors;
  QMap<DataMatrixPtr, DataMatrixPtr> duplicatedMatrices;

  QList<QListWidgetItem*> selectedItems = _changeFilePrimitiveList->selectedItems();
  for (int i = 0; i < selectedItems.size(); ++i) {
    if (DataVectorPtr vector = kst_cast<DataVector>(_store->retrieveObject(selectedItems[i]->text()))) {
      vector->writeLock();
      dataSource->readLock();
      bool valid = dataSource->isValidField(vector->field());
      dataSource->unlock();
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
          newVector->changeFile(dataSource);
          newVector->update();
          newVector->unlock();

          if (_duplicateDependents->isChecked()) {
             duplicateDependents(VectorPtr(vector), VectorPtr(newVector), duplicatedRelations);
          }
        } else {
          if (!oldSources.contains(vector->dataSource())) {
            oldSources << vector->dataSource();
          }
          vector->writeLock();
          vector->changeFile(dataSource);
          vector->update();
          vector->unlock();
        }
      }
      vector->unlock();
    } else if (DataMatrixPtr matrix = kst_cast<DataMatrix>(_store->retrieveObject(selectedItems[i]->text()))) {
      matrix->writeLock();
      dataSource->readLock();
      bool valid = dataSource->isValidMatrix(matrix->field());
      dataSource->unlock();
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
          newMatrix->changeFile(dataSource);
          newMatrix->update();
          newMatrix->unlock();

          if (_duplicateDependents->isChecked()) {
            duplicateDependents(MatrixPtr(matrix), MatrixPtr(newMatrix), duplicatedRelations);
          }
        } else {
          if (!oldSources.contains(matrix->dataSource())) {
            oldSources << matrix->dataSource();
          }
          matrix->writeLock();
          matrix->changeFile(dataSource);
          matrix->update();
          matrix->unlock();
        }
      }
      matrix->unlock();
    }
  }

  // Plot the items.
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
  }
  updatePrimitiveList();
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
