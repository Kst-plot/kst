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
#include "datascalar.h"

#include <QDebug>
#include <QTextDocument>
#include <QXmlStreamWriter>

#include "kst_i18n.h"
#include "debug.h"
#include "objectstore.h"

namespace Kst {

const QString DataScalar::staticTypeString = I18N_NOOP("Data Scalar");
const QString DataScalar::staticTypeTag = I18N_NOOP("datascalar");

/** Create a DataVector: raw data from a file */
DataScalar::DataScalar(ObjectStore *store)
: Scalar(store), DataPrimitive(this) {

  setOrphan(true);
}


DataScalar::~DataScalar() {
}


QString DataScalar::_automaticDescriptiveName() const {
  QString name = _field;

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
  if (dataSource()) {
    dataSource()->readLock();
    bool rc = dataSource()->scalar().isValid(_field);
    dataSource()->unlock();
    return rc;
  }
  return false;
}

bool DataScalar::checkValidity(const DataSourcePtr& ds) const {
  if (ds) {
    ds->readLock();
    bool rc = ds->scalar().isValid(_field);
    ds->unlock();
    return rc;
  }
  return false;
}

void DataScalar::change(DataSourcePtr in_file, const QString &in_field) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  _field = in_field;
  setDataSource(in_file);
}

void DataScalar::changeFile(DataSourcePtr in_file) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (!in_file) {
    Debug::self()->log(i18n("Data file for scalar %1 was not opened.", Name()), Debug::Warning);
  }
  setDataSource(in_file);
}


/** Save data scalar information */
void DataScalar::save(QXmlStreamWriter &s) {
  if (dataSource()) {
    s.writeStartElement("datascalar");
    saveFilename(s);
    s.writeAttribute("field", _field);

    saveNameInfo(s, XNUM);
    s.writeEndElement();
  }
}


/** Update a data Scalar */
void DataScalar::internalUpdate() {
  if (dataSource()) {
    dataSource()->writeLock();
    ReadInfo readInfo(&_value);
    dataSource()->scalar().read(_field, readInfo);
    dataSource()->unlock();
  }
}


PrimitivePtr DataScalar::makeDuplicate() const {
  Q_ASSERT(store());
  DataScalarPtr scalar = store()->createObject<DataScalar>();

  scalar->writeLock();
  scalar->change(dataSource(), _field);
  if (descriptiveNameIsManual()) {
    scalar->setDescriptiveName(descriptiveName());
  }

  scalar->registerChange();
  scalar->unlock();

  return kst_cast<Primitive>(scalar);
}

qint64 DataScalar::minInputSerial() const {
  if (dataSource()) {
    return (dataSource()->serial());
  }
  return LLONG_MAX;
}

qint64 DataScalar::maxInputSerialOfLastChange() const {
  if (dataSource()) {
    return (dataSource()->serialOfLastChange());
  }
  return NoInputs;
}

QString DataScalar::descriptionTip() const {
  QString IDstring;

  IDstring = i18n(
      "Data Scalar: %1 = %4\n"
      "  %2\n"
      "  Field: %3"
  ).arg(Name()).arg(dataSource()->fileName()).arg(_field).arg(value());
  return IDstring;
}


QString DataScalar::propertyString() const {
  return i18n("%2 of %1 = %3").arg(dataSource()->fileName()).arg(_field).arg(value());
}

void DataScalar::reload() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (dataSource()) {
    dataSource()->writeLock();
    dataSource()->reset();
    dataSource()->unlock();
    reset();
    registerChange();
  }
}

void DataScalar::reset() {
    ReadInfo readInfo(&_value);
    dataSource()->scalar().read(_field, readInfo);
}

}
// vim: ts=2 sw=2 et
