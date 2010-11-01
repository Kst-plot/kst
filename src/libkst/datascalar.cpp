/***************************************************************************
                          datastscalar.cpp  -  a scalar from a data source
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
#include "datascalar.h"
#include "debug.h"
#include "objectstore.h"

namespace Kst {

const QString DataScalar::staticTypeString = I18N_NOOP("Data Scalar");
const QString DataScalar::staticTypeTag = I18N_NOOP("datascalar");

/** Create a DataVector: raw data from a file */
DataScalar::DataScalar(ObjectStore *store)
: Scalar(store) {

  _dp = new DataPrimitive(this);

  setOrphan(true);
}


DataScalar::~DataScalar() {
}


QString DataScalar::_automaticDescriptiveName() const {
  QString name = _dp->_field;

  // un-escape escaped special characters so they aren't escaped 2x.
  name.replace("\\_", "_").replace("\\^","^").replace("\\[", "[").replace("\\]", "]");
  // now escape the special characters.
  name.replace('_', "\\_").replace('^', "\\^").replace('[', "\\[").replace(']', "\\]");

  return name;
}


const QString& DataScalar::typeString() const {
  return staticTypeString;
}


/** return true if it has a valid file and field, or false otherwise */
bool DataScalar::isValid() const {
  if (_dp->_file) {
    _dp->_file->readLock();
    bool rc = _dp->_file->scalar().isValid(_dp->_field);
    _dp->_file->unlock();
    return rc;
  }
  return false;
}

bool DataScalar::_checkValidity(const DataSourcePtr ds) const {
  if (ds) {
    ds->readLock();
    bool rc = ds->scalar().isValid(_dp->_field);
    ds->unlock();
    return rc;
  }
  return false;
}

void DataScalar::change(DataSourcePtr in_file, const QString &in_field) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  _dp->_field = in_field;
  _dp->_file = in_file;
}

void DataScalar::changeFile(DataSourcePtr in_file) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (!in_file) {
    Debug::self()->log(i18n("Data file for scalar %1 was not opened.", Name()), Debug::Warning);
  }
  _dp->_file = in_file;
}


/** Save data scalar information */
void DataScalar::save(QXmlStreamWriter &s) {
  if (_dp->_file) {
    s.writeStartElement("datascalar");
    _dp->saveFilename(s);
    s.writeAttribute("field", _dp->_field);

    saveNameInfo(s, XNUM);
    s.writeEndElement();
  }
}


/** Update a data Scalar */
void DataScalar::internalUpdate() {
  if (_dp->_file) {
    _dp->_file->writeLock();
    ReadInfo readInfo(&_value);
    _dp->_file->scalar().read(_dp->_field, readInfo);
    _dp->_file->unlock();
  }
}


PrimitivePtr DataScalar::_makeDuplicate() const {
  Q_ASSERT(store());
  DataScalarPtr scalar = store()->createObject<DataScalar>();

  scalar->writeLock();
  scalar->change(_dp->_file, _dp->_field);
  if (descriptiveNameIsManual()) {
    scalar->setDescriptiveName(descriptiveName());
  }

  scalar->registerChange();
  scalar->unlock();

  return kst_cast<Primitive>(scalar);
}

qint64 DataScalar::minInputSerial() const {
  if (_dp->_file) {
    return (_dp->_file->serial());
  }
  return LLONG_MAX;
}

qint64 DataScalar::minInputSerialOfLastChange() const {
  if (_dp->_file) {
    return (_dp->_file->serialOfLastChange());
  }
  return LLONG_MAX;
}

QString DataScalar::descriptionTip() const {
  QString IDstring;

  IDstring = i18n(
      "Data Scalar: %1 = %4\n"
      "  %2\n"
      "  Field: %3"
  ).arg(Name()).arg(_dp->dataSource()->fileName()).arg(_dp->_field).arg(value());
  return IDstring;
}


QString DataScalar::propertyString() const {
  return i18n("%2 of %1 = %3").arg(_dp->dataSource()->fileName()).arg(_dp->_field).arg(value());
}

void DataScalar::reload() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (_dp->_file) {
    _dp->_file->writeLock();
    _dp->_file->reset();
    _dp->_file->unlock();
    reset();
    registerChange();
  }
}

void DataScalar::reset() {
    ReadInfo readInfo(&_value);
    _dp->_file->scalar().read(_dp->_field, readInfo);
}

}
// vim: ts=2 sw=2 et
