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

namespace Kst {

DataPrimitive::DataPrimitive()
{
  _file = 0L;
}


DataPrimitive::~DataPrimitive() {
  _file = 0;
}


/** return the name of the file */
QString DataPrimitive::filename() const {
  QString rc;
  if (_file) {
    _file->readLock();
    rc = _file->fileName();
    _file->unlock();
  }
  return rc;
}


/** return the field */
const QString& DataPrimitive::field() const {
  return _field;
}


DataSourcePtr DataPrimitive::dataSource() const {
  return _file;
}

}
