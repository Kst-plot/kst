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

#include "scalarmodel.h"

#include <assert.h>

#include <QFont>

namespace Kst {

ScalarModel::ScalarModel(Scalar *scalar)
: QAbstractItemModel(), _scalar(scalar) {
  assert(scalar);
}


ScalarModel::~ScalarModel() {
}


int ScalarModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 1;
}


int ScalarModel::rowCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 1;
}


QVariant ScalarModel::data(const QModelIndex& index, int role) const {
  Q_UNUSED(role)
  Q_UNUSED(index)
  QVariant rc;
  if (index.isValid() && _scalar) {
    switch (role) {
      case Qt::DisplayRole:
        if (index.column() == 0) {
          rc = QVariant(_scalar->tag().displayString());
        } else {
          rc = QVariant(_scalar->value());
        }
        break;
      case Qt::FontRole:
        {
          if (_scalar->editable()) {
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


QModelIndex ScalarModel::index(int row, int col, const QModelIndex& parent) const {
  Q_UNUSED(parent)
  Q_UNUSED(col)
  if (_scalar) {
    return createIndex(row, 1);
  }
  return QModelIndex();
}


QModelIndex ScalarModel::parent(const QModelIndex& index) const {
  Q_UNUSED(index)
  return QModelIndex();
}


QVariant ScalarModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (!_scalar || role != Qt::DisplayRole || section != 0) {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
  return _scalar->tag().displayString();
}


Qt::ItemFlags ScalarModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags f = QAbstractItemModel::flags(index);
  if (!_scalar || !index.isValid()) {
    return f;
  }

  if (_scalar->editable() && index.row() >= 0) {
    f |= Qt::ItemIsEditable;
  }

  return f;
}


bool ScalarModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (role != Qt::EditRole) {
    return QAbstractItemModel::setData(index, value, role);
  }

  if (!_scalar || !index.isValid() || !_scalar->editable() || index.row() < 0) {
    return false;
  }

  bool ok = false;
  double scalarValue = value.toDouble(&ok);
  if (!ok) {
    return false;
  }

  qDebug() << "UGLY!! Add setData API to Scalar!";
  _scalar->setValue(scalarValue);
  return true;
}


}

// vim: ts=2 sw=2 et
