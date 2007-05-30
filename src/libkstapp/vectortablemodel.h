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

#ifndef VECTORTABLEMODEL_H
#define VECTORTABLEMODEL_H

#include "vectormodel.h"

namespace Kst {

class VectorTableModel : public QAbstractItemModel
{
public:
  VectorTableModel();
  ~VectorTableModel();

  int columnCount(const QModelIndex& parent = QModelIndex()) const;
  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
  QModelIndex index(int row, int col, const QModelIndex& parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex& index) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  Qt::ItemFlags flags(const QModelIndex& index) const;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

  QVector<VectorModel*>& vectors() { return _vectors; }
  const QVector<VectorModel*>& vectors() const { return _vectors; }

private:
  QVector<VectorModel*> _vectors;
};

}

#endif

// vim: ts=2 sw=2 et
