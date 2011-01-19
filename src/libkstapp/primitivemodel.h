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

 class PrimitiveTreeItem
 {
 public:
     explicit PrimitiveTreeItem(const QList<QVariant> &data, PrimitiveTreeItem *parent = 0);
     ~PrimitiveTreeItem();

     void addChild(PrimitiveTreeItem *child);

     PrimitiveTreeItem *child(int row);
     int childCount() const;
     int columnCount() const;
     QVariant data(int column) const;
     int row() const;
     PrimitiveTreeItem *parent();

 private:
     QList<PrimitiveTreeItem*> childItems;
     QList<QVariant> itemData;
     PrimitiveTreeItem *parentItem;
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
  void createTree();

  template<class T>
  void addMeta(T* m, PrimitiveTreeItem* parent = 0);

  template<class T>
  void addMetas(const PrimitiveMap& metarMap, PrimitiveTreeItem* parent);

  template<class T>
  void addPrimitivesMetas(const PrimitivePtr& prim, PrimitiveTreeItem* parent = 0);

  template<class T>
  void addDataObjectsMetas(DataObjectPtr dataObject, PrimitiveTreeItem* parent = 0);
  
  virtual void addDataSourcesMetas(DataSourcePtr dataSource, PrimitiveTreeItem* parent = 0) = 0;

protected:
  PrimitiveTreeItem* addPrimitiveTreeItem(const QList<QVariant>& data, PrimitiveTreeItem* parent);

private:
  ObjectStore *_store;
  PrimitiveTreeItem *_rootItem;
};



template<class T>
void PrimitiveModel::createTree() {
  QList<ObjectPtr> objects = _store->objectList();
  foreach(const ObjectPtr& obj, objects) {
    if (kst_cast<Primitive>(obj)) {
      if (kst_cast<T>(obj) && !kst_cast<T>(obj)->orphan()) {
        continue;
      }
      addPrimitivesMetas<T>(kst_cast<Primitive>(obj));
    } else if (kst_cast<DataSource>(obj) && !kst_cast<DataSource>(obj)->scalar().list().isEmpty()) {
      addDataSourcesMetas(kst_cast<DataSource>(obj));
    } else if (kst_cast<DataObject>(obj)) {
      addDataObjectsMetas<T>(kst_cast<DataObject>(obj));
    }
  }
}


template<class T>
void PrimitiveModel::addMeta(T* m, PrimitiveTreeItem* parent) {
  addPrimitiveTreeItem(QList<QVariant>() << m->Name() << m->value(), parent);
}

template<class T>
void PrimitiveModel::addMetas(const PrimitiveMap& metarMap, PrimitiveTreeItem* parent) {
  foreach(const PrimitivePtr& m, metarMap) {
    if(kst_cast<T>(m)) {
      addMeta<T>(kst_cast<T>(m), parent);
    }
  }
}

template<class T>
void PrimitiveModel::addPrimitivesMetas(const PrimitivePtr& prim, PrimitiveTreeItem* parent) {
  PrimitiveTreeItem* item = addPrimitiveTreeItem(QList<QVariant>() << prim->Name(), parent);
  addMetas<T>(prim->metas(), item);
}

template<class T>
void PrimitiveModel::addDataObjectsMetas(DataObjectPtr dataObject, PrimitiveTreeItem* parent) {
  PrimitiveTreeItem* item = addPrimitiveTreeItem(QList<QVariant>() << dataObject->Name(), parent);

  ObjectList<Primitive> primitives = dataObject->outputPrimitives();
  foreach(PrimitivePtr prim, primitives) {
    if (!kst_cast<String>(prim)) {
      addPrimitivesMetas<T>(prim.data(), item);
    }
  }
}
  



}

#endif

// vim: ts=2 sw=2 et
