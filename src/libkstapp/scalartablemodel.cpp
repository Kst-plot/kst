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

#include "scalartablemodel.h"

#include <assert.h>

namespace Kst {

ScalarTableModel::ScalarTableModel()
: QAbstractItemModel() {
}


ScalarTableModel::~ScalarTableModel() {
}


int ScalarTableModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 1;
}


int ScalarTableModel::rowCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  int rc = 0;
  foreach (ScalarModel *m, _scalars) {
    Q_UNUSED(m)
    ++rc;
  }
  return rc;
}


QVariant ScalarTableModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }
  ScalarModel *m = _scalars[index.row()];
  QModelIndex idx = m->index(0, index.column());
  return m->data(idx, role);
}


QModelIndex ScalarTableModel::index(int row, int col, const QModelIndex& parent) const {
  Q_UNUSED(parent)
  if (row < 0 || row >= _scalars.count()) {
    return QModelIndex();
  }

  ScalarModel *m = _scalars[row];
  if (col < 0 || col >= m->columnCount()) {
    return QModelIndex();
  }
  return createIndex(row, col);
}


QModelIndex ScalarTableModel::parent(const QModelIndex& index) const {
  Q_UNUSED(index)
  return QModelIndex();
}


QVariant ScalarTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole || section < 0 || section >= _scalars.count()) {
    return QAbstractItemModel::headerData(section, orientation, role);
  } else if (orientation == Qt::Horizontal) {
      return QVariant("Scalar Value");
  }
  return _scalars[section]->headerData(0, orientation, role);
}


Qt::ItemFlags ScalarTableModel::flags(const QModelIndex& index) const {
  const int col = index.column();
  const int row = index.row();
  if (row < 0 || row >= _scalars.count() || !index.isValid()) {
    return QAbstractItemModel::flags(index);
  }

  ScalarModel *m = _scalars[row];
  if (col < 0 || col >= m->columnCount()) {
    return QAbstractItemModel::flags(index);
  }

  QModelIndex idx = m->index(0, col);
  return m->flags(idx);
}


bool ScalarTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (!index.isValid() || role != Qt::EditRole) {
    return false;
  }
  ScalarModel *m = _scalars[index.row()];
  QModelIndex idx = m->index(0, index.column());
  return m->setData(idx, value, role);
}

}

// vim: ts=2 sw=2 et
