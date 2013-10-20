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
#include <QMenu>

namespace Kst {

VectorModel::VectorModel()
  : QAbstractTableModel (), _rows(0) {
}


VectorModel::~VectorModel() {
}

bool VectorModel::addVector(VectorPtr v)
{
  assert(v);
  if (!_vectorList.contains(v)) {
    beginInsertColumns(QModelIndex(), columnCount(), columnCount());
    _vectorList.append(v);
    // Standard nb of digits after comma: 6
    _digitNbList.append(6);
    endInsertColumns();
    reset();
    _rows = rowCount();
    return true;
  }
  return false;
}

bool VectorModel::removeVector(int order)
{
  beginRemoveColumns(QModelIndex(), order, order);
  _vectorList.removeAt(order);
  _digitNbList.removeAt(order);
  endRemoveColumns();
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
        // If vector is shorter display nothing
        if (index.row() >= _vectorList.at(index.column())->length()) {
          return QVariant();
        } else {
            double value = _vectorList.at(index.column())->value(index.row());
            // Return string depending on number of digits, for 0 try to print as int
            switch (_digitNbList.at(index.column())) {
              case 0:
                // Cast to long int if not too big
                if (value < (double) LLONG_MAX && value > (double) -LLONG_MIN) {
                  return QVariant(QString::number((qlonglong) value));
                }
                else {
                  return QVariant(value);
                }
                break;
              default:
                return QVariant(QString::number(value, 'g', _digitNbList.at(index.column())));
                break;
            }
        }
        break;
      case Qt::FontRole:
        {
          if (_vectorList.at(index.column())->editable()) {
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
  if (_vectorList.isEmpty() || role != Qt::DisplayRole || orientation == Qt::Vertical) {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
  return QVariant(_vectorList.at(section)->Name());
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

void VectorModel::resetIfChanged() {
  if (_rows!=rowCount()) {
    reset();
    _rows = rowCount();
  }
}

void VectorModel::setDigitNumber(int column, int nbDigits) {
  _digitNbList[column] = nbDigits;
}

}

// vim: ts=2 sw=2 et
