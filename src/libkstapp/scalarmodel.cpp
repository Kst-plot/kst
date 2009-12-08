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

#include "scalarmodel.h"

#include <assert.h>
#include <objectstore.h>
#include <dataobject.h>
#include <datavector.h>
#include <generatedvector.h>
#include <datamatrix.h>
#include <generatedmatrix.h>
#include <datasource.h>

namespace Kst {

ScalarTreeItem::ScalarTreeItem(const QList<QVariant> &data, ScalarTreeItem *parent) {
  parentItem = parent;
  itemData = data; 
  if (parent) {
    parent->addChild(this);
  }
}


ScalarTreeItem::~ScalarTreeItem() {
  qDeleteAll(childItems);
}


void ScalarTreeItem::addChild(ScalarTreeItem *item) {
  childItems.append(item);
}


ScalarTreeItem *ScalarTreeItem::child(int row) {
    return childItems.value(row);
}

int ScalarTreeItem::childCount() const {
    return childItems.count();
}


int ScalarTreeItem::row() const {
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<ScalarTreeItem*>(this));

    return 0;
}


int ScalarTreeItem::columnCount() const {
    return itemData.count();
}


QVariant ScalarTreeItem::data(int column) const {
    return itemData.value(column);
}

ScalarTreeItem *ScalarTreeItem::parent() {
    return parentItem;
}


ScalarModel::ScalarModel(ObjectStore *store)
: QAbstractItemModel(), _store(store) {
  QList<QVariant> rootData;
  rootData << "Scalars";
  _rootItem = new ScalarTreeItem(rootData);
  createTree();
}


ScalarModel::~ScalarModel() {
}


int ScalarModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 2;
}


void ScalarModel::addScalar(ScalarPtr scalar, ScalarTreeItem* parent) {
  ScalarTreeItem* parentItem;
  if (parent) {
    parentItem = parent;
  } else {
    parentItem = _rootItem;
  }
  QList<QVariant> data;
  data << scalar->Name() << scalar->value();
  new ScalarTreeItem(data, parentItem);
}


void ScalarModel::addScalars(const QHash<QString, Kst::ScalarPtr> scalarMap, ScalarTreeItem* parent) {
  QMap<QString, ScalarPtr> map;
  foreach(ScalarPtr scalar, scalarMap) {
    map.insert(scalar->Name(), scalar);
  }
  QMapIterator<QString, ScalarPtr> iObject(map);
  while (iObject.hasNext()) {
    iObject.next();
    addScalar(iObject.value(), parent);
  }
}


void ScalarModel::addVector(VectorPtr vector, ScalarTreeItem* parent) {
  ScalarTreeItem* parentItem;
  if (parent) {
    parentItem = parent;
  } else {
    parentItem = _rootItem;
  }
  QList<QVariant> data;
  data << vector->Name();
  ScalarTreeItem* item = new ScalarTreeItem(data, parentItem);
  addScalars(vector->scalars(), item);
}


void ScalarModel::addMatrix(MatrixPtr matrix, ScalarTreeItem* parent) {
  ScalarTreeItem* parentItem;
  if (parent) {
    parentItem = parent;
  } else {
    parentItem = _rootItem;
  }
  QList<QVariant> data;
  data << matrix->Name();
  ScalarTreeItem* item = new ScalarTreeItem(data, parentItem);
  addScalars(matrix->scalars(), item);
}


void ScalarModel::addDataObject(DataObjectPtr dataObject, ScalarTreeItem* parent) {
  ScalarTreeItem* parentItem;
  if (parent) {
    parentItem = parent;
  } else {
    parentItem = _rootItem;
  }
  QList<QVariant> data;
  data << dataObject->Name();
  ScalarTreeItem* item = new ScalarTreeItem(data, parentItem);

  QMap<QString, ObjectPtr> objectMap;
  foreach(Scalar* scalar, dataObject->outputScalars()) {
    objectMap.insert(scalar->Name(), scalar);
  }
  foreach(Vector* vector, dataObject->outputVectors()) {
    objectMap.insert(vector->Name(), vector);
  }
  foreach(Matrix* matrix, dataObject->outputMatrices()) {
    objectMap.insert(matrix->Name(), matrix);
  }

  QMapIterator<QString, ObjectPtr> iObject(objectMap);
  while (iObject.hasNext()) {
    iObject.next();
    if (VectorPtr vector = kst_cast<Vector>(iObject.value())) {
      addVector(vector, item);
    } else if (MatrixPtr matrix = kst_cast<Matrix>(iObject.value())) {
      addMatrix(matrix, item);
    } else if (ScalarPtr scalar = kst_cast<Scalar>(iObject.value())) {
      addScalar(scalar, item);
    }
  }
}


