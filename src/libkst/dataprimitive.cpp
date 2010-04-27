/***************************************************************************
                          datastprimitive.cpp  -  add datasource handling
                          capability to a primitive
                             -------------------
    begin                : October, 2009
    copyright            : (C) 2009 by cbn
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dataprimitive.h"

#include "datasource.h"

namespace Kst {


struct DataPrimitive::Private
{
  DataSourcePtr _file;
};



DataPrimitive::DataPrimitive() : d(*new Private)
{
  d._file = 0;
  _field = QString::null;
}


DataPrimitive::~DataPrimitive() {
  _field = QString::null;
  d._file = 0;
  delete &d;
}


/** return the name of the file */
QString DataPrimitive::filename() const {
  QString rc;
  if (d._file) {
    d._file->readLock();
    rc = d._file->fileName();
    d._file->unlock();
  }
  return rc;
}


/** return the field */
const QString& DataPrimitive::field() const {
  return _field;
}


DataSourcePtr DataPrimitive::dataSource() const {
  return d._file;
}

DataSourcePtr& DataPrimitive::file() {
  return d._file;
}

DataSourcePtr& DataPrimitive::file() const {
  return d._file;
}

}
