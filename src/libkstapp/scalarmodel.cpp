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

#include "scalarmodel.h"

#include <assert.h>
#include <objectstore.h>
#include <dataobject.h>
#include <datavector.h>
#include <generatedvector.h>
#include <datamatrix.h>
#include <generatedmatrix.h>
#include <datasource.h>

namespace Kst {


void ScalarModel::addDataSourcesMetas(DataSourcePtr dataSource, PrimitiveTreeItem* parent) {
  PrimitiveTreeItem* item = addPrimitiveTreeItem(QList<QVariant>() << dataSource->Name(), parent);

  QStringList scalars = dataSource->scalar().list();
  scalars.sort();
  foreach(QString scalar, scalars) {
    QList<QVariant> data;
    double value;
    DataScalar::ReadInfo readInfo(&value);
    dataSource->scalar().read(scalar, readInfo);
    data << scalar << value;
    new PrimitiveTreeItem(data, item);
  }
}




}

// vim: ts=2 sw=2 et
