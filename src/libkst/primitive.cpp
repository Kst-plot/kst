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
#include "updatemanager.h"

#include <assert.h>

namespace Kst {

const QString Primitive::staticTypeString = I18N_NOOP("Primitive");

Primitive::Primitive(ObjectStore *store, Object *provider)
  : _provider(provider) {
  Q_UNUSED(store);
  _slaveName = "fixme: set _slaveName";
}


Primitive::~Primitive() {
}


const QString& Primitive::typeString() const {
  return staticTypeString;
}


Object::UpdateType Primitive::update() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);
  Object::UpdateType rc = internalUpdate(UPDATE);
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

void Primitive::triggerUpdateSignal(ObjectPtr object) {
#if DEBUG_UPDATE_CYCLE > 1
  qDebug() << "UP - Primitive" << shortName() << "has been updated as part of update of" << object->shortName() << "informing dependents";
#endif
  emit updated(object);
}


void Primitive::immediateUpdate() {
  UpdateManager::self()->updateStarted(this,this);
  update();
  triggerUpdateSignal(this);
  UpdateManager::self()->updateFinished(this,this);
}

QString Primitive::propertyString() const {
  return QString("Base Class Property String");
}

QString Primitive::sizeString() const {
  return QString("Base Class Size String");
}

}

// vim: et sw=2 ts=2
