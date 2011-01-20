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


void StringModel::addDataSourcesMetas(DataSourcePtr dataSource, PrimitiveTreeItem* parent) {

  PrimitiveTreeItem* item = addDataSourceFileItem(dataSource, parent);

  QStringList strings = dataSource->string().list();
  if (strings.isEmpty()) {
    return;
  }


  strings.sort();
  foreach(const QString& str, strings) {
    QString value;
    DataString::ReadInfo readInfo(&value);
    dataSource->string().read(str, readInfo);
    new PrimitiveTreeItem(QList<QVariant>() << str << value, item);
  }
}


}

// vim: ts=2 sw=2 et
