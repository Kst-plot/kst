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
#include <QFileInfo>

namespace Kst {

PrimitiveTreeItem::PrimitiveTreeItem(const QList<QVariant> &data, PrimitiveTreeItem *parent) {
  parentItem = parent;
  itemData = data; 
  if (parent) {
    parent->addChild(this);
  }
}


PrimitiveTreeItem::~PrimitiveTreeItem() {
  qDeleteAll(childItems);
}


void PrimitiveTreeItem::addChild(PrimitiveTreeItem *item) {
  childItems.append(item);
}


PrimitiveTreeItem *PrimitiveTreeItem::child(int row) {
  return childItems.value(row);
}


int PrimitiveTreeItem::childCount() const {
  return childItems.count();
}


int PrimitiveTreeItem::row() const {
  if (parentItem)
    return parentItem->childItems.indexOf(const_cast<PrimitiveTreeItem*>(this));

  return 0;
}


int PrimitiveTreeItem::columnCount() const {
  return itemData.count();
}


QVariant PrimitiveTreeItem::data(int column) const {
  return itemData.value(column);
}


PrimitiveTreeItem *PrimitiveTreeItem::parent() {
  return parentItem;
}


PrimitiveModel::PrimitiveModel(ObjectStore *store)
  : QAbstractItemModel(), _store(store) {
    QList<QVariant> rootData;
    rootData << "Scalars";
    _rootItem = new PrimitiveTreeItem(rootData);
}


PrimitiveModel::~PrimitiveModel() {
}


int PrimitiveModel::columnCount(const QModelIndex& parent) const {
  Q_UNUSED(parent)
    return 2;
}


PrimitiveTreeItem* PrimitiveModel::addPrimitiveTreeItem(const QList<QVariant>& data, PrimitiveTreeItem* parent) {
  PrimitiveTreeItem* parentItem;
  if (parent) {
    parentItem = parent;
  } else {
    parentItem = _rootItem;
  }
  return new PrimitiveTreeItem(data, parentItem);
}


int PrimitiveModel::rowCount(const QModelIndex& parent) const {
  PrimitiveTreeItem *parentItem;
  if (parent.column() > 0)
    return 0;

  if (!parent.isValid())
    parentItem = _rootItem;
  else
    parentItem = static_cast<PrimitiveTreeItem*>(parent.internalPointer());

  return parentItem->childCount();
}


QVariant PrimitiveModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }

  if (role != Qt::DisplayRole) {
    return QVariant();
  }

  PrimitiveTreeItem *item = static_cast<PrimitiveTreeItem*>(index.internalPointer());

  return item->data(index.column());
}


QModelIndex PrimitiveModel::index(int row, int col, const QModelIndex& parent) const {
  if (row < 0 || col < 0 || col > 1) {
    return QModelIndex();
  }

  if (!hasIndex(row, col, parent))
    return QModelIndex();

  PrimitiveTreeItem *parentItem;

  if (!parent.isValid())
    parentItem = _rootItem;
  else
    parentItem = static_cast<PrimitiveTreeItem*>(parent.internalPointer());

  PrimitiveTreeItem *childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, col, childItem);
  else
    return QModelIndex();
}


QModelIndex PrimitiveModel::parent(const QModelIndex& index) const {
  Q_ASSERT(_store);

  if (!index.isValid())
    return QModelIndex();

  PrimitiveTreeItem *childItem = static_cast<PrimitiveTreeItem*>(index.internalPointer());
  PrimitiveTreeItem *parentItem = childItem->parent();

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
