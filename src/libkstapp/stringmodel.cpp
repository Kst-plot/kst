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

#include "stringmodel.h"

#include <assert.h>
#include <objectstore.h>
#include <dataobject.h>
#include <datavector.h>
#include <generatedvector.h>
#include <datamatrix.h>
#include <generatedmatrix.h>
#include <string_kst.h>

namespace Kst {


void StringModel::addDataSource(DataSourcePtr dataSource, ScalarTreeItem* parent) {
  ScalarTreeItem* item = addScalarTreeItem(QList<QVariant>() << dataSource->Name(), parent);

  QStringList scalars = dataSource->string().list();
  scalars.sort();
  foreach(QString scalar, scalars) {
    QList<QVariant> data;
    QString value;
    DataString::ReadInfo readInfo(&value);
    dataSource->string().read(scalar, readInfo);
    data << scalar << value;
    new ScalarTreeItem(data, item);
  }
}


}

// vim: ts=2 sw=2 et
