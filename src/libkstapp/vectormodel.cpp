/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
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
: QAbstractTableModel () {
  addVector(v);
}


VectorModel::~VectorModel() {
}

bool VectorModel::addVector(Vector *v)
{
  assert(v);
  if (!_vectorList.contains(v)) {
    beginInsertColumns(QModelIndex(), columnCount(), columnCount()+1);
    _vectorList.append(v);
    insertColumn(columnCount());
    endInsertColumns();
  }
  reset();
  return true;
}

bool VectorModel::removeVector(Vector *v)
{
    return false;
}

bool VectorModel::insertColumn(int column, const QModelIndex &parent)
{
  return true;
}


int VectorModel::columnCount(const QModelIndex&) const {
  return _vectorList.length();
}


int VectorModel::rowCount(const QModelIndex&) const {
  int length = 0;
  for(int i=0;i<_vectorList.length();i++) {
      length = qMax(length, _vectorList.at(i)->length());
  }
  return length;
}


QVariant VectorModel::data(const QModelIndex& index, int role) const {
  if (index.isValid() && !_vectorList.isEmpty()) {
    switch (role) {
      case Qt::DisplayRole:
//        if (index.column() == 0) {
//          return QVariant(index.row());
//        } else {
          return QVariant(_vectorList.at(index.column())->value(index.row()));
//        }
        break;
      case Qt::FontRole:
        {
          if (index.column() > 0) {
            if (_vectorList.at(index.column())->editable()) {
              QFont f;
              f.setBold(true);
              return QVariant(f);
            }
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
  if (_vectorList.isEmpty() || role != Qt::DisplayRole || orientation == Qt::Vertical) {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
//  if (section == 0) {
//    return QVariant("Index");
//  } else {
    return QVariant(_vectorList.at(section)->Name());
//  }
//  return QVariant();
}


Qt::ItemFlags VectorModel::flags(const QModelIndex& index) const {
  Qt::ItemFlags f = QAbstractItemModel::flags(index);
  if (!_vectorList.isEmpty() || !index.isValid()) {
    return f;
  }

//  if (_vector->editable() && index.row() >= 0 && index.row() < _vector->length()) {
//    f |= Qt::ItemIsEditable;
//  }

  return f;
}


bool VectorModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (role != Qt::EditRole) {
    return QAbstractItemModel::setData(index, value, role);
  }

  if (!_vectorList.isEmpty() || !index.isValid() || !_vectorList.at(index.column())->editable() || index.row() < 0 || index.row() >= rowCount()) {
    return false;
  }

  bool ok = false;
  double v = value.toDouble(&ok);
  if (!ok) {
    return false;
  }

  qDebug() << "UGLY!! Add setData API to KstVector!";
  double *d = const_cast<double*>(_vectorList.at(index.column())->value());
  d[index.row()] = v;
  return true;
}

void VectorModel::beginInsertColumns(const QModelIndex &parent, int first, int last)
{
}

void VectorModel::endInsertColumns()
{
}


}

// vim: ts=2 sw=2 et
