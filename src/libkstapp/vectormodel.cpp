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

#include <QFont>

namespace Kst {

VectorModel::VectorModel(Vector *v)
: QAbstractItemModel(), _v(v) {
  assert(v);
}


VectorModel::~VectorModel() {
}


int VectorModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 1;
}


int VectorModel::rowCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return _v ? _v->length() : 0;
}


QVariant VectorModel::data(const QModelIndex& index, int role) const {
  Q_UNUSED(role)
  QVariant rc;
  if (index.isValid() && _v) {
    switch (role) {
      case Qt::DisplayRole:
        rc = QVariant(_v->value(index.row()));
        break;
      case Qt::FontRole:
        {
          if (_v->editable()) {
            QFont f;
            f.setBold(true);
            rc = f;
          }
        }
        break;
      default:
        break;
    }
  }
  return rc;
}


QModelIndex VectorModel::index(int row, int col, const QModelIndex& parent) const {
  Q_UNUSED(parent)
  Q_UNUSED(col)
  if (_v && row < _v->length()) {
    return createIndex(row, 1);
  }
  return QModelIndex();
}


QModelIndex VectorModel::parent(const QModelIndex& index) const {
  Q_UNUSED(index)
  return QModelIndex();
}


QVariant VectorModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (!_v || role != Qt::DisplayRole || orientation == Qt::Vertical || section != 0) {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
  return _v->tagName();
}


Qt::ItemFlags VectorModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags f = QAbstractItemModel::flags(index);
  if (!_v || !index.isValid()) {
    return f;
  }

  if (_v->editable() && index.row() >= 0 && index.row() < _v->length()) {
    f |= Qt::ItemIsEditable;
  }

  return f;
}


bool VectorModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (role != Qt::EditRole) {
    return QAbstractItemModel::setData(index, value, role);
  }

  if (!_v || !index.isValid() || !_v->editable() || index.row() < 0 || index.row() >= _v->length()) {
    return false;
  }

  bool ok = false;
  double v = value.toDouble(&ok);
  if (!ok) {
    return false;
  }

  qDebug() << "UGLY!! Add setData API to KstVector!";
  double *d = const_cast<double*>(_v->value());
  d[index.row()] = v;
  return true;
}


}

// vim: ts=2 sw=2 et
