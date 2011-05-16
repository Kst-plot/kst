/***************************************************************************
                              primitive.cpp
                             -------------------
    begin                : Tue Jun 20, 2006
    copyright            : Copyright (C) 2006, The University of Toronto
    email                : netterfield@astro.utoronto.ca
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

#include <limits.h>

#include "kst_i18n.h"
#include "primitive.h"
#include "updatemanager.h"
#include "datasource.h"

namespace Kst {

const QString Primitive::staticTypeString = I18N_NOOP("Primitive");

Primitive::Primitive(ObjectStore *store, Object *provider)
  : Object(), _provider(provider) {
  Q_UNUSED(store);
  _slaveName = "fixme: set _slaveName";
}


Primitive::~Primitive() {
}


const QString& Primitive::typeString() const {
  return staticTypeString;
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
    name = _provider->descriptiveName() + ':';
  }
  name += _slaveName;

  return name;
}

qint64 Primitive::minInputSerial() const {
  if (_provider) {
    return (_provider->serial());
  }
  return LLONG_MAX;
}

qint64 Primitive::maxInputSerialOfLastChange() const {
  if (_provider) {
    return (_provider->serialOfLastChange());
  }
  return NoInputs;
}


QString Primitive::propertyString() const {
  return QString("Base Class Property String");
}

QString Primitive::sizeString() const {
  return QString("Base Class Size String");
}

bool Primitive::used() const {
  if (_provider) {
    return true;
  } else {
    return Object::used();
  }
}




}

// vim: et sw=2 ts=2
