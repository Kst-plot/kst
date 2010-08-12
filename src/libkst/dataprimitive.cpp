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
#include <QDir>


namespace Kst {


struct DataPrimitive::Private
{
  DataSourcePtr _file;

  static void saveFilename(const QString& fn, QXmlStreamWriter& s);
};


void DataPrimitive::Private::saveFilename(const QString& fn, QXmlStreamWriter& s) {
  if (!fn.isEmpty()) {
    QDir current = QDir::current();
    QString currentP = QDir::currentPath();
    QString relFn = current.relativeFilePath(fn);
    s.writeAttribute("file", fn);
    if (QDir::isRelativePath(relFn)) { // is absolute if on a differnt disk/network 
        s.writeAttribute("fileRelative", relFn);
    }
  }
}





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
    DataPrimitive::Private::saveFilename(d._file->fileName(), s);
    file()->unlock();
  }
}


void DataPrimitive::saveFilename(const QString& fileName, QXmlStreamWriter& s)
{
    DataPrimitive::Private::saveFilename(fileName, s);
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
