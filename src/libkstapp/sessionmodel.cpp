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
  return 5;
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
  QVariant rc;
  if (!index.isValid() || role != Qt::DisplayRole) {
    return rc;
  }
  const int row = index.row(), col = index.column();
  KstDataObjectPtr p = KST::dataObjectList[row];
  if (!p) {
    return rc;
  }
  switch (col) {
    case 0:
      p->readLock();
      rc = p->tagName();
      p->unlock();
      break;
    case 1:
    case 2:
    case 3:
    case 4:
    default:
      break;
  }
  return rc;
}


QModelIndex SessionModel::index(int row, int col, const QModelIndex& parent) const {
  if (col < 0 || col > 4) {
    return QModelIndex();
  }
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
