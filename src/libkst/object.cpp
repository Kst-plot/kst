/***************************************************************************
              object.cpp: abstract base class for all Kst objects
                             -------------------
    begin                : May 25, 2003
    copyright            : (C) 2003 The University of Toronto
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

#include "kst_i18n.h"

#include "object.h"
#include "objectstore.h"

namespace Kst {

#if 0
static int i = 0;
#endif

const QString Object::staticTypeString = I18N_NOOP("Object");

Object::Object() :
  Shared(), KstRWLock(), NamedObject(),
  _store(0L)
{
}


Object::~Object() {
  if (_store) {
    _store->removeObject(this);
  }
}


QString Object::type() {
  return staticMetaObject.className();
}


const QString& Object::typeString() const {
  return staticTypeString;
}


// Returns count - 2 to account for "this" and the list pointer, therefore
// you MUST have a reference-counted pointer to call this function
int Object::getUsage() const {
  return _KShared_count() - 1;
}


void Object::deleteDependents() {
}


ObjectStore* Object::store() const {
  return _store;
}

void Object::beginUpdate(ObjectPtr object) {
  processUpdate(object);
}

void Object::processUpdate(ObjectPtr object) {
  Q_UNUSED(object);
  // Do nothing by default.
}

}

// vim: ts=2 sw=2 et
