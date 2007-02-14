/***************************************************************************
              kstobject.cpp: abstract base class for all Kst objects
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

#include "ksdebug.h"
#include "kstobject.h"

/** Tag globals */
const QChar KstObjectTag::tagSeparator = QChar('/');
const QChar KstObjectTag::tagSeparatorReplacement = QChar('_');

const QStringList KstObjectTag::globalTagContext = QStringList();
const QStringList KstObjectTag::constantTagContext = QStringList("CONSTANTS");
const QStringList KstObjectTag::orphanTagContext = QStringList();

const KstObjectTag KstObjectTag::invalidTag = KstObjectTag(QString::null, KstObjectTag::globalTagContext);


static int i = 0;

KstObject::KstObject() : KstShared(), QObject(), KstRWLock(),
                         _lastUpdateCounter(0),
                         _tag(tr("Object %1").arg(++i), KstObjectTag::globalTagContext)
{
  _dirty = false;
  _lastUpdate = KstObject::NO_CHANGE;
}


KstObject::~KstObject() {
}


int KstObject::operator==(const QString& tag) const {
  return (tag == _tag.tagString() || tag == _tag.displayString()) ? 1 : 0;
}


// Returns true if update has already been done
bool KstObject::checkUpdateCounter(int update_counter) {
  if (update_counter == _lastUpdateCounter) {
    return true;
  } else if (update_counter > 0) {
    _lastUpdateCounter = update_counter;
  }
  return false;
}


inline QString KstObject::tagName() const {
  return _tag.tag();
}


inline KstObjectTag& KstObject::tag() {
  return _tag;
}


inline const KstObjectTag& KstObject::tag() const {
  return _tag;
}


void KstObject::setTagName(const KstObjectTag& tag) {
  if (tag == _tag) {
    return;
  }

  _tag = tag;
  setName(_tag.tagString().local8Bit().data());
}


QString KstObject::tagLabel() const {
  return QString("[%1]").arg(_tag.tagString());
}


// Returns count - 2 to account for "this" and the list pointer, therefore
// you MUST have a reference-counted pointer to call this function
int KstObject::getUsage() const {
  return _KShared_count() - 1;
}


bool KstObject::deleteDependents() {
  return false;
}


KstObject::UpdateType KstObject::setLastUpdateResult(UpdateType result) {
  return _lastUpdate = result;
}


KstObject::UpdateType KstObject::lastUpdateResult() const {
  return _lastUpdate;
}


void KstObject::setDirty(bool dirty) {
  _dirty = dirty;
}


bool KstObject::dirty() const {
  return _dirty;
}


// vim: ts=2 sw=2 et
