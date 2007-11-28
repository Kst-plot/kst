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

Object::Object(const ObjectTag& tag) :
  QObject(), Shared(), KstRWLock(),
  _lastUpdateCounter(0), _store(0L), _tag(tag)
{
  _dirty = false;
  _lastUpdate = Object::NO_CHANGE;
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


#if 0
int Object::operator==(const QString& tag) const {
  return (tag == _tag.tagString() || tag == _tag.displayString()) ? 1 : 0;
}
#endif


// Returns true if update has already been done
bool Object::checkUpdateCounter(int update_counter) {
  if (update_counter == _lastUpdateCounter) {
    return true;
  } else if (update_counter > 0) {
    _lastUpdateCounter = update_counter;
  }
  return false;
}


#if 0
inline QString Object::tagName() const {
  return _tag.tag();
}
#endif


ObjectTag& Object::tag() {
  return _tag;
}


const ObjectTag& Object::tag() const {
  return _tag;
}


void Object::setTagName(const ObjectTag& tag) {
  if (tag == _tag) {
    return;
  }

  _tag = tag;
  setObjectName(_tag.tagString().toLocal8Bit().data());

  if (_store) {
    // handle rename through object store
    _store->renameObject(this, tag);
  }
}


#if 0
QString Object::tagLabel() const {
  return QString("[%1]").arg(_tag.tagString());
}
#endif


// Returns count - 2 to account for "this" and the list pointer, therefore
// you MUST have a reference-counted pointer to call this function
int Object::getUsage() const {
  return _KShared_count() - 1;
}


void Object::deleteDependents() {
}


Object::UpdateType Object::setLastUpdateResult(UpdateType result) {
  return _lastUpdate = result;
}


Object::UpdateType Object::lastUpdateResult() const {
  return _lastUpdate;
}


void Object::setDirty(bool dirty) {
  _dirty = dirty;
}


bool Object::dirty() const {
  return _dirty;
}


ObjectStore* Object::store() const {
  return _store;
}

}

// vim: ts=2 sw=2 et
