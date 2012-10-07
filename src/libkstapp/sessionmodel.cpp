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

#include "sessionmodel.h"

#include <assert.h>
#include <objectstore.h>
#include <dataobject.h>
#include <relation.h>
#include <datavector.h>
#include <datascalar.h>
#include <vscalar.h>
#include <generatedvector.h>
#include <datamatrix.h>
#include <generatedmatrix.h>

namespace Kst {

SessionModel::SessionModel(ObjectStore *store)
: QAbstractItemModel(), _store(store) {
  generateObjectList();
}


SessionModel::~SessionModel() {
}


int SessionModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 4;
}


void SessionModel::triggerReset() {
  generateObjectList();
  reset();
}


void SessionModel::generateObjectList() {
  ObjectList<Primitive> pol = _store->getObjects<Primitive>();
  ObjectList<Relation> rol = _store->getObjects<Relation>();
  ObjectList<DataObject> dol = _store->getObjects<DataObject>();
  _objectList.clear();

  foreach(Primitive* P, pol) {
    if (!P->provider()) {
      _objectList.append(P);
    }
  }

  foreach(Relation* relation, rol) {
    _objectList.append(relation);
  }

  foreach(DataObject* dataObject, dol) {
    _objectList.append(dataObject);
  }
}


int SessionModel::rowCount(const QModelIndex& parent) const {
  Q_ASSERT(_store);
  //ObjectList<Object> dol = generateObjectList();
  int rc = 0;
  if (!parent.isValid()) {
    rc = _objectList.count();  /* + generated primitives */
    return rc;
  }

  if (parent.parent().isValid()) {
    return rc;
  }

  DataObject *pdo = kst_cast<DataObject>(_objectList.at(parent.row()));
  if (pdo) {
    pdo->readLock();
    rc = pdo->outputVectors().count();
    rc += pdo->outputMatrices().count();
    pdo->unlock();
  }
  return rc;
}


QVariant SessionModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }
  if (role == Qt::UserRole) {
    if (index.parent().isValid()) {
      Q_ASSERT(!index.parent().parent().isValid());
      QVariant p = data(index.parent(), role);
      DataObjectPtr parent = p.value<DataObject*>();
      const int vectorCount = parent->outputVectors().count();
      if (index.row() < vectorCount) {
        if (VectorPtr v = parent->outputVectors().values()[index.row()]) {
          return qVariantFromValue(v.data());
        }
      } else if (MatrixPtr m = parent->outputMatrices().values()[index.row() - vectorCount]) {
        return qVariantFromValue(m.data());
      }
    } else {
      Q_ASSERT(_store);
      DataObjectPtr p = kst_cast<DataObject>(_objectList.at(index.row()));
      return qVariantFromValue(p.data());
    }
  }

  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  if (index.internalPointer()) { //parent().isValid()) {
    Q_ASSERT(!index.parent().parent().isValid());
    DataObject *parent = static_cast<DataObject*>(index.internalPointer());
    return dataObjectOutputData(parent, index);
  }

  Q_ASSERT(_store);
  //ObjectList<Object> objectList = generateObjectList();

  const int row = index.row();
  if (row >= _objectList.count()) {
    return QVariant();
  }

  if (DataObjectPtr p = kst_cast<DataObject>(_objectList.at(row))) {
    return dataObjectData(p, index);
  } else if (RelationPtr p = kst_cast<Relation>(_objectList.at(row))) {
    return relationData(p, index);
  } else if (PrimitivePtr p=kst_cast<Primitive>(_objectList.at(row))) {
    return primitiveData(p, index);
  } else {
    return QVariant();
  }
}


QVariant SessionModel::dataObjectOutputData(DataObjectPtr parent, const QModelIndex& index) const {
  QVariant rc;

  if (!parent) {
    return rc;
  }

  const int row = index.row();
  const int vectorCount = parent->outputVectors().count();
  if (parent->outputVectors().count() > row) {
    if (VectorPtr v = parent->outputVectors().values()[row]) {
      return primitiveData(v, index);
    }
  } else if (MatrixPtr m = parent->outputMatrices().values()[row - vectorCount]) {
    return primitiveData(m, index);
  }
  return rc;
}


QVariant SessionModel::primitiveData(PrimitivePtr p, const QModelIndex& index) const {
  QVariant rc;

  if (!p) {
    return rc;
  }

  p->readLock();
  switch (index.column()) {
    case 0:
      rc.setValue(p->Name());
      break;
    case 1:
      rc = p->typeString();
      break;
    case 2:
      rc = p->sizeString();
      break;
    case 3:
      rc = p->propertyString();
      break;
    default:
      break;
  }
  p->unlock();
  return rc;
}

QVariant SessionModel::dataObjectData(DataObjectPtr p, const QModelIndex& index) const {
  QVariant rc;
  if (!p) {
    return rc;
  }

  switch (index.column()) {
    case 0:
      p->readLock();
      rc.setValue(p->Name());
      p->unlock();
      break;
    case 1:
      p->readLock();
      rc = p->typeString();
      p->unlock();
      break;
    case 2:
      p->readLock();
      rc = p->sampleCount();
      p->unlock();
      break;
    case 3:
      p->readLock();
      rc = p->propertyString();
      p->unlock();
      break;
    default:
      break;
  }
  return rc;
}


QVariant SessionModel::relationData(RelationPtr p, const QModelIndex& index) const {
  QVariant rc;
  if (!p) {
    return rc;
  }

  switch (index.column()) {
    case 0:
      p->readLock();
      rc.setValue(p->Name());
      p->unlock();
      break;
    case 1:
      p->readLock();
      rc = p->typeString();
      p->unlock();
      break;
    case 2:
      p->readLock();
      rc = p->sampleCount();
      p->unlock();
      break;
    case 3:
      p->readLock();
      rc = p->propertyString();
      p->unlock();
      break;
    default:
      break;
  }
  return rc;
}


QModelIndex SessionModel::index(int row, int col, const QModelIndex& parent) const {
  if (row < 0 || col < 0 || col > 4) {
    return QModelIndex();
  }

  Q_ASSERT(_store);
  //ObjectList<Object> dol = generateObjectList();

  if (!parent.isValid()) {
    const int cnt = _objectList.count();
    if (row >= cnt) {
      return QModelIndex();
    }
    return createIndex(row, col);
  }

  const int cnt = _objectList.count();
  DataObject *p = 0;
  if (parent.row() >= 0 && parent.row() < cnt) { 
    p = kst_cast<DataObject>(_objectList.at(parent.row()));
  }
  if (!p) {
    return QModelIndex();
  }

  p->readLock();
  if (row >= (p->outputVectors().count() + p->outputMatrices().count())) {
    p->unlock();
    return QModelIndex();
  }
  p->unlock();
  return createIndex(row, col, p);
}


QModelIndex SessionModel::parent(const QModelIndex& index) const {
  // If it is a primitive and has a provider, return the index of that provider

  DataObject *dop = static_cast<DataObject*>(index.internalPointer());
  if (!dop) {
    return QModelIndex();
  }

  Q_ASSERT(_store);
  const int cnt = _objectList.indexOf(dop);
  if (cnt < 0) {
    return QModelIndex();
  }
  return createIndex(cnt, index.column());
}


QVariant SessionModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole) {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
  switch (section) {
    case 0:
      return tr("Name");
    case 1:
      return tr("Type");
    case 2:
      return tr("Samples");
    case 3:
      return tr("Properties");
    default:
      break;
  }
  return QVariant();
}

}

// vim: ts=2 sw=2 et
