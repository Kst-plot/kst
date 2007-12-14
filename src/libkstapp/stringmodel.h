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

#ifndef STRINGMODEL_H
#define STRINGMODEL_H

#include <QAbstractItemModel>
#include "dataobject.h"
#include "object.h"

namespace Kst {

class ObjectStore;

class StringModel : public QAbstractItemModel
{

  enum ColumnID { Name, Value };

public:
  StringModel(ObjectStore *store);
  ~StringModel();

  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int col, const QModelIndex& parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex& index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  void generateObjectList();

private:
  QVariant stringData(StringPtr string, const QModelIndex& index) const;

  ObjectStore *_store;
  ObjectList<Object> _objectList;
};

}

#endif

// vim: ts=2 sw=2 et
