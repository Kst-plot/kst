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

#ifndef SESSIONMODEL_H
#define SESSIONMODEL_H


#include <QAbstractItemModel>
#include "dataobject.h"
#include "object.h"
#include "relation.h"

namespace Kst {

class ObjectStore;

class SessionModel : public QAbstractItemModel
{
public:
  SessionModel(ObjectStore *store);
  ~SessionModel();

  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int col, const QModelIndex& parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex& index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  void generateObjectList();
  void triggerReset();
  ObjectList<Object> *objectList() {return &_objectList;}
private:
  QVariant dataObjectOutputData(DataObjectPtr parent, const QModelIndex& index) const;
  QVariant primitiveData(PrimitivePtr parent, const QModelIndex& index) const;
  QVariant dataObjectData(DataObjectPtr dataObject, const QModelIndex& index) const;
  QVariant relationData(RelationPtr relation, const QModelIndex& index) const;

  ObjectList<Object> _objectList;

  ObjectStore *_store;
};

}

#endif

// vim: ts=2 sw=2 et
