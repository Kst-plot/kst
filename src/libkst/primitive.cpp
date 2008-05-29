/***************************************************************************
                              primitive.cpp
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
#include <QDebug>

#include "kst_i18n.h"
#include "primitive.h"

#include <assert.h>

namespace Kst {

const QString Primitive::staticTypeString = I18N_NOOP("Primitive");

Primitive::Primitive(ObjectStore *store, Object *provider)
  : _provider(provider) {
  _slaveName = "fixme: set _slaveName";
}


Primitive::~Primitive() {
}


const QString& Primitive::typeString() const {
  return staticTypeString;
}


Object::UpdateType Primitive::update() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  Object::UpdateType providerRC = NO_CHANGE;

  ObjectPtr prov = ObjectPtr(_provider);  // use a ObjectPtr to prevent provider being deleted during update
  if (prov) {
    KstWriteLocker pl(prov);

    providerRC = prov->update();
    if (!force && providerRC == Object::NO_CHANGE) {
      return providerRC;
    }
  }

  Object::UpdateType rc = internalUpdate(providerRC);
  setDirty(false);
  return rc;
}


Object::UpdateType Primitive::internalUpdate(Object::UpdateType providerRC) {
  Q_UNUSED(providerRC)
  return NO_CHANGE;
}

void Primitive::setProvider(Object* obj) {
  _provider = obj;
}

void Primitive::setSlaveName(QString slaveName) {
  _slaveName=slaveName;
}

QString Primitive::_automaticDescriptiveName() const {
  QString name;
  if (_provider) {
    name = _provider->descriptiveName() + "/";
  }
  name += _slaveName;

  return name;
}

}

// vim: et sw=2 ts=2
