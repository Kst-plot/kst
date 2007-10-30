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

#include "sessionmodel.h"

#include <assert.h>
#include <objectstore.h>
#include <dataobject.h>

namespace Kst {

SessionModel::SessionModel(ObjectStore *store)
: QAbstractItemModel(), _store(store) {
}


SessionModel::~SessionModel() {
}


int SessionModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 5;
}


int SessionModel::rowCount(const QModelIndex& parent) const {
  Q_ASSERT(_store);
  DataObjectList dol = _store->getObjects<DataObject>();

  int rc = 0;
  if (!parent.isValid()) {
    rc = dol.count();  /* + generated primitives */
    return rc;
  }

  if (parent.parent().isValid()) {
    return rc;
  }

  DataObject *pdo = dol.at(parent.row());
  Q_ASSERT(pdo);
  if (pdo) {
    pdo->readLock();
    rc = pdo->outputVectors().count();
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
      DataObjectPtr parent = qVariantValue<DataObject*>(p);
      VectorPtr v = parent->outputVectors().values()[index.row()];
      return qVariantFromValue(v.data());
    } else {
      Q_ASSERT(_store);
      DataObjectPtr p = _store->getObjects<DataObject>().at(index.row());
      return qVariantFromValue(p.data());
    }
  }

  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  if (index.internalPointer()) { //parent().isValid()) {
    Q_ASSERT(!index.parent().parent().isValid());
    return vectorData(index, role);
  }

  return dataObjectData(index, role);
}


QVariant SessionModel::vectorData(const QModelIndex& index, int role) const {
  Q_UNUSED(role)
  QVariant rc;

  DataObject *parent = static_cast<DataObject*>(index.internalPointer());
  if (!parent) {
    return rc;
  }

  const int row = index.row(), col = index.column();
  VectorPtr v = parent->outputVectors().values()[row];
  if (!v) {
    return rc;
  }

  switch (col) {
    case 0:
      {
      v->readLock();
      rc.setValue(v->tag().displayString());
      v->unlock();
      break;
      }
    case 1:
      v->readLock();
      rc = v->typeString();
      v->unlock();
      break;
    case 2:
      break;
    case 3:
      v->readLock();
      rc = v->length();
      v->unlock();
      break;
    case 4:
      v->readLock();
      rc = tr("[%1..%2]").arg(v->min()).arg(v->max());
      v->unlock();
      break;
    default:
      break;
  }
  return rc;
}


QVariant SessionModel::dataObjectData(const QModelIndex& index, int role) const {
  Q_UNUSED(role)
  QVariant rc;
  const int row = index.row(), col = index.column();
  Q_ASSERT(_store);
  DataObjectPtr p = _store->getObjects<DataObject>().at(row);
  if (!p) {
    return rc;
  }
  switch (col) {
    case 0:
      p->readLock();
      rc.setValue(p->tag().displayString());
      p->unlock();
      break;
    case 1:
      p->readLock();
      rc = p->typeString();
      p->unlock();
      break;
    case 2:
      break;
    case 3:
      p->readLock();
      rc = p->sampleCount();
      p->unlock();
      break;
    case 4:
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
  DataObjectList dol = _store->getObjects<DataObject>();

  if (!parent.isValid()) {
    const int cnt = dol.count();
    if (row >= cnt) {
      return QModelIndex();
    }
    return createIndex(row, col);
  }

  const int cnt = dol.count();
  DataObject *p = 0;
  if (row >= 0 && row < cnt) {
    p = dol.at(row);
  }
  if (!p) {
    return QModelIndex();
  }

  p->readLock();
  if (row >= p->outputVectors().count()) {
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
  const int cnt = _store->getObjects<DataObject>().indexOf(dop);
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
      return tr("Used");
    case 3:
      return tr("Samples");
    case 4:
      return tr("Properties");
    default:
      break;
  }
  return QVariant();
}

}

// vim: ts=2 sw=2 et
