/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2006 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//#define UPDATEDEBUG

#include "primitive.h"

#include <assert.h>

#include <qdebug.h>

namespace Kst {

Primitive::Primitive(Object *provider)
: Object(), _provider(provider) {
}


Primitive::~Primitive() {
}


Object::UpdateType Primitive::update(int update_counter) {
#ifdef UPDATEDEBUG
  qDebug() << "Updating Primitive " << tag().displayString() << endl;
#endif
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  if (Object::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  Object::UpdateType providerRC = NO_CHANGE;

  if (update_counter > 0) {
    ObjectPtr prov = ObjectPtr(_provider);  // use a KstObjectPtr to prevent provider being deleted during update
    if (prov) {
      KstWriteLocker pl(prov);

      providerRC = prov->update(update_counter);
      if (!force && providerRC == Object::NO_CHANGE) {
        return setLastUpdateResult(providerRC);
      }
    }
  }

  Object::UpdateType rc = internalUpdate(providerRC);
  setDirty(false);
  return rc;
}


Object::UpdateType Primitive::internalUpdate(Object::UpdateType providerRC) {
  Q_UNUSED(providerRC)
  return setLastUpdateResult(NO_CHANGE);
}

}

// vim: et sw=2 ts=2
