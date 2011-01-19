/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2011 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef PRIMITIVE_MODEL_H
#define PRIMITIVE_MODEL_H

#include <QAbstractItemModel>
#include "dataobject.h"
#include "datasource.h"
#include "object.h"

namespace Kst {

class ObjectStore;

 class ScalarTreeItem
 {
 public:
     explicit ScalarTreeItem(const QList<QVariant> &data, ScalarTreeItem *parent = 0);
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

class PrimitiveModel : public QAbstractItemModel
{

  enum ColumnID { Name, Value };

public:
  PrimitiveModel(ObjectStore *store);
  ~PrimitiveModel();

  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int col, const QModelIndex& parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex& index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  template<class T>
  void createTree() {
    QList<ObjectPtr> objects = _store->objectList();
    foreach(const ObjectPtr& obj, objects) {
      if (kst_cast<Primitive>(obj)) {
        if (kst_cast<T>(obj) && !kst_cast<T>(obj)->orphan()) {
          continue;
        }
        addPrimitivesScalars<T>(kst_cast<Primitive>(obj));
      } else if (kst_cast<DataSource>(obj) && !kst_cast<DataSource>(obj)->scalar().list().isEmpty()) {
        addDataSource(kst_cast<DataSource>(obj));
      } else if (kst_cast<DataObject>(obj)) {
        addDataObject<T>(kst_cast<DataObject>(obj));
      }
    }
  }


  template<class T>
  void addScalar(T* scalar, ScalarTreeItem* parent = 0) {
    addScalarTreeItem(QList<QVariant>() << scalar->Name() << scalar->value(), parent);
  }

  template<class T>
  void addScalars(const PrimitiveMap& scalarMap, ScalarTreeItem* parent) {
    foreach(const PrimitivePtr& scalar, scalarMap) {
      if(kst_cast<T>(scalar)) {
        addScalar<T>(kst_cast<T>(scalar), parent);
      }
    }
  }

  template<class T>
  void addPrimitivesScalars(const PrimitivePtr& prim, ScalarTreeItem* parent = 0) {
    ScalarTreeItem* item = addScalarTreeItem(QList<QVariant>() << prim->Name(), parent);
    addScalars<T>(prim->metas(), item);
  }

  template<class T>
  void addDataObject(DataObjectPtr dataObject, ScalarTreeItem* parent = 0) {
    ScalarTreeItem* item = addScalarTreeItem(QList<QVariant>() << dataObject->Name(), parent);

    ObjectList<Primitive> primitives = dataObject->outputPrimitives();
    foreach(PrimitivePtr prim, primitives) {
      if (!kst_cast<String>(prim)) {
        addPrimitivesScalars<T>(prim.data(), item);
      }
    }
  }
  
  virtual void addDataSource(DataSourcePtr dataSource, ScalarTreeItem* parent = 0) = 0;

protected:
  ScalarTreeItem* addScalarTreeItem(const QList<QVariant>& data, ScalarTreeItem* parent);

private:
  ObjectStore *_store;
  ScalarTreeItem *_rootItem;
};



}

#endif

// vim: ts=2 sw=2 et
