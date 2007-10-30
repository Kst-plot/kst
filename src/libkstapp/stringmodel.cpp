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

#include "stringmodel.h"

#include <assert.h>
#include <QFont>

namespace Kst {

StringModel::StringModel(String *string)
: QAbstractItemModel(), _string(string) {
  assert(string);
}


StringModel::~StringModel() {
}


int StringModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 1;
}


int StringModel::rowCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 1;
}


QVariant StringModel::data(const QModelIndex& index, int role) const {
  QVariant rc;
  if (index.isValid() && _string) {
    switch (role) {
      case Qt::DisplayRole:
        if (index.column() == 0) {
          rc = QVariant(_string->tagName());
        } else {
          rc = QVariant(_string->value());
        }
        break;
      case Qt::FontRole:
        {
          if (_string->editable()) {
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


QModelIndex StringModel::index(int row, int col, const QModelIndex& parent) const {
  Q_UNUSED(parent)
  Q_UNUSED(col)
  if (_string) {
    return createIndex(row, 1);
  }
  return QModelIndex();
}


QModelIndex StringModel::parent(const QModelIndex& index) const {
  Q_UNUSED(index)
  return QModelIndex();
}


QVariant StringModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (!_string || role != Qt::DisplayRole || section != 0) {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
  return _string->tagName();
}


Qt::ItemFlags StringModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags f = QAbstractItemModel::flags(index);
  if (!_string || !index.isValid()) {
    return f;
  }

  if (_string->editable() && index.row() >= 0) {
    f |= Qt::ItemIsEditable;
  }

  return f;
}


bool StringModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (role != Qt::EditRole) {
    return QAbstractItemModel::setData(index, value, role);
  }

  if (!_string || !index.isValid() || !_string->editable() || index.row() < 0) {
    return false;
  }

  QString stringValue = value.toString();
  qDebug() << "UGLY!! Add setData API to String!";
  _string->setValue(stringValue);
  return true;
}


}

// vim: ts=2 sw=2 et
