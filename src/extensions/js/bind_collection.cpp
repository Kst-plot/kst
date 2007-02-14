/***************************************************************************
                             bind_collection.cpp
                             -------------------
    begin                : Mar 31 2005
    copyright            : (C) 2005 The University of Toronto
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

#include "bind_collection.h"

#include <kdebug.h>

KstBindCollection::KstBindCollection(KJS::ExecState *exec, const QString& name, bool readOnly)
: KstBinding(name, false), _readOnly(readOnly) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindCollection::KstBindCollection(int id)
: KstBinding("Collection Method", id) {
}


KstBindCollection::~KstBindCollection() {
}


struct CollectionBindings {
  const char *name;
  KJS::Value (KstBindCollection::*method)(KJS::ExecState*, const KJS::List&);
};


struct CollectionProperties {
  const char *name;
  void (KstBindCollection::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindCollection::*get)(KJS::ExecState*) const;
};


static CollectionBindings collectionBindings[] = {
  { "append", &KstBindCollection::append },
  { "prepend", &KstBindCollection::prepend },
  { "remove", &KstBindCollection::remove },
  { "clear", &KstBindCollection::clear },
  { 0L, 0L }
};


static CollectionProperties collectionProperties[] = {
  { "readOnly", 0L, &KstBindCollection::readOnly },
  { "length", 0L, &KstBindCollection::length },
  { 0L, 0L, 0L }
};


void KstBindCollection::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  if (!_readOnly) {
    for (int i = 0; collectionBindings[i].name != 0L; ++i) {
      KJS::Object o = KJS::Object(new KstBindCollection(i + 1));
      obj.put(exec, collectionBindings[i].name, o, KJS::Function);
    }
  }
}


KJS::ReferenceList KstBindCollection::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; collectionProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(collectionProperties[i].name)));
  }

  QStringList others = collection(exec);
  for (QStringList::ConstIterator i = others.begin(); i != others.end(); ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(*i)));
  }

  return rc;
}


bool KstBindCollection::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; collectionProperties[i].name; ++i) {
    if (prop == collectionProperties[i].name) {
      return true;
    }
  }

  return collection(exec).contains(prop) || KstBinding::hasProperty(exec, propertyName);
}


KJS::Value KstBindCollection::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindCollection *imp = dynamic_cast<KstBindCollection*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*collectionBindings[id - 1].method)(exec, args);
}


KJS::Value KstBindCollection::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (id() > 0) {
    return KstBinding::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; collectionProperties[i].name; ++i) {
    if (prop == collectionProperties[i].name) {
      if (!collectionProperties[i].get) {
        break;
      }
      return (this->*collectionProperties[i].get)(exec);
    }
  }
  
  KJS::Value v = extract(exec, propertyName);
  if (v.type() != KJS::UndefinedType) {
    return v;
  }
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindCollection::getPropertyByIndex(KJS::ExecState *exec, unsigned propertyName) const {
  return extract(exec, propertyName);
}


KJS::Value KstBindCollection::append(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
  exec->setException(eobj);
  return KJS::Undefined();
}


KJS::Value KstBindCollection::prepend(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
  exec->setException(eobj);
  return KJS::Undefined();
}


KJS::Value KstBindCollection::remove(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
  exec->setException(eobj);
  return KJS::Undefined();
}


KJS::Value KstBindCollection::clear(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
  exec->setException(eobj);
  return KJS::Undefined();
}


KJS::Value KstBindCollection::readOnly(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Boolean(_readOnly);
}


KJS::Value KstBindCollection::length(KJS::ExecState *exec) const {
  KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
  exec->setException(eobj);
  return KJS::Undefined();
}


QStringList KstBindCollection::collection(KJS::ExecState *exec) const {
  KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
  exec->setException(eobj);
  return QStringList();
}


KJS::Value KstBindCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  Q_UNUSED(item)
  KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
  exec->setException(eobj);
  return KJS::Undefined();
}


KJS::Value KstBindCollection::extract(KJS::ExecState *exec, unsigned item) const {
  Q_UNUSED(item)
  KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
  exec->setException(eobj);
  return KJS::Undefined();
}


// vim: ts=2 sw=2 et
