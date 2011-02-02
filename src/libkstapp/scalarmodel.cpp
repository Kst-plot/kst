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

#include <QFileInfo>

namespace Kst {


void ScalarModel::addDataSourcesMetas(DataSourcePtr dataSource, PrimitiveTreeItem* parent) {

  QStringList scalars = dataSource->scalar().list();
  if (scalars.isEmpty()) {
    return;
  }

  PrimitiveTreeItem* item = addPrimitiveTreeItem(QList<QVariant>() << dataSource->descriptiveName(), parent);

  scalars.sort();
  foreach(const QString& scalar, scalars) {
    double value;
    DataScalar::ReadInfo readInfo(&value);
    dataSource->scalar().read(scalar, readInfo);
    new PrimitiveTreeItem(QList<QVariant>() << scalar << value, item);
  }
}

// There are not yet any per-file scalars, though things like num_frames should be constant across a datasource
// and therefore availible as a per-file scalar... so, maybe, FIXME.
PrimitiveTreeItem* ScalarModel::addDataSourceFileItem(DataSourcePtr dataSource, PrimitiveTreeItem* parent)
{

  QString path = dataSource->descriptiveName();
  QFileInfo info(path);

  PrimitiveTreeItem* item = addPrimitiveTreeItem(QList<QVariant>() << info.fileName(), parent);
  //new PrimitiveTreeItem(QList<QVariant>() << "In directory" << info.path(), item);

  return item;
}



}

// vim: ts=2 sw=2 et
