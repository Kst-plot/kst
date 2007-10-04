/***************************************************************************
                              kstprimitive.cpp
                             -------------------
    begin                : Tue Jun 20, 2006
    copyright            : Copyright (C) 2006, The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *   Permission is granted to link with any opensource library             *
 *                                                                         *
 ***************************************************************************/

//#define UPDATEDEBUG

#include "kstprimitive.h"

#include <assert.h>

#include <qdebug.h>


KstPrimitive::KstPrimitive(Kst::Object *provider)
: Kst::Object(), _provider(provider) {
}


KstPrimitive::~KstPrimitive() {
}


Kst::Object::UpdateType KstPrimitive::update(int update_counter) {
#ifdef UPDATEDEBUG
  qDebug() << "Updating Primitive " << tag().displayString() << endl;
#endif
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  if (Kst::Object::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  Kst::Object::UpdateType providerRC = NO_CHANGE;

  if (update_counter > 0) {
    Kst::ObjectPtr prov = Kst::ObjectPtr(_provider);  // use a KstObjectPtr to prevent provider being deleted during update
    if (prov) {
      KstWriteLocker pl(prov);

      providerRC = prov->update(update_counter);
      if (!force && providerRC == Kst::Object::NO_CHANGE) {
        return setLastUpdateResult(providerRC);
      }
    }
  }

  Kst::Object::UpdateType rc = internalUpdate(providerRC);
  setDirty(false);
  return rc;
}


Kst::Object::UpdateType KstPrimitive::internalUpdate(Kst::Object::UpdateType providerRC) {
  Q_UNUSED(providerRC)
  return setLastUpdateResult(NO_CHANGE);
}


// vim: et sw=2 ts=2
