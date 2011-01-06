/***************************************************************************
                          datastprimitive.cpp  -  add datasource handling
                          capability to a primitive
                             -------------------
    begin                : October, 2009
    copyright            : (C) 2009 by cbn
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"
#include "kst_i18n.h"

#include "dataprimitive.h"

#include "datasource.h"

#include <QXmlStreamWriter>
#include <QDir>


namespace Kst {


struct DataPrimitive::Private
{
  Primitive* _primitive;
  DataSourcePtr _file;

  static void saveFilename(const QString& fn, QXmlStreamWriter& s);
};


void DataPrimitive::Private::saveFilename(const QString& fn, QXmlStreamWriter& s) {
  if (!fn.isEmpty()) {
    // QDir::current is set to *.kst's file path in mainwindow.cpp
    QDir current = QDir::current();
    QString relFn = current.relativeFilePath(fn);
    s.writeAttribute("file", current.absoluteFilePath(fn));
    if (QDir::isRelativePath(relFn)) { // is absolute if on a differnt disk/network 
        s.writeAttribute("fileRelative", relFn);
    }
  }
}


DataPrimitive::DataPrimitive(Primitive* primitive) : d(*new Private)
{
  d._file = 0;
  d._primitive = primitive;
  _field.clear();
}


DataPrimitive::~DataPrimitive() {
  _field = QString::null;
  d._file = 0;
  d._primitive = 0;
  delete &d;
}


void DataPrimitive::changeFile(DataSourcePtr in_file) {
  Q_ASSERT(d._primitive->myLockStatus() == KstRWLock::WRITELOCKED);

  if (!in_file) {
    Debug::self()->log(i18n("Data file for vector %1 was not opened.", d._primitive->Name()), Debug::Warning);
  }
  d._file = in_file;
  if (d._file) {
    d._file->writeLock();
  }
  d._primitive->reset();
  if (d._file) {
    d._file->unlock();
  }
  d._primitive->registerChange();
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
    d._file->readLock();
    DataPrimitive::Private::saveFilename(d._file->fileName(), s);
    d._file->unlock();
  }
}


void DataPrimitive::saveFilename(const QString& fileName, QXmlStreamWriter& s)
{
    DataPrimitive::Private::saveFilename(fileName, s);
}


QString DataPrimitive::readFilename(const QXmlStreamAttributes& attrs)
{
    // TODO discuss strategy: 
    //   - first try relative path
    //   - fall back to absolute path    

    // QDir::current is set to *.kst's file path in mainwindow.cpp
    QDir current = QDir::current();

    QString fnRel = attrs.value("fileRelative").toString();    

    if (!fnRel.isEmpty() && current.exists(fnRel)) {
        // internally only use absolute paths
        return current.absoluteFilePath(fnRel);
    }

    return attrs.value("file").toString();
}


/** return the field */
const QString& DataPrimitive::field() const {
  return _field;
}

DataSourcePtr DataPrimitive::dataSource() const {
  return d._file;
}

void DataPrimitive::setDataSource(const DataSourcePtr& file) {
  d._file = file;
}

}