void ScalarModel::addDataSource(DataSourcePtr dataSource, ScalarTreeItem* parent) {
  ScalarTreeItem* parentItem;
  if (parent) {
    parentItem = parent;
  } else {
    parentItem = _rootItem;
  }
  QList<QVariant> data;
  data << dataSource->shortName();
  ScalarTreeItem* item = new ScalarTreeItem(data, parentItem);

  QStringList scalars = dataSource->scalarList();
  scalars.sort();
  foreach(QString scalar, scalars) {
    QList<QVariant> data;
    double value;
    dataSource->readScalar(value, scalar);
    data << scalar << value;
    new ScalarTreeItem(data, item);
  }
}


void ScalarModel::createTree() {
  QMap<QString, ObjectPtr> objectMap;
  foreach(DataVector* vector, _store->getObjects<DataVector>()) {
    objectMap.insert(vector->Name(), vector);
  }
  foreach(GeneratedVector* vector, _store->getObjects<GeneratedVector>()) {
    objectMap.insert(vector->Name(), vector);
  }
  foreach(DataMatrix* matrix, _store->getObjects<DataMatrix>()) {
    objectMap.insert(matrix->Name(), matrix);
  }
  foreach(GeneratedMatrix* matrix, _store->getObjects<GeneratedMatrix>()) {
    objectMap.insert(matrix->Name(), matrix);
  }
  foreach(DataObjectPtr dataObject, _store->getObjects<DataObject>()) {
    objectMap.insert(dataObject->Name(), dataObject);
  }
  foreach(Scalar* scalar, _store->getObjects<Scalar>()) {
    if (scalar->orphan()) {
      objectMap.insert(scalar->Name(), scalar);
    }
  }
  foreach(DataSource* ds, _store->dataSourceList()) {
    if (!ds->scalarList().isEmpty()) {
      objectMap.insert(ds->shortName(), ds);
    }
  }

  QMapIterator<QString, ObjectPtr> iObject(objectMap);
  while (iObject.hasNext()) {
    iObject.next();
    if (VectorPtr vector = kst_cast<Vector>(iObject.value())) {
      addVector(vector);
    } else if (MatrixPtr matrix = kst_cast<Matrix>(iObject.value())) {
      addMatrix(matrix);
    } else if (DataObjectPtr dataObject = kst_cast<DataObject>(iObject.value())) {
      addDataObject(dataObject);
    } else if (ScalarPtr scalar = kst_cast<Scalar>(iObject.value())) {
      addScalar(scalar);
    } else if (DataSourcePtr dataSource = kst_cast<DataSource>(iObject.value())) {
      addDataSource(dataSource);
    }
  }
}

int ScalarModel::rowCount(const QModelIndex& parent) const {
  ScalarTreeItem *parentItem;
  if (parent.column() > 0)
      return 0;

  if (!parent.isValid())
      parentItem = _rootItem;
  else
      parentItem = static_cast<ScalarTreeItem*>(parent.internalPointer());

  return parentItem->childCount();
}


QVariant ScalarModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  ScalarTreeItem *item = static_cast<ScalarTreeItem*>(index.internalPointer());

  return item->data(index.column());
}


QModelIndex ScalarModel::index(int row, int col, const QModelIndex& parent) const {
  if (row < 0 || col < 0 || col > 1) {
    return QModelIndex();
  }

  if (!hasIndex(row, col, parent))
      return QModelIndex();

  ScalarTreeItem *parentItem;

  if (!parent.isValid())
      parentItem = _rootItem;
  else
      parentItem = static_cast<ScalarTreeItem*>(parent.internalPointer());

  ScalarTreeItem *childItem = parentItem->child(row);
  if (childItem)
      return createIndex(row, col, childItem);
  else
      return QModelIndex();
}


QModelIndex ScalarModel::parent(const QModelIndex& index) const {
  Q_ASSERT(_store);

  if (!index.isValid())
      return QModelIndex();

  ScalarTreeItem *childItem = static_cast<ScalarTreeItem*>(index.internalPointer());
  ScalarTreeItem *parentItem = childItem->parent();

  if (parentItem == _rootItem)
      return QModelIndex();

  return createIndex(parentItem->row(), 0, parentItem);
}


QVariant ScalarModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole) {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
  switch (section) {
    case Name:
      return tr("Name");
    case Value:
      return tr("Value");
    default:
      break;
  }
  return QVariant();
}

}

// vim: ts=2 sw=2 et
