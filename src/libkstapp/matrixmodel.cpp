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

#include "matrixmodel.h"

#include <assert.h>

#include <QFont>

namespace Kst {

MatrixModel::MatrixModel(MatrixPtr m)
: QAbstractItemModel(), _m(m) {
  assert(m.data());
}


MatrixModel::~MatrixModel() {
}


int MatrixModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return _m->xNumSteps();
}


int MatrixModel::rowCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return _m->yNumSteps();
}


QVariant MatrixModel::data(const QModelIndex& index, int role) const {
  Q_UNUSED(role)
  QVariant rc;
  if (index.isValid()) {
    switch (role) {
      case Qt::DisplayRole:
        rc = QVariant(_m->value(index.column(), index.row()));
        break;
      case Qt::FontRole:
        {
          if (_m->editable()) {
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


QModelIndex MatrixModel::index(int row, int col, const QModelIndex& parent) const {
  Q_UNUSED(parent)
  if (row >= 0 && row < _m->yNumSteps() && col >= 0 && col < _m->xNumSteps()) {
    return createIndex(row, col);
  }
  return QModelIndex();
}


QModelIndex MatrixModel::parent(const QModelIndex& index) const {
  Q_UNUSED(index)
  return QModelIndex();
}


QVariant MatrixModel::headerData(int section, Qt::Orientation orientation, int role) const {
  return QAbstractItemModel::headerData(section, orientation, role);
}


Qt::ItemFlags MatrixModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags f = QAbstractItemModel::flags(index);
  if (!index.isValid()) {
    return f;
  }

  if (_m->editable() && index.row() >= 0 && index.row() < _m->yNumSteps() && index.column() >= 0 && index.column() < _m->xNumSteps()) {
    f |= Qt::ItemIsEditable;
  }

  return f;
}


bool MatrixModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (role != Qt::EditRole) {
    return QAbstractItemModel::setData(index, value, role);
  }

  if (!index.isValid() || !_m->editable() || index.row() < 0 || index.row() >= _m->yNumSteps() || index.column() < 0 || index.column() >= _m->xNumSteps()) {
    return false;
  }

  bool ok = false;
  double v = value.toDouble(&ok);
  if (!ok) {
    return false;
  }

  return _m->setValue(index.column(), index.row(), v);
}


}

// vim: ts=2 sw=2 et
