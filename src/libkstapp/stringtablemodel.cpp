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

#include "stringtablemodel.h"

#include <assert.h>

namespace Kst {

StringTableModel::StringTableModel()
: QAbstractItemModel() {
}


StringTableModel::~StringTableModel() {
}


int StringTableModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 1;
}


int StringTableModel::rowCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  int rc = 0;
  foreach (StringModel *m, _strings) {
    Q_UNUSED(m)
    ++rc;
  }
  return rc;
}


QVariant StringTableModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }
  StringModel *m = _strings[index.row()];
  QModelIndex idx = m->index(0, index.column());
  return m->data(idx, role);
}


QModelIndex StringTableModel::index(int row, int col, const QModelIndex& parent) const {
  Q_UNUSED(parent)
  if (row < 0 || row >= _strings.count()) {
    return QModelIndex();
  }

  StringModel *m = _strings[row];
  if (col < 0 || col >= m->columnCount()) {
    return QModelIndex();
  }
  return createIndex(row, col);
}


QModelIndex StringTableModel::parent(const QModelIndex& index) const {
  Q_UNUSED(index)
  return QModelIndex();
}


QVariant StringTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole || section < 0 || section >= _strings.count()) {
    return QAbstractItemModel::headerData(section, orientation, role);
  } else if (orientation == Qt::Horizontal) {
      return QVariant("String");
  }
  return _strings[section]->headerData(0, orientation, role);
}


Qt::ItemFlags StringTableModel::flags(const QModelIndex& index) const {
  const int col = index.column();
  const int row = index.row();
  if (row < 0 || row >= _strings.count() || !index.isValid()) {
    return QAbstractItemModel::flags(index);
  }

  StringModel *m = _strings[row];
  if (col < 0 || col >= m->columnCount()) {
    return QAbstractItemModel::flags(index);
  }

  QModelIndex idx = m->index(0, col);
  return m->flags(idx);
}


bool StringTableModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (!index.isValid() || role != Qt::EditRole) {
    return false;
  }
  StringModel *m = _strings[index.row()];
  QModelIndex idx = m->index(0, index.column());
  return m->setData(idx, value, role);
}

}

// vim: ts=2 sw=2 et
