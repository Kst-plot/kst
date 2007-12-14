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

#include "stringmodel.h"

#include <assert.h>
#include <objectstore.h>
#include <dataobject.h>
#include <datavector.h>
#include <generatedvector.h>
#include <datamatrix.h>
#include <generatedmatrix.h>
#include <string_kst.h>

namespace Kst {

StringModel::StringModel(ObjectStore *store)
: QAbstractItemModel(), _store(store) {
  generateObjectList();
}


StringModel::~StringModel() {
}


int StringModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 2;
}


void StringModel::generateObjectList() {
  ObjectList<DataObject> dol = _store->getObjects<DataObject>();
  ObjectList<String> sol = _store->getObjects<String>();

  foreach(DataObject* dataObject, dol) {
    foreach(StringPtr string, dataObject->outputStrings()) {
      _objectList.append(string);
    }
  }

  foreach(String* string, sol) {
    if (string->orphan()) {
      _objectList.append(string);
    }
  }
}

int StringModel::rowCount(const QModelIndex& parent) const {
  int rc = 0;
  if (!parent.isValid()) {
    rc = _objectList.count();
  }
  return rc;
}


QVariant StringModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  const int row = index.row();
  if (row < _objectList.count()) {
    if (StringPtr p = kst_cast<String>(_objectList.at(row))) {
      return stringData(p, index);
    }
  }

  return QVariant();
}


QVariant StringModel::stringData(StringPtr string, const QModelIndex& index) const {
  QVariant rc;

  if (string) {
    if (index.column() == Name) {
      string->readLock();
      rc.setValue(string->tag().name());
      string->unlock();
    } else if (index.column() == Value) {
      string->readLock();
      rc = QVariant(string->value());
      string->unlock();
    }
  }

  return rc;
}


QModelIndex StringModel::index(int row, int col, const QModelIndex& parent) const {
  if (row < 0 || col < 0 || col > 1) {
    return QModelIndex();
  }

  const int count = _objectList.count();
  ObjectPtr object = 0;
  if (!parent.isValid()) {
    if (row < count) {
      return createIndex(row, col);
    }
  }

  return QModelIndex();
}


QModelIndex StringModel::parent(const QModelIndex& index) const {
  Q_UNUSED(index);
  return QModelIndex();
}


QVariant StringModel::headerData(int section, Qt::Orientation orientation, int role) const {
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
