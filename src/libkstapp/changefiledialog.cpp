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

#include "changefiledialog.h"

#include "datacollection.h"
#include "datavector.h"
#include "datamatrix.h"
#include "datascalar.h"
#include "datastring.h"
#include "vscalar.h"
#include "primitive.h"

#include "plotitem.h"

#include "objectstore.h"
#include "document.h"
#include "mainwindow.h"
#include "application.h"
#include "updatemanager.h"
#include "datasourcepluginmanager.h"

#include <QDir>
#include <QMessageBox>
#include <QThreadPool>

namespace Kst {

ChangeFileDialog::ChangeFileDialog(QWidget *parent)
  : QDialog(parent), _dataSource(0), _requestID(0) {
   setupUi(this);

  MainWindow::setWidgetFlags(this);

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
  _dataSource = DataSourcePluginManager::findOrLoadSource(_store, filename);
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
  PrimitiveList allPrimitives = _store->getObjects<Primitive>();

  QStringList fileNameList;

  ObjectList<Primitive> primitives;

  foreach (const PrimitivePtr& P, allPrimitives) {
    DataPrimitive* dp = qobject_cast<DataPrimitive*>(P);
    if (dp) {
      primitives.append(P);
      fileNameList.append(dp->filename());
    }
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

  // we need a list which preservs the order things are added, and a map to associate the duplicated
  // primitive with its duplicate.
  QMap<PrimitivePtr, PrimitivePtr> duplicatedPrimitiveMap; // associate duplicated with duplicates.
  PrimitiveList duplicatedPrimitiveList; // list of duplicated primitives in order they were found

  DataSourceList oldSources;
  QString invalidSources;
  int invalid = 0;

  _selectedFilePrimitiveList->selectAll();
  QList<QListWidgetItem*> selectedItems = _selectedFilePrimitiveList->selectedItems();
  int n_selectedItems = selectedItems.size();
  for (int i = 0; i < n_selectedItems; ++i) {
    PrimitivePtr prim = kst_cast<Primitive>(_store->retrieveObject(selectedItems[i]->text()));    
    if (prim) {
      DataPrimitive* dp = qobject_cast<DataPrimitive*>(prim);
      if (dp) {
        prim->readLock();
        _dataSource->readLock();
        bool valid = dp->checkValidity(_dataSource);
        _dataSource->unlock();
        prim->unlock();
        if (!valid) {
          if (invalid > 0) {
            invalidSources += ", ";
          }
          invalidSources += dp->field();
          ++invalid;
        } else if (_duplicateSelected->isChecked()) {
          prim->readLock();
          PrimitivePtr newPrim = dp->makeDuplicate();
          DataPrimitive* newdp = qobject_cast<DataPrimitive*>(newPrim);
          prim->unlock();
          newPrim->writeLock();
          newdp->changeFile(_dataSource);
          newPrim->unlock();
          duplicatedPrimitiveMap[prim] = newPrim;
          duplicatedPrimitiveList.append(prim);
          PrimitiveList output_prims = prim->outputPrimitives();
          PrimitiveList dup_output_prims = newPrim->outputPrimitives();
          // add output primitives to list of primitives that have been duplicated.
          int n = output_prims.count();
          for (int i_output=0; i_output<n; i_output++) {
            duplicatedPrimitiveList.append(output_prims.at(i_output));
            duplicatedPrimitiveMap[output_prims.at(i_output)] = dup_output_prims.at(i_output);
          }
        } else {
          prim->readLock();
          if (!oldSources.contains(dp->dataSource())) {
            oldSources << dp->dataSource();
          }
          prim->unlock();
          prim->writeLock();
          dp->changeFile(_dataSource);
          prim->unlock();
        }
      }
    }
    prim = 0;
  }

  // Duplicate data objects:
  // list of all data objects before we start duplication
  DataObjectList dataObjects = _store->getObjects<DataObject>();

  // map of data objects which have been duplicated.
  QMap<DataObjectPtr, DataObjectPtr> duplicatedDataObjects;

  // Lookup list to record if the data object has already been duplicated
  // (We could have just checked if it was in the map, but that is slower)
  QVector<bool> dataObjectDuplicated(dataObjects.count());
  int n_dataObjects = dataObjects.count();
  for (int i_OB = 0; i_OB < n_dataObjects; i_OB++) {
    dataObjectDuplicated[i_OB] = false;
  }


  // go through whole list until nothing is duplicated
  bool new_duplicate_found;
  do {
    new_duplicate_found = false;
    for (int i_DP =0; i_DP<duplicatedPrimitiveList.count(); i_DP++) { // The list size will grow, so check count() each time.
      for (int i_OB = 0; i_OB < n_dataObjects; i_OB++) {
        PrimitiveList input_primitives = dataObjects.at(i_OB)->inputPrimitives();
        if (input_primitives.contains(duplicatedPrimitiveList.at(i_DP))) {
          DataObjectPtr duplicate_object;
          if (!dataObjectDuplicated[i_OB]) {
            duplicate_object = dataObjects.at(i_OB)->makeDuplicate();

            // put the outputs of the new data object into the list of duplicated primitives.
            PrimitiveList dup_output_prims = duplicate_object->outputPrimitives();
            PrimitiveList output_prims = dataObjects.at(i_OB)->outputPrimitives();
            int n = output_prims.count();
            for (int i_output=0; i_output<n; i_output++) {
              duplicatedPrimitiveList.append(output_prims.at(i_output));
              duplicatedPrimitiveMap[output_prims.at(i_output)] = dup_output_prims.at(i_output);
            }

            duplicatedDataObjects[dataObjects.at(i_OB)] = duplicate_object;
            dataObjectDuplicated[i_OB] = true;
            new_duplicate_found = true;
          } else {
            duplicate_object = duplicatedDataObjects[dataObjects.at(i_OB)];
          }
          // replace the input primitive with its copy.
          PrimitivePtr old_input_primitive = duplicatedPrimitiveList.at(i_DP);
          PrimitivePtr new_input_primitive = duplicatedPrimitiveMap[old_input_primitive];
          duplicate_object->replaceInput(old_input_primitive, new_input_primitive);
        }
      }
    }
  } while (new_duplicate_found);


  // Duplicate Relations
  RelationList relations = _store->getObjects<Relation>();

  // map of data objects which have been duplicated.
  QMap<RelationPtr, RelationPtr> duplicatedRelations;

  // Lookup list to record if the relation has already been duplicated
  // (We could have just checked if it was in the map, but that is slower)
  int n_relations = relations.count();
  QVector<bool> relationDuplicated(n_relations);
  for (int i_R = 0; i_R < n_relations; i_R++) {
    relationDuplicated[i_R] = false;
  }

  for (int i_DP =0; i_DP<duplicatedPrimitiveList.count(); i_DP++) {
    for (int i_R = 0; i_R < n_relations; i_R++) {
      PrimitiveList input_primitives = relations.at(i_R)->inputPrimitives();
      if (input_primitives.contains(duplicatedPrimitiveList.at(i_DP))) {
        RelationPtr duplicate_relation;
        if (!relationDuplicated[i_R]) {
          duplicate_relation = relations.at(i_R)->makeDuplicate();
          duplicatedRelations[relations.at(i_R)] = duplicate_relation;
          relationDuplicated[i_R] = true;
        } else {
          duplicate_relation = duplicatedRelations[relations.at(i_R)];
        }
        // replace the input primitive with its copy.
        PrimitivePtr old_input_primitive = duplicatedPrimitiveList.at(i_DP);
        PrimitivePtr new_input_primitive = duplicatedPrimitiveMap[old_input_primitive];
        duplicate_relation->replaceInput(old_input_primitive, new_input_primitive);
      }
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
  } else {
    updatePrimitiveList();
  }

  UpdateManager::self()->doUpdates(true);
  kstApp->mainWindow()->document()->setChanged(true);
}

}
// vim: ts=2 sw=2 et
