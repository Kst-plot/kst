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
: QAbstractTableModel (), _vector(v) {
  assert(v);
}


VectorModel::~VectorModel() {
}


int VectorModel::columnCount(const QModelIndex&) const {
  return 2;
}


int VectorModel::rowCount(const QModelIndex&) const {
  return _vector ? _vector->length() : 0;
}


QVariant VectorModel::data(const QModelIndex& index, int role) const {
  if (index.isValid() && _vector) {
    switch (role) {
      case Qt::DisplayRole:
        if (index.column() == 0) {
          return QVariant(index.row());
        } else if (index.column() == 1) {
          return QVariant(_vector->value(index.row()));
        }
        break;
      case Qt::FontRole:
        {
          if (_vector->editable()) {
            QFont f;
            f.setBold(true);
            return QVariant(f);
          }
        }
        break;
      default:
        break;
    }
  }
  return QVariant();
}


QModelIndex VectorModel::parent(const QModelIndex&) const {
  return QModelIndex();
}


QVariant VectorModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (!_vector || role != Qt::DisplayRole || orientation == Qt::Vertical || section > 1) {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
  if (section == 0) {
    return QVariant("Index");
  } else if(section == 1) {
    return QVariant(_vector->Name());
  }
  return QVariant();
}


Qt::ItemFlags VectorModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags f = QAbstractItemModel::flags(index);
  if (!_vector || !index.isValid()) {
    return f;
  }

  if (_vector->editable() && index.row() >= 0 && index.row() < _vector->length()) {
    f |= Qt::ItemIsEditable;
  }

  return f;
}


bool VectorModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (role != Qt::EditRole) {
    return QAbstractItemModel::setData(index, value, role);
  }

  if (!_vector || !index.isValid() || !_vector->editable() || index.row() < 0 || index.row() >= _vector->length()) {
    return false;
  }

  bool ok = false;
  double v = value.toDouble(&ok);
  if (!ok) {
    return false;
  }

  qDebug() << "UGLY!! Add setData API to KstVector!";
  double *d = const_cast<double*>(_vector->value());
  d[index.row()] = v;
  return true;
}


}

// vim: ts=2 sw=2 et
