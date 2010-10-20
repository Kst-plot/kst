/***************************************************************************
                          dataststring.cpp  -  a string from a data source
                             -------------------
    begin                : September, 2008
    copyright            : (C) 2008 by cbn
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
: String(store) {

  _dp = new DataPrimitive(this);

  setOrphan(true);
}


DataString::~DataString() {
}


QString DataString::_automaticDescriptiveName() const {
  QString name = _dp->_field;
  return name.replace('_', "\\_");
}


const QString& DataString::typeString() const {
  return staticTypeString;
}


/** return true if it has a valid file and field, or false otherwise */
bool DataString::isValid() const {
  if (_dp->_file) {
    _dp->_file->readLock();
    bool rc = _dp->_file->string().isValid(_dp->_field);
    _dp->_file->unlock();
    return rc;
  }
  return false;
}


bool DataString::_checkValidity(const DataSourcePtr ds) const {
  if (ds) {
    ds->readLock();
    bool rc = ds->string().isValid(_dp->_field);
    ds->unlock();
    return rc;
  }
  return false;
}


void DataString::change(DataSourcePtr in_file, const QString &in_field) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  _dp->_field = in_field;
  _dp->_file = in_file;
}

void DataString::changeFile(DataSourcePtr in_file) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (!in_file) {
    Debug::self()->log(i18n("Data file for string %1 was not opened.", Name()), Debug::Warning);
  }
  _dp->_file = in_file;
}


/** Save data string information */
void DataString::save(QXmlStreamWriter &s) {
  if (_dp->_file) {
    s.writeStartElement("datastring");
    _dp->saveFilename(s);
    s.writeAttribute("field", _dp->_field);

    saveNameInfo(s, XNUM);
    s.writeEndElement();
  }
}


/** Update a data String */
void DataString::internalUpdate() {
  if (_dp->_file) {
    _dp->_file->writeLock();
    ReadInfo readInfo(&_value);
    _dp->_file->string().read(_dp->_field, readInfo);
    _dp->_file->unlock();
  }
}

qint64 DataString::minInputSerial() const {
  if (_dp->_file) {
    return (_dp->_file->serial());
  }
  return LLONG_MAX;
}

qint64 DataString::minInputSerialOfLastChange() const {
  if (_dp->_file) {
    return (_dp->_file->serialOfLastChange());
  }
  return LLONG_MAX;
}



PrimitivePtr DataString::_makeDuplicate() const {
  Q_ASSERT(store());
  DataStringPtr string = store()->createObject<DataString>();

  string->writeLock();
  string->change(_dp->_file, _dp->_field);
  if (descriptiveNameIsManual()) {
    string->setDescriptiveName(descriptiveName());
  }

  string->registerChange();
  string->unlock();

  return kst_cast<Primitive>(string);
}


QString DataString::descriptionTip() const {
  QString IDstring;

  IDstring = i18n(
      "Data String: %1 = %4\n"
      "  %2\n"
      "  Field: %3"
  ).arg(Name()).arg(_dp->dataSource()->fileName()).arg(_dp->_field).arg(value());
  return IDstring;
}


QString DataString::propertyString() const {
  return i18n("%1 of %2").arg(_dp->_field).arg(_dp->dataSource()->fileName());
}

void DataString::reload() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (_dp->_file) {
    _dp->_file->writeLock();
    _dp->_file->reset();
    _dp->_file->unlock();
    reset();
    registerChange();
  }
}

void DataString::reset() {
  ReadInfo readInfo(&_value);
  _dp->_file->string().read(_dp->_field, readInfo);
}

}
// vim: ts=2 sw=2 et
