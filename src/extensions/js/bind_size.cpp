/***************************************************************************
                               bind_size.cpp
                             -------------------
    begin                : May 28 2005
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

#include "bind_size.h"

KstBindSize::KstBindSize(KJS::ExecState *exec, int w, int h)
: KstBinding("Size"), _sz(w, h) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindSize::KstBindSize(KJS::ExecState *exec, const QSize& sz)
: KstBinding("Size"), _sz(sz) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindSize::KstBindSize(KJS::ExecState *exec, KJS::Object *globalObject)
: KstBinding("Size") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    globalObject->put(exec, "Size", o);
  }
}


KstBindSize::KstBindSize(int id)
: KstBinding("Size Method", id) {
}


KstBindSize::~KstBindSize() {
}


KJS::Object KstBindSize::construct(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() == 0) {
    return KJS::Object(new KstBindSize(exec, 0, 0));
  }

  if (args.size() != 2) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Object();
  }

  if (args[0].type() != KJS::NumberType || args[1].type() != KJS::NumberType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Object();
  }

  return KJS::Object(new KstBindSize(exec, args[0].toUInt32(exec), args[1].toUInt32(exec)));
}


struct SizeBindings {
  const char *name;
  KJS::Value (KstBindSize::*method)(KJS::ExecState*, const KJS::List&);
};


struct SizeProperties {
  const char *name;
  void (KstBindSize::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindSize::*get)(KJS::ExecState*) const;
};


static SizeBindings sizeBindings[] = {
  { 0L, 0L }
};


static SizeProperties sizeProperties[] = {
  { "w", &KstBindSize::setW, &KstBindSize::w },
  { "h", &KstBindSize::setH, &KstBindSize::h },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindSize::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; sizeProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(sizeProperties[i].name)));
  }

  return rc;
}


bool KstBindSize::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; sizeProperties[i].name; ++i) {
    if (prop == sizeProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


void KstBindSize::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  QString prop = propertyName.qstring();
  for (int i = 0; sizeProperties[i].name; ++i) {
    if (prop == sizeProperties[i].name) {
      if (!sizeProperties[i].set) {
        break;
      }
      (this->*sizeProperties[i].set)(exec, value);
      return;
    }
  }

  KstBinding::put(exec, propertyName, value, attr);
}


KJS::Value KstBindSize::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; sizeProperties[i].name; ++i) {
    if (prop == sizeProperties[i].name) {
      if (!sizeProperties[i].get) {
        break;
      }
      return (this->*sizeProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindSize::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindSize *imp = dynamic_cast<KstBindSize*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*sizeBindings[id - 1].method)(exec, args);
}


void KstBindSize::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; sizeBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindSize(i + 1));
    obj.put(exec, sizeBindings[i].name, o, KJS::Function);
  }
}


void KstBindSize::setW(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned int w = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(w)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  _sz.setWidth(w);
}


KJS::Value KstBindSize::w(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Number(_sz.width());
}


void KstBindSize::setH(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned int h = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(h)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  _sz.setHeight(h);
}


KJS::Value KstBindSize::h(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Number(_sz.height());
}


KJS::UString KstBindSize::toString(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::UString(QString("(%1, %2)").arg(_sz.width()).arg(_sz.height()));
}

// vim: ts=2 sw=2 et
