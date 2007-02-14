/***************************************************************************
                            bind_jsdataobject.cpp
                             -------------------
    begin                : Apr 23 2005
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

#include "bind_jsdataobject.h"

#include <kstdatacollection.h>

#include <kdebug.h>

KstBindJSDataObject::KstBindJSDataObject(KJS::ExecState *exec, JSDataObjectPtr d)
: KstBinding("Script Object"), _d(d) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindJSDataObject::KstBindJSDataObject(KJS::ExecState *exec, KJS::Object *globalObject)
: KstBinding("Script Object") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    globalObject->put(exec, "ScriptObject", o);
  }
}


KstBindJSDataObject::KstBindJSDataObject(int id)
: KstBinding("ScriptObject Method", id) {
}


KstBindJSDataObject::~KstBindJSDataObject() {
}


KJS::Object KstBindJSDataObject::construct(KJS::ExecState *exec, const KJS::List& args) {
  JSDataObjectPtr d;

  // JSDataObject(script)
  if (args.size() == 1) {
    if (args[0].type() != KJS::StringType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Null();
    }

    d = new JSDataObject;
    d->setScript(args[0].toString(exec).qstring());
  }

  // JSDataObject(script, property)
  if (args.size() == 2) {
    if (args[0].type() != KJS::StringType || args[1].type() != KJS::StringType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Null();
    }

    d = new JSDataObject;
    d->setScript(args[0].toString(exec).qstring());
    d->setProperty(args[1].toString(exec).qstring());
  }


  if (!d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Null();
  }

  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(d.data());
  KST::dataObjectList.lock().unlock();

  return KJS::Object(new KstBindJSDataObject(exec, d));
}


struct PluginBindings {
  const char *name;
  KJS::Value (KstBindJSDataObject::*method)(KJS::ExecState*, const KJS::List&);
};


struct PluginProperties {
  const char *name;
  void (KstBindJSDataObject::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindJSDataObject::*get)(KJS::ExecState*) const;
};


static PluginBindings scriptObjectBindings[] = {
  { 0L, 0L }
};


static PluginProperties scriptObjectProperties[] = {
  { "tagName", &KstBindJSDataObject::setTagName, &KstBindJSDataObject::tagName },
  { "script", &KstBindJSDataObject::setScript, &KstBindJSDataObject::script },
  { "property", &KstBindJSDataObject::setProperty, &KstBindJSDataObject::property },
  { "valid", 0L, &KstBindJSDataObject::valid },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindJSDataObject::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; scriptObjectProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(scriptObjectProperties[i].name)));
  }

  return rc;
}


bool KstBindJSDataObject::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; scriptObjectProperties[i].name; ++i) {
    if (prop == scriptObjectProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


void KstBindJSDataObject::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBinding::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; scriptObjectProperties[i].name; ++i) {
    if (prop == scriptObjectProperties[i].name) {
      if (!scriptObjectProperties[i].set) {
        break;
      }
      (this->*scriptObjectProperties[i].set)(exec, value);
      return;
    }
  }

  KstBinding::put(exec, propertyName, value, attr);
}


KJS::Value KstBindJSDataObject::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBinding::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; scriptObjectProperties[i].name; ++i) {
    if (prop == scriptObjectProperties[i].name) {
      if (!scriptObjectProperties[i].get) {
        break;
      }
      return (this->*scriptObjectProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindJSDataObject::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindJSDataObject *imp = dynamic_cast<KstBindJSDataObject*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*scriptObjectBindings[id - 1].method)(exec, args);
}


void KstBindJSDataObject::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; scriptObjectBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindJSDataObject(i + 1));
    obj.put(exec, scriptObjectBindings[i].name, o, KJS::Function);
  }
}


void KstBindJSDataObject::setTagName(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  _d->setTagName(value.toString(exec).qstring());
}


KJS::Value KstBindJSDataObject::tagName(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstReadLocker rl(_d);
  return KJS::String(_d->tagName());
}


void KstBindJSDataObject::setScript(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  _d->setScript(value.toString(exec).qstring());
}


KJS::Value KstBindJSDataObject::script(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstReadLocker rl(_d);
  return KJS::String(_d->script());
}


void KstBindJSDataObject::setProperty(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstWriteLocker wl(_d);
  _d->setProperty(value.toString(exec).qstring());
}


KJS::Value KstBindJSDataObject::property(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstReadLocker rl(_d);
  return KJS::String(_d->propertyString());
}


KJS::Value KstBindJSDataObject::valid(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstReadLocker rl(_d);
  return KJS::Boolean(_d->isValid());
}


// vim: ts=2 sw=2 et
