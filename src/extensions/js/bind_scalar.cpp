/***************************************************************************
                               bind_scalar.cpp
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

#include "bind_scalar.h"

#include <kstdatacollection.h>

#include <kdebug.h>

KstBindScalar::KstBindScalar(KJS::ExecState *exec, KstScalarPtr s)
: KstBindObject(exec, s.data(), "Scalar") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindScalar::KstBindScalar(KJS::ExecState *exec, KJS::Object *globalObject)
: KstBindObject(exec, globalObject, "Scalar") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (!globalObject) {
    _d = new KstScalar;
  }
}


KstBindScalar::KstBindScalar(int id)
: KstBindObject(id, "Scalar Method") {
}


KstBindScalar::~KstBindScalar() {
}


KJS::Object KstBindScalar::construct(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  if (args.size() == 0) {
    return KJS::Object(new KstBindScalar(exec));
  }

  if (args.size() > 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Object();
  }

  if (args[0].type() != KJS::NumberType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Object();
  }

  KstScalarPtr s = new KstScalar;
  s->setValue(args[0].toNumber(exec));
  return KJS::Object(new KstBindScalar(exec, s));
}


struct ScalarBindings {
  const char *name;
  KJS::Value (KstBindScalar::*method)(KJS::ExecState*, const KJS::List&);
};


struct ScalarProperties {
  const char *name;
  void (KstBindScalar::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindScalar::*get)(KJS::ExecState*) const;
};


static ScalarBindings scalarBindings[] = {
  { 0L, 0L }
};


static ScalarProperties scalarProperties[] = {
  { "value", &KstBindScalar::setValue, &KstBindScalar::value },
  { 0L, 0L, 0L }
};


int KstBindScalar::methodCount() const {
  return sizeof scalarBindings + KstBindObject::methodCount();
}


int KstBindScalar::propertyCount() const {
  return sizeof scalarProperties + KstBindObject::propertyCount();
}


KJS::ReferenceList KstBindScalar::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindObject::propList(exec, recursive);

  for (int i = 0; scalarProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(scalarProperties[i].name)));
  }

  return rc;
}


bool KstBindScalar::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; scalarProperties[i].name; ++i) {
    if (prop == scalarProperties[i].name) {
      return true;
    }
  }

  return KstBindObject::hasProperty(exec, propertyName);
}


void KstBindScalar::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; scalarProperties[i].name; ++i) {
    if (prop == scalarProperties[i].name) {
      if (!scalarProperties[i].set) {
        break;
      }
      (this->*scalarProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindScalar::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; scalarProperties[i].name; ++i) {
    if (prop == scalarProperties[i].name) {
      if (!scalarProperties[i].get) {
        break;
      }
      return (this->*scalarProperties[i].get)(exec);
    }
  }
  
  return KstBindObject::get(exec, propertyName);
}


KJS::Value KstBindScalar::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindObject::methodCount();
  if (id > start) {
    KstBindScalar *imp = dynamic_cast<KstBindScalar*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*scalarBindings[id - start - 1].method)(exec, args);
  }

  return KstBindObject::call(exec, self, args);
}


void KstBindScalar::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindObject::methodCount();
  for (int i = 0; scalarBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindScalar(i + start + 1));
    obj.put(exec, scalarBindings[i].name, o, KJS::Function);
  }
}


#define makeScalar(X) dynamic_cast<KstScalar*>(const_cast<KstObject*>(X.data()))


void KstBindScalar::setValue(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::NumberType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstScalarPtr s = makeScalar(_d);
  if (s) {
    KstWriteLocker wl(s);
    s->setValue(value.toNumber(exec));
  }
}


KJS::Value KstBindScalar::value(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstScalarPtr s = makeScalar(_d);
  if (s) {
    KstReadLocker rl(s);
    return KJS::Number(s->value());
  }
  return KJS::Number(0);
}


#undef makeScalar

// vim: ts=2 sw=2 et
