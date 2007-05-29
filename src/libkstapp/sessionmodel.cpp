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
#include <kstdatacollection.h>
#include <kstdataobjectcollection.h>

namespace Kst {

SessionModel::SessionModel()
: QAbstractItemModel() {
}


SessionModel::~SessionModel() {
}


int SessionModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 1;
}


int SessionModel::rowCount(const QModelIndex& parent) const {
  int rc = 0;
  if (!parent.isValid()) {
    rc = KST::dataObjectList.count() /* + generated primitives */;
    return rc;
  }

  QVariant p = parent.data();
  if (!p.isValid()) {
    return 0;
  }

  return rc;
}


QVariant SessionModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }
  return QVariant();
}


QModelIndex SessionModel::index(int row, int col, const QModelIndex& parent) const {
  return createIndex(row, col);
}


QModelIndex SessionModel::parent(const QModelIndex& index) const {
  Q_UNUSED(index)
  // If it is a primitive and has a provider, return the index of that provider
  return QModelIndex();
}


QVariant SessionModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole) {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
  return QVariant();
}

}

// vim: ts=2 sw=2 et
