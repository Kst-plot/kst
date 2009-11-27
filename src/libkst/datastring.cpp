/***************************************************************************
                          dataststring.cpp  -  a string from a data source
                             -------------------
    begin                : September, 2008
    copyright            : (C) 2008 by cbn
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
#include <QDebug>
#include <QTextDocument>
#include <QXmlStreamWriter>

#include "kst_i18n.h"
#include "datastring.h"
#include "debug.h"
#include "objectstore.h"

namespace Kst {

const QString DataString::staticTypeString = I18N_NOOP("Data String");
const QString DataString::staticTypeTag = I18N_NOOP("datastring");

/** Create a DataVector: raw data from a file */
DataString::DataString(ObjectStore *store)
: String(store), DataPrimitive() {

  _file = 0L;
  _field = QString::null;

  setOrphan(true);
}


DataString::~DataString() {
  _file = 0;
}


QString DataString::_automaticDescriptiveName() const {
  QString name = field();
  return name.replace("_", "\\_");
}


const QString& DataString::typeString() const {
  return staticTypeString;
}


/** return true if it has a valid file and field, or false otherwise */
bool DataString::isValid() const {
  if (_file) {
    _file->readLock();
    bool rc = _file->isValidString(_field);
    _file->unlock();
    return rc;
  }
  return false;
}

void DataString::change(DataSourcePtr in_file, const QString &in_field) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  _field = in_field;
  _file = in_file;
}

void DataString::changeFile(DataSourcePtr in_file) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (!in_file) {
    Debug::self()->log(i18n("Data file for string %1 was not opened.", Name()), Debug::Warning);
  }
  _file = in_file;
}


/** Save data string information */
void DataString::save(QXmlStreamWriter &s) {
  if (_file) {
    s.writeStartElement("datastring");
    _file->readLock();
    s.writeAttribute("file", _file->fileName());
    _file->unlock();
    s.writeAttribute("field", _field);

    saveNameInfo(s, XNUM);
    s.writeEndElement();
  }
}


/** Update a data String */
void DataString::internalUpdate() {
  if (_file) {
    _file->writeLock();
    _file->readString(_value, _field);
    _file->unlock();
  }
}

qint64 DataString::minInputSerial() const {
  if (_file) {
    return (_file->serial());
  }
  return LLONG_MAX;
}

qint64 DataString::minInputSerialOfLastChange() const {
  if (_file) {
    return (_file->serialOfLastChange());
  }
  return LLONG_MAX;
}



DataStringPtr DataString::makeDuplicate() const {
  Q_ASSERT(store());
  DataStringPtr string = store()->createObject<DataString>();

  string->writeLock();
  string->change(_file, _field);
  if (descriptiveNameIsManual()) {
    string->setDescriptiveName(descriptiveName());
  }

  string->registerChange();
  string->unlock();

  return string;
}


QString DataString::descriptionTip() const {
  QString IDstring;

  IDstring = i18n(
      "Data String: %1 = %4\n"
      "  %2\n"
      "  Field: %3"
  ).arg(Name()).arg(dataSource()->fileName()).arg(field()).arg(value());
  return IDstring;
}


QString DataString::propertyString() const {
  return i18n("%1 of %2").arg(_field).arg(dataSource()->fileName());
}

void DataString::reload() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (_file) {
    _file->writeLock();
    if (_file->reset()) { // try the efficient way first
      reset();
    } else { // the inefficient way
      DataSourcePtr newsrc = DataSource::loadSource(store(), _file->fileName(), _file->fileType());
      if (newsrc) {
        _file->unlock();
        if (store()) {
          store()->removeObject(_file);
        }
        _file = newsrc;
        _file->writeLock();
        reset();
      }
    }
    _file->unlock();
    registerChange();
  }
}

void DataString::reset() {
  _file->readString(_value, _field);
}

}
// vim: ts=2 sw=2 et
