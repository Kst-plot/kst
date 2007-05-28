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

#include "vectormodel.h"

#include <assert.h>

namespace Kst {

VectorModel::VectorModel(KstVectorPtr v)
: QAbstractItemModel(), _v(v) {
  assert(v.data());
}


VectorModel::~VectorModel() {
}


int VectorModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 1;
}


int VectorModel::rowCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return _v->length();
}


QVariant VectorModel::data(const QModelIndex& index, int role) const {
  Q_UNUSED(role)
  if (!index.isValid() || role != Qt::DisplayRole) {
    return QVariant();
  }
  return QVariant(_v->value(index.row()));
}


QModelIndex VectorModel::index(int row, int col, const QModelIndex& parent) const {
  Q_UNUSED(parent)
  Q_UNUSED(col)
  if (row < _v->length()) {
    return createIndex(row, 1);
  }
  return QModelIndex();
}


QModelIndex VectorModel::parent(const QModelIndex& index) const {
  Q_UNUSED(index)
  return QModelIndex();
}


QVariant VectorModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole || orientation == Qt::Vertical || section != 0) {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
  return _v->tagName();
}

}

// vim: ts=2 sw=2 et
