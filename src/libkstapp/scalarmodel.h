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

#ifndef SCALARMODEL_H
#define SCALARMODEL_H

#include <QAbstractItemModel>
#include "dataobject.h"
#include "datasource.h"
#include "object.h"

namespace Kst {

class ObjectStore;

 class ScalarTreeItem
 {
 public:
     ScalarTreeItem(const QList<QVariant> &data, ScalarTreeItem *parent = 0);
     ~ScalarTreeItem();

     void addChild(ScalarTreeItem *child);

     ScalarTreeItem *child(int row);
     int childCount() const;
     int columnCount() const;
     QVariant data(int column) const;
     int row() const;
     ScalarTreeItem *parent();

 private:
     QList<ScalarTreeItem*> childItems;
     QList<QVariant> itemData;
     ScalarTreeItem *parentItem;
 };

class ScalarModel : public QAbstractItemModel
{

  enum ColumnID { Name, Value };

public:
  ScalarModel(ObjectStore *store);
  ~ScalarModel();

  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int col, const QModelIndex& parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex& index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  void createTree();
  void addScalar(ScalarPtr scalar, ScalarTreeItem* parent = 0);
  void addVector(VectorPtr vector, ScalarTreeItem* parent = 0);
  void addMatrix(MatrixPtr matrix, ScalarTreeItem* parent = 0);
  void addDataObject(DataObjectPtr dataObject, ScalarTreeItem* parent = 0);
  void addDataSource(DataSourcePtr dataSource, ScalarTreeItem* parent = 0);
  void addScalars(const QHash<QString, Kst::ScalarPtr> scalarMap, ScalarTreeItem* parent = 0);

private:
  ObjectStore *_store;
  ScalarTreeItem *_rootItem;

};

}

#endif

// vim: ts=2 sw=2 et
