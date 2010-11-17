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

  // un-escape escaped special characters so they aren't escaped 2x.
  name.replace("\\_", "_").replace("\\^","^").replace("\\[", "[").replace("\\]", "]");
  // now escape the special characters.
  name.replace('_', "\\_").replace('^', "\\^").replace('[', "\\[").replace(']', "\\]");

  return name;
}


const QString& DataString::typeString() const {
  return staticTypeString;
}


/** return true if it has a valid file and field, or false otherwise */
bool DataString::isValid() const {
  if (_dp->dataSource()) {
    _dp->dataSource()->readLock();
    bool rc = _dp->dataSource()->string().isValid(_dp->_field);
    _dp->dataSource()->unlock();
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
  _dp->setDataSource(in_file);
}

void DataString::changeFile(DataSourcePtr in_file) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (!in_file) {
    Debug::self()->log(i18n("Data file for string %1 was not opened.", Name()), Debug::Warning);
  }
  _dp->setDataSource(in_file);
}


/** Save data string information */
void DataString::save(QXmlStreamWriter &s) {
  if (_dp->dataSource()) {
    s.writeStartElement("datastring");
    _dp->saveFilename(s);
    s.writeAttribute("field", _dp->_field);

    saveNameInfo(s, XNUM);
    s.writeEndElement();
  }
}


/** Update a data String */
void DataString::internalUpdate() {
  if (_dp->dataSource()) {
    _dp->dataSource()->writeLock();
    ReadInfo readInfo(&_value);
    _dp->dataSource()->string().read(_dp->_field, readInfo);
    _dp->dataSource()->unlock();
  }
}

qint64 DataString::minInputSerial() const {
  if (_dp->dataSource()) {
    return (_dp->dataSource()->serial());
  }
  return LLONG_MAX;
}

qint64 DataString::minInputSerialOfLastChange() const {
  if (_dp->dataSource()) {
    return (_dp->dataSource()->serialOfLastChange());
  }
  return LLONG_MAX;
}



PrimitivePtr DataString::_makeDuplicate() const {
  Q_ASSERT(store());
  DataStringPtr string = store()->createObject<DataString>();

  string->writeLock();
  string->change(_dp->dataSource(), _dp->_field);
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

  if (_dp->dataSource()) {
    _dp->dataSource()->writeLock();
    _dp->dataSource()->reset();
    _dp->dataSource()->unlock();
    reset();
    registerChange();
  }
}

void DataString::reset() {
  ReadInfo readInfo(&_value);
  _dp->dataSource()->string().read(_dp->_field, readInfo);
}

}
// vim: ts=2 sw=2 et
