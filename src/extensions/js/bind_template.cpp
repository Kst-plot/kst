/***************************************************************************
                              bind_template.cpp
                             -------------------
    begin                : Mar 29 2005
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

#include "bind_template.h"

#include <kstdatacollection.h>

#include <kdebug.h>

KstBindTemplate::KstBindTemplate(KJS::ExecState *exec, KstTemplatePtr d)
: KstBinding("Template"), _d(d) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindTemplate::KstBindTemplate(KJS::ExecState *exec, KJS::Object *globalObject)
: KstBinding("Template") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    globalObject->put(exec, "Template", o);
  }
}


KstBindTemplate::KstBindTemplate(int id)
: KstBinding("Template Method", id) {
}


KstBindTemplate::~KstBindTemplate() {
}


KJS::Object KstBindTemplate::construct(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  return KJS::Object(new KstBindTemplate(exec));
}


struct TemplateBindings {
  const char *name;
  KJS::Value (KstBindTemplate::*method)(KJS::ExecState*, const KJS::List&);
};


struct TemplateProperties {
  const char *name;
  void (KstBindTemplate::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindTemplate::*get)(KJS::ExecState*) const;
};


static TemplateBindings templateBindings[] = {
  { "doIt", &KstBindTemplate::doIt },
  { 0L, 0L }
};


static TemplateProperties templateProperties[] = {
  { "value", &KstBindTemplate::setValue, &KstBindTemplate::value },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindTemplate::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; templateProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(templateProperties[i].name)));
  }

  return rc;
}


bool KstBindTemplate::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; templateProperties[i].name; ++i) {
    if (prop == templateProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


void KstBindTemplate::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBinding::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; templateProperties[i].name; ++i) {
    if (prop == templateProperties[i].name) {
      if (!templateProperties[i].set) {
        break;
      }
      (this->*templateProperties[i].set)(exec, value);
      return;
    }
  }

  KstBinding::put(exec, propertyName, value, attr);
}


KJS::Value KstBindTemplate::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBinding::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; templateProperties[i].name; ++i) {
    if (prop == templateProperties[i].name) {
      if (!templateProperties[i].get) {
        break;
      }
      return (this->*templateProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindTemplate::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindTemplate *imp = dynamic_cast<KstBindTemplate*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*templateBindings[id - 1].method)(exec, args);
}


void KstBindTemplate::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; templateBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindTemplate(i + 1));
    obj.put(exec, templateBindings[i].name, o, KJS::Function);
  }
}


void KstBindTemplate::setValue(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::NumberType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  _d->setValue(value.toNumber(exec));
}


KJS::Value KstBindTemplate::value(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstReadLocker rl(_d);
  return KJS::Number(_d->value());
}


KJS::Value KstBindTemplate::doIt(KJS::ExecState *exec, const KJS::List& args) {
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly one argument.");
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args[0].type() != KJS::NumberType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  double i = args[0].toNumber(exec);

  KstWriteLocker wl(_d);
  return KJS::Number(_d->doIt(i));
}


int KstBindTemplate::methodCount() const {
  return sizeof templateBindings + KstBinding::methodCount();
}


int KstBindTemplate::propertyCount() const {
  return sizeof templateProperties + KstBinding::propertyCount();
}


// vim: ts=2 sw=2 et
