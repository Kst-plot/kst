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
#include "updatemanager.h"

namespace Kst {

const QString DataString::staticTypeString = I18N_NOOP("Data String");
const QString DataString::staticTypeTag = I18N_NOOP("datastring");

/** Create a DataVector: raw data from a file */
DataString::DataString(ObjectStore *store)
: String(store) {

  _file = 0L;
  _field = QString::null;

  setOrphan(true);
  setDirty();
}


DataString::~DataString() {
  _file = 0;
}


QString DataString::_automaticDescriptiveName() const {
  return field();
}


const QString& DataString::typeString() const {
  return staticTypeString;
}


void DataString::change(DataSourcePtr in_file, const QString &in_field) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  _field = in_field;
  _file = in_file;

  if (_file) {
    _file->writeLock();
  }
  setDirty(true);
  if (_file) {
    _file->unlock();
  }

  if (in_file) {
    connect(in_file, SIGNAL(sourceUpdated(ObjectPtr)), this, SLOT(sourceUpdated(ObjectPtr)));
  }

}


void DataString::sourceUpdated(ObjectPtr object) {
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


void DataString::changeFile(DataSourcePtr in_file) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (!in_file) {
    Debug::self()->log(i18n("Data file for string %1 was not opened.", Name()), Debug::Warning);
  }
  _file = in_file;
  if (_file) {
    _file->writeLock();
  }
  setDirty(true);
  if (_file) {
    _file->unlock();
  }
}


/** return the name of the file */
QString DataString::filename() const {
  QString rc;
  if (_file) {
    _file->readLock();
    rc = _file->fileName();
    _file->unlock();
  }
  return rc;
}


/** return the field */
const QString& DataString::field() const {
  return _field;
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
Object::UpdateType DataString::update() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);
  QString old_value;
  Object::UpdateType rc = NO_CHANGE;
  old_value = _value;
  if (_file) {
    _file->writeLock();
    _file->readString(_value, _field);
    _file->unlock();
    if (dirty() || (_value != old_value)) {
      rc = UPDATE;
    } else {
      rc = NO_CHANGE;
    }
  } else {
    rc = NO_CHANGE;
  }


  setDirty(false);
  return rc;
}


DataStringPtr DataString::makeDuplicate() const {
  Q_ASSERT(store());
  DataStringPtr string = store()->createObject<DataString>();

  string->writeLock();
  string->change(_file, _field);
  if (descriptiveNameIsManual()) {
    string->setDescriptiveName(descriptiveName());
  }

  string->update();
  string->unlock();

  return string;
}


DataSourcePtr DataString::dataSource() const {
  return _file;
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


/** return true if it has a valid file and field, or false otherwise */
bool DataString::isValid() const {
  if (_file) {
    _file->readLock();
    bool rc = _file->isValidField(_field);
    _file->unlock();
    return rc;
  }
  return false;
}

QString DataString::propertyString() const {
  return i18n("%1 of %2").arg(_field).arg(dataSource()->fileName());
}
}
// vim: ts=2 sw=2 et
