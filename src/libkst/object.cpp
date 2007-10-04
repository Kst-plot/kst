/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "object.h"

namespace Kst {

/** Tag globals */
const QChar ObjectTag::tagSeparator = QChar('/');
const QChar ObjectTag::tagSeparatorReplacement = QChar('_');

const QStringList ObjectTag::globalTagContext = QStringList();
const QStringList ObjectTag::constantTagContext = QStringList("CONSTANTS");
const QStringList ObjectTag::orphanTagContext = QStringList();

const ObjectTag ObjectTag::invalidTag = ObjectTag(QString::null, ObjectTag::globalTagContext);


static int i = 0;

Object::Object() : Shared(), QObject(), KstRWLock(),
                         _lastUpdateCounter(0),
                         _tag(tr("Object %1").arg(++i), ObjectTag::globalTagContext)
{
  _dirty = false;
  _lastUpdate = Object::NO_CHANGE;
}


Object::~Object() {
}


int Object::operator==(const QString& tag) const {
  return (tag == _tag.tagString() || tag == _tag.displayString()) ? 1 : 0;
}


// Returns true if update has already been done
bool Object::checkUpdateCounter(int update_counter) {
  if (update_counter == _lastUpdateCounter) {
    return true;
  } else if (update_counter > 0) {
    _lastUpdateCounter = update_counter;
  }
  return false;
}


inline QString Object::tagName() const {
  return _tag.tag();
}


inline ObjectTag& Object::tag() {
  return _tag;
}


inline const ObjectTag& Object::tag() const {
  return _tag;
}


void Object::setTagName(const ObjectTag& tag) {
  if (tag == _tag) {
    return;
  }

  _tag = tag;
  setObjectName(_tag.tagString().toLocal8Bit().data());
}


QString Object::tagLabel() const {
  return QString("[%1]").arg(_tag.tagString());
}


// Returns count - 2 to account for "this" and the list pointer, therefore
// you MUST have a reference-counted pointer to call this function
int Object::getUsage() const {
  return _KShared_count() - 1;
}


bool Object::deleteDependents() {
  return false;
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

}
// vim: ts=2 sw=2 et
