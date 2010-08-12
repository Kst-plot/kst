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

#include <QXmlStreamWriter>

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


void DataPrimitive::saveFilename(QXmlStreamWriter& s) {
  if (d._file) {
    file()->readLock();
    s.writeAttribute("file", d._file->fileName());
    file()->unlock();
  }
}


void DataPrimitive::saveFilename(const QString& fileName, QXmlStreamWriter& s)
{
    s.writeAttribute("file", fileName);
}


QString DataPrimitive::readFilename(const QXmlStreamAttributes& attrs)
{
    return attrs.value("file").toString();
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
