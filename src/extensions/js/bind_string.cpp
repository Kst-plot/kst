/***************************************************************************
                               bind_string.cpp
                             -------------------
    begin                : Mar 28 2005
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

#include "bind_string.h"

#include <kstdatacollection.h>

#include <kdebug.h>

KstBindString::KstBindString(KJS::ExecState *exec, KstStringPtr s)
: KstBindObject(exec, s.data(), "String") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindString::KstBindString(KJS::ExecState *exec, KJS::Object *globalObject)
: KstBindObject(exec, globalObject, "String") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (!globalObject) {
    _d = new KstString;
  }
}


KstBindString::KstBindString(int id)
: KstBindObject(id, "String Method") {
}


KstBindString::~KstBindString() {
}


KJS::Object KstBindString::construct(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  return KJS::Object(new KstBindString(exec));
}


struct StringBindings {
  const char *name;
  KJS::Value (KstBindString::*method)(KJS::ExecState*, const KJS::List&);
};


struct StringProperties {
  const char *name;
  void (KstBindString::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindString::*get)(KJS::ExecState*) const;
};


static StringBindings stringBindings[] = {
  { 0L, 0L }
};


static StringProperties stringProperties[] = {
  { "value", &KstBindString::setValue, &KstBindString::value },
  { 0L, 0L, 0L }
};


int KstBindString::methodCount() const {
    return sizeof stringBindings + KstBindObject::methodCount();
}


int KstBindString::propertyCount() const {
    return sizeof stringProperties + KstBindObject::propertyCount();
}


KJS::ReferenceList KstBindString::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindObject::propList(exec, recursive);

  for (int i = 0; stringProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(stringProperties[i].name)));
  }

  return rc;
}


bool KstBindString::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; stringProperties[i].name; ++i) {
    if (prop == stringProperties[i].name) {
      return true;
    }
  }

  return KstBindObject::hasProperty(exec, propertyName);
}


void KstBindString::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; stringProperties[i].name; ++i) {
    if (prop == stringProperties[i].name) {
      if (!stringProperties[i].set) {
        break;
      }
      (this->*stringProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindString::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; stringProperties[i].name; ++i) {
    if (prop == stringProperties[i].name) {
      if (!stringProperties[i].get) {
        break;
      }
      return (this->*stringProperties[i].get)(exec);
    }
  }
  
  return KstBindObject::get(exec, propertyName);
}


KJS::Value KstBindString::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindObject::methodCount();
  if (id > start) {
    KstBindString *imp = dynamic_cast<KstBindString*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*stringBindings[id - start - 1].method)(exec, args);
  }

  return KstBindObject::call(exec, self, args);
}


void KstBindString::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindObject::methodCount();
  for (int i = 0; stringBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindString(i + start + 1));
    obj.put(exec, stringBindings[i].name, o, KJS::Function);
  }
}


#define makeString(X) dynamic_cast<KstString*>(const_cast<KstObject*>(X.data()))


void KstBindString::setValue(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::NumberType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstStringPtr s = makeString(_d);
  if (s) {
    KstWriteLocker wl(s);
    s->setValue(value.toString(exec).qstring());
  }
}


KJS::Value KstBindString::value(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstStringPtr s = makeString(_d);
  if (s) {
    KstReadLocker rl(s);
    return KJS::String(s->value());
  }
  return KJS::String();
}


#undef makeString

// vim: ts=2 sw=2 et
