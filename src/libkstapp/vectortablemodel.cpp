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

#include "vectortablemodel.h"

#include <assert.h>

namespace Kst {

VectorTableModel::VectorTableModel()
: QAbstractItemModel() {
}


VectorTableModel::~VectorTableModel() {
}


int VectorTableModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return _vectors.count();
}


int VectorTableModel::rowCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  int rc = 0;
  foreach (VectorModel *m, _vectors) {
    rc = qMax(rc, m->rowCount());
  }
  return rc;
}


QVariant VectorTableModel::data(const QModelIndex& index, int role) const {
  Q_UNUSED(role)
  if (!index.isValid() || role != Qt::DisplayRole) {
    return QVariant();
  }
  VectorModel *m = _vectors[index.column()];
  QModelIndex idx = m->index(index.row(), 0);
  return m->data(idx, role);
}


QModelIndex VectorTableModel::index(int row, int col, const QModelIndex& parent) const {
  Q_UNUSED(parent)
  if (col < 0 || col >= _vectors.count()) {
    return QModelIndex();
  }

  VectorModel *m = _vectors[col];
  if (row < 0 || row >= m->rowCount()) {
    return QModelIndex();
  }
  return createIndex(row, col);
}


QModelIndex VectorTableModel::parent(const QModelIndex& index) const {
  Q_UNUSED(index)
  return QModelIndex();
}


QVariant VectorTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole || orientation == Qt::Vertical || section < 0 || section >= _vectors.count()) {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
  return _vectors[section]->headerData(0, orientation, role);
}

}

// vim: ts=2 sw=2 et
