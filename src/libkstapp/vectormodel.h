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

#ifndef VECTORMODEL_H
#define VECTORMODEL_H

#include <QAbstractTableModel>
#include <QPointer>
#include <vector.h>

namespace Kst {

class VectorModel : public QAbstractTableModel
{
public:
  VectorModel(Vector *v);
  ~VectorModel();

  bool addVector(Vector *v);
  bool removeVector(int order);
  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  QModelIndex parent(const QModelIndex& index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  Qt::ItemFlags flags(const QModelIndex& index) const;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

private:
  QList<Vector *> _vectorList;
};

}

#endif

// vim: ts=2 sw=2 et
