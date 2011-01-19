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

#include "primitivemodel.h"

#include <assert.h>
#include <objectstore.h>
#include <dataobject.h>
#include <datavector.h>
#include <generatedvector.h>
#include <datamatrix.h>
#include <generatedmatrix.h>
#include <datasource.h>

namespace Kst {

ScalarTreeItem::ScalarTreeItem(const QList<QVariant> &data, ScalarTreeItem *parent) {
  parentItem = parent;
  itemData = data; 
  if (parent) {
    parent->addChild(this);
  }
}


ScalarTreeItem::~ScalarTreeItem() {
  qDeleteAll(childItems);
}


void ScalarTreeItem::addChild(ScalarTreeItem *item) {
  childItems.append(item);
}


ScalarTreeItem *ScalarTreeItem::child(int row) {
    return childItems.value(row);
}

int ScalarTreeItem::childCount() const {
    return childItems.count();
}


int ScalarTreeItem::row() const {
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<ScalarTreeItem*>(this));

    return 0;
}


int ScalarTreeItem::columnCount() const {
    return itemData.count();
}


QVariant ScalarTreeItem::data(int column) const {
    return itemData.value(column);
}

ScalarTreeItem *ScalarTreeItem::parent() {
    return parentItem;
}


PrimitiveModel::PrimitiveModel(ObjectStore *store)
: QAbstractItemModel(), _store(store) {
  QList<QVariant> rootData;
  rootData << "Scalars";
  _rootItem = new ScalarTreeItem(rootData);
}


PrimitiveModel::~PrimitiveModel() {
}


int PrimitiveModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
  return 2;
}


ScalarTreeItem* PrimitiveModel::addScalarTreeItem(const QList<QVariant>& data, ScalarTreeItem* parent) {
  ScalarTreeItem* parentItem;
  if (parent) {
    parentItem = parent;
  } else {
    parentItem = _rootItem;
  }
  return new ScalarTreeItem(data, parentItem);
}




int PrimitiveModel::rowCount(const QModelIndex& parent) const {
  ScalarTreeItem *parentItem;
  if (parent.column() > 0)
      return 0;

  if (!parent.isValid())
      parentItem = _rootItem;
  else
      parentItem = static_cast<ScalarTreeItem*>(parent.internalPointer());

  return parentItem->childCount();
}


QVariant PrimitiveModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  ScalarTreeItem *item = static_cast<ScalarTreeItem*>(index.internalPointer());

  return item->data(index.column());
}


QModelIndex PrimitiveModel::index(int row, int col, const QModelIndex& parent) const {
  if (row < 0 || col < 0 || col > 1) {
    return QModelIndex();
  }

  if (!hasIndex(row, col, parent))
      return QModelIndex();

  ScalarTreeItem *parentItem;

  if (!parent.isValid())
      parentItem = _rootItem;
  else
      parentItem = static_cast<ScalarTreeItem*>(parent.internalPointer());

  ScalarTreeItem *childItem = parentItem->child(row);
  if (childItem)
      return createIndex(row, col, childItem);
  else
      return QModelIndex();
}


QModelIndex PrimitiveModel::parent(const QModelIndex& index) const {
  Q_ASSERT(_store);

  if (!index.isValid())
      return QModelIndex();

  ScalarTreeItem *childItem = static_cast<ScalarTreeItem*>(index.internalPointer());
  ScalarTreeItem *parentItem = childItem->parent();

  if (parentItem == _rootItem)
      return QModelIndex();

  return createIndex(parentItem->row(), 0, parentItem);
}


QVariant PrimitiveModel::headerData(int section, Qt::Orientation orientation, int role) const {
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
