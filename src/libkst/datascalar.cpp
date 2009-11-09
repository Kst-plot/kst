/***************************************************************************
                          datastscalar.cpp  -  a scalar from a data source
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
#include "datascalar.h"
#include "debug.h"
#include "objectstore.h"
#include "updatemanager.h"

namespace Kst {

const QString DataScalar::staticTypeString = I18N_NOOP("Data Scalar");
const QString DataScalar::staticTypeTag = I18N_NOOP("datascalar");

/** Create a DataVector: raw data from a file */
DataScalar::DataScalar(ObjectStore *store)
: Scalar(store), DataPrimitive() {

  _field = QString::null;

  setOrphan(true);
}


DataScalar::~DataScalar() {
  _file = 0;
}


QString DataScalar::_automaticDescriptiveName() const {
  QString name = field();
  return name.replace("_", "\\_");
}


const QString& DataScalar::typeString() const {
  return staticTypeString;
}


/** return true if it has a valid file and field, or false otherwise */
bool DataScalar::isValid() const {
  if (_file) {
    _file->readLock();
    bool rc = _file->isValidScalar(_field);
    _file->unlock();
    return rc;
  }
  return false;
}

void DataScalar::change(DataSourcePtr in_file, const QString &in_field) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  _field = in_field;
  _file = in_file;

  if (in_file) {
    connect(in_file, SIGNAL(sourceUpdated(ObjectPtr)), this, SLOT(sourceUpdated(ObjectPtr)));
  }

}


void DataScalar::sourceUpdated(ObjectPtr object) {
  //qDebug() << "UP - Data Source update required by Vector" << shortName() << "for update of" << object->shortName();
  writeLock();
  UpdateManager::self()->updateStarted(object, this);
  if (update()) {
  //qDebug() << "UP - Vector" << shortName() << "has been updated as part of update of" << object->shortName() << "informing dependents";
    emit updated(object);
  }
  UpdateManager::self()->updateFinished(object, this);
  unlock();
}


void DataScalar::changeFile(DataSourcePtr in_file) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (!in_file) {
    Debug::self()->log(i18n("Data file for scalar %1 was not opened.", Name()), Debug::Warning);
  }
  _file = in_file;
}


/** Save data scalar information */
void DataScalar::save(QXmlStreamWriter &s) {
  if (_file) {
    s.writeStartElement("datascalar");
    _file->readLock();
    s.writeAttribute("file", _file->fileName());
    _file->unlock();
    s.writeAttribute("field", _field);

    saveNameInfo(s, XNUM);
    s.writeEndElement();
  }
}


/** Update a data Scalar */
Object::UpdateType DataScalar::update() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);
  Object::UpdateType rc = NO_CHANGE;
  if (_file) {
    _file->writeLock();
    _file->readScalar(_value, _field);
    _file->unlock();
    rc = UPDATE;
  }

  return rc;
}


DataScalarPtr DataScalar::makeDuplicate() const {
  Q_ASSERT(store());
  DataScalarPtr scalar = store()->createObject<DataScalar>();

  scalar->writeLock();
  scalar->change(_file, _field);
  if (descriptiveNameIsManual()) {
    scalar->setDescriptiveName(descriptiveName());
  }

  scalar->update();
  scalar->unlock();

  return scalar;
}


QString DataScalar::descriptionTip() const {
  QString IDstring;

  IDstring = i18n(
      "Data Scalar: %1 = %4\n"
      "  %2\n"
      "  Field: %3"
  ).arg(Name()).arg(dataSource()->fileName()).arg(field()).arg(value());
  return IDstring;
}


QString DataScalar::propertyString() const {
  return i18n("%2 of %1 = %3").arg(dataSource()->fileName()).arg(field()).arg(value());
}

void DataScalar::reload() {
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
    update();
  }
}

void DataScalar::reset() {
  _file->readScalar(_value, _field);
}

}
// vim: ts=2 sw=2 et
