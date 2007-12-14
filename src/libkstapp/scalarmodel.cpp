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
#include <objectstore.h>
#include <dataobject.h>
#include <datavector.h>
#include <generatedvector.h>
#include <datamatrix.h>
#include <generatedmatrix.h>

namespace Kst {

ScalarModel::ScalarModel(ObjectStore *store)
: QAbstractItemModel(), _store(store) {
  generateObjectList();
}


ScalarModel::~ScalarModel() {
}


int ScalarModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 2;
}


void ScalarModel::generateObjectList() {
  ObjectList<DataVector> dvol = _store->getObjects<DataVector>();
  ObjectList<GeneratedVector> gvol = _store->getObjects<GeneratedVector>();
  ObjectList<DataMatrix> dmol = _store->getObjects<DataMatrix>();
  ObjectList<GeneratedMatrix> gmol = _store->getObjects<GeneratedMatrix>();
  ObjectList<DataObject> dol = _store->getObjects<DataObject>();
  ObjectList<Scalar> sol = _store->getObjects<Scalar>();

  foreach(DataVector* vector, dvol) {
    _objectList.append(vector);
  }

  foreach(GeneratedVector* vector, gvol) {
    _objectList.append(vector);
  }

  foreach(DataMatrix* matrix, dmol) {
    _objectList.append(matrix);
  }

  foreach(GeneratedMatrix* matrix, gmol) {
    _objectList.append(matrix);
  }

  foreach(DataObject* dataObject, dol) {
    foreach(VectorPtr vector, dataObject->outputVectors()) {
      _objectList.append(vector);
    }
    foreach(MatrixPtr matrix, dataObject->outputMatrices()) {
      _objectList.append(matrix);
    }
  }

  foreach(Scalar* scalar, sol) {
    if (scalar->orphan()) {
      _objectList.append(scalar);
    }
  }
}

int ScalarModel::rowCount(const QModelIndex& parent) const {
  int rc = 0;
  if (!parent.isValid()) {
    rc = _objectList.count();
  } else if (!parent.parent().isValid()) {
    if (VectorPtr vector = kst_cast<Vector>(_objectList.at(parent.row()))) {
      vector->readLock();
      rc = vector->scalars().count();
      vector->unlock();
    } else if (MatrixPtr matrix = kst_cast<Matrix>(_objectList.at(parent.row()))) {
      matrix->readLock();
      rc = matrix->scalars().count();
      matrix->unlock();
    }
  }
  return rc;
}


QVariant ScalarModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  if (index.internalPointer()) {
    Object* object = static_cast<Object*>(index.internalPointer());
    if (!object) {
      return QVariant();
    }

    if (VectorPtr parent = kst_cast<Vector>(object)) {
      return vectorOutputData(parent, index);
    } else if (MatrixPtr parent = kst_cast<Matrix>(object)) {
      return matrixOutputData(parent, index);
    }
  }

  const int row = index.row();
  if (row < _objectList.count()) {
    if (ScalarPtr p = kst_cast<Scalar>(_objectList.at(row))) {
      return scalarData(p, index);
    } else if (ObjectPtr p = kst_cast<Object>(_objectList.at(row))) {
      return objectData(p, index);
    }
  }

  return QVariant();
}


QVariant ScalarModel::vectorOutputData(VectorPtr parent, const QModelIndex& index) const {
  QVariant rc;

  if (parent) {
    if (parent->scalars().count() > index.row()) {
      if (ScalarPtr scalar = parent->scalars().values()[index.row()]) {
        return scalarData(scalar, index);
      }
    }
  }

  return rc;
}


QVariant ScalarModel::matrixOutputData(MatrixPtr parent, const QModelIndex& index) const {
  QVariant rc;

  if (parent) {
    if (parent->scalars().count() > index.row()) {
      if (ScalarPtr scalar = parent->scalars().values()[index.row()]) {
        return scalarData(scalar, index);
      }
    }
  }

  return rc;
}


QVariant ScalarModel::objectData(ObjectPtr object, const QModelIndex& index) const {
  QVariant rc;

  if (object) {
    if (index.column() == Name) {
      object->readLock();
      rc.setValue(object->tag().displayString());
      object->unlock();
    }
  }

  return rc;
}


QVariant ScalarModel::scalarData(ScalarPtr scalar, const QModelIndex& index) const {
  QVariant rc;

  if (scalar) {
    if (index.column() == Name) {
      scalar->readLock();
      rc.setValue(scalar->tag().name());
      scalar->unlock();
    } else if (index.column() == Value) {
      scalar->readLock();
      rc = QVariant(scalar->value());
      scalar->unlock();
    }
  }

  return rc;
}


QModelIndex ScalarModel::index(int row, int col, const QModelIndex& parent) const {
  if (row < 0 || col < 0 || col > 1) {
    return QModelIndex();
  }

  const int count = _objectList.count();
  ObjectPtr object = 0;
  if (!parent.isValid()) {
    if (row < count) {
      return createIndex(row, col);
    }
  } else if (!parent.parent().isValid()) {
    if (parent.row() >= 0 && parent.row() < count) { 
      if (VectorPtr vector = kst_cast<Vector>(_objectList.at(parent.row()))) {
        vector->readLock();
        if (row < vector->scalars().count()) {
          object = vector;
        }
        vector->unlock();
      } else if (MatrixPtr matrix = kst_cast<Matrix>(_objectList.at(parent.row()))) {
        matrix->readLock();
        if (row < matrix->scalars().count()) {
          object = matrix;
        }
        matrix->unlock();
      }
    }
  }

  if (object) {
    return createIndex(row, col, object);
  } else {
    return QModelIndex();
  }
}


QModelIndex ScalarModel::parent(const QModelIndex& index) const {
  Q_ASSERT(_store);
  int row = -1;

  if (Vector *vector = static_cast<Vector*>(index.internalPointer())) {
    row = _objectList.indexOf(vector);
  } else if (Matrix *matrix = static_cast<Matrix*>(index.internalPointer())) {
    row = _objectList.indexOf(matrix);
  }

  if (row < 0) {
    return QModelIndex();
  }
  return createIndex(row, index.column());
}


QVariant ScalarModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole) {
    return QAbstractItemModel::headerData(section, orientation, role);
  }
  switch (section) {
    case Name:
      return tr("Name");
    case Value:
      return tr("Value");
    default:
      break;
  }
  return QVariant();
}

}

// vim: ts=2 sw=2 et
