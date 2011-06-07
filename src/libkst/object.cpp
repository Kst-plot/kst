/***************************************************************************
              object.cpp: abstract base class for all Kst objects
                             -------------------
    begin                : May 25, 2003
    copyright            : (C) 2003 The University of Toronto
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

#include "object.h"
#include "kst_i18n.h"

#include "objectstore.h"

namespace Kst {

const QString Object::staticTypeString = I18N_NOOP("Object");

Object::Object() :
  Shared(), KstRWLock(), NamedObject(),
  _store(0L), _serial(0), _serialOfLastChange(0)
{
}


Object::~Object() {
}


QString Object::type() {
  return staticMetaObject.className();
}

void Object::reset() {
  _serial = _serialOfLastChange = Forced;
}

const QString& Object::typeString() const {
  return staticTypeString;
}


// Returns count - 1 to account for "this" and the list pointer, therefore
// you MUST have a reference-counted pointer to call this function
int Object::getUsage() const {
  return _KShared_count() - 1;
}


void Object::deleteDependents() {
  QList<ObjectPtr> Objects = _store->objectList();
  foreach (ObjectPtr object, Objects) {
    if (object->uses(this)) {
      _store->removeObject(object);
    }
  }
}


bool Object::uses(ObjectPtr p) const {
   Q_UNUSED(p)

   return false;
}


ObjectStore* Object::store() const {
  return _store;
}

// decide, based on serial numbers, whether to do an update.
// if all inputs are up to date, update.  Otherwise, defer.
Object::UpdateType Object::objectUpdate(qint64 newSerial) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (newSerial == _serial) {
    return NoChange;
  }

  if (newSerial == Forced) { // register the forced update, but don't do it now.
    _serial = Forced;
    return Deferred;
  } else if (minInputSerial() < newSerial) { // if an input was forced, this will be true
    return Deferred;
  } else if ((_serialOfLastChange < maxInputSerialOfLastChange()) || (_serial == Object::Forced)) {
    internalUpdate();
    _serialOfLastChange = newSerial;
    _serial = newSerial;
    return Updated;
  }
  // else
  _serial = newSerial;
  return NoChange;
}

}

// vim: ts=2 sw=2 et
