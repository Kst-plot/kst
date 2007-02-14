/***************************************************************************
                               bind_object.cpp
                             -------------------
    begin                : May 31 2005
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

#include "bind_object.h"
#include "bind_objectcollection.h"

#include <kdebug.h>

KstBindObject::KstBindObject(KJS::ExecState *exec, KstObjectPtr d, const char *name)
: KstBinding(name ? name : "Object"), _d(d) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindObject::KstBindObject(KJS::ExecState *exec, KJS::Object *globalObject, const char *name)
: KstBinding(name ? name : "Object") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    globalObject->put(exec, name ? name : "Object", o);
  }
}


KstBindObject::KstBindObject(int id, const char *name)
: KstBinding(name ? name : "Object Method", id) {
}


KstBindObject::~KstBindObject() {
}


KJS::Object KstBindObject::construct(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  return KJS::Object(new KstBindObject(exec));
}


struct ObjectBindings {
  const char *name;
  KJS::Value (KstBindObject::*method)(KJS::ExecState*, const KJS::List&);
};


struct ObjectProperties {
  const char *name;
  void (KstBindObject::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindObject::*get)(KJS::ExecState*) const;
};


static ObjectBindings objectBindings[] = {
  { 0L, 0L }
};


static ObjectProperties objectProperties[] = {
  { "tagName", &KstBindObject::setTagName, &KstBindObject::tagName },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindObject::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; objectProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(objectProperties[i].name)));
  }

  return rc;
}


bool KstBindObject::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; objectProperties[i].name; ++i) {
    if (prop == objectProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


void KstBindObject::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBinding::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; objectProperties[i].name; ++i) {
    if (prop == objectProperties[i].name) {
      if (!objectProperties[i].set) {
        break;
      }
      (this->*objectProperties[i].set)(exec, value);
      return;
    }
  }

  KstBinding::put(exec, propertyName, value, attr);
}


KJS::Value KstBindObject::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBinding::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; objectProperties[i].name; ++i) {
    if (prop == objectProperties[i].name) {
      if (!objectProperties[i].get) {
        break;
      }
      return (this->*objectProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindObject::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBinding::methodCount();
  if (id > start) {
    KstBindObject *imp = dynamic_cast<KstBindObject*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*objectBindings[id - start - 1].method)(exec, args);
  }

  return KstBinding::call(exec, self, args);
}


void KstBindObject::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBinding::methodCount();
  for (int i = 0; objectBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindObject(i + start + 1));
    obj.put(exec, objectBindings[i].name, o, KJS::Function);
  }
}


void KstBindObject::setTagName(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  _d->setTagName(KstObjectTag::fromString(value.toString(exec).qstring()));
}


KJS::Value KstBindObject::tagName(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstReadLocker rl(_d);
  return KJS::String(_d->tagName());
}


int KstBindObject::methodCount() const {
  return sizeof objectBindings + KstBinding::methodCount();
}


int KstBindObject::propertyCount() const {
  return sizeof objectProperties + KstBinding::propertyCount();
}


// vim: ts=2 sw=2 et
