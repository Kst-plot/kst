/***************************************************************************
                             bind_dataobject.cpp
                             -------------------
    begin                : Apr 10 2005
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

#include "bind_dataobject.h"

#include <kstdatacollection.h>

#include <kdebug.h>

KstBindDataObject::KstBindDataObject(KJS::ExecState *exec, KstDataObjectPtr d, const char *name)
: KstBindObject(exec, d.data(), name ? name : "DataObject") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindDataObject::KstBindDataObject(KJS::ExecState *exec, KJS::Object *globalObject, const char *name)
: KstBindObject(exec, globalObject, name ? name : "DataObject") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindDataObject::KstBindDataObject(int id, const char *name)
: KstBindObject(id, name ? name : "DataObject Method") {
}


KstBindDataObject::~KstBindDataObject() {
}


struct DataObjectBindings {
  const char *name;
  KJS::Value (KstBindDataObject::*method)(KJS::ExecState*, const KJS::List&);
};


struct DataObjectProperties {
  const char *name;
  void (KstBindDataObject::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindDataObject::*get)(KJS::ExecState*) const;
};


static DataObjectBindings dataObjectBindings[] = {
  { "convertTo", &KstBindDataObject::convertTo },
  { 0L, 0L }
};


static DataObjectProperties dataObjectProperties[] = {
  { "type", 0L, &KstBindDataObject::type },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindDataObject::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindObject::propList(exec, recursive);

  for (int i = 0; dataObjectProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(dataObjectProperties[i].name)));
  }

  return rc;
}


bool KstBindDataObject::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; dataObjectProperties[i].name; ++i) {
    if (prop == dataObjectProperties[i].name) {
      return true;
    }
  }

  return KstBindObject::hasProperty(exec, propertyName);
}


void KstBindDataObject::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; dataObjectProperties[i].name; ++i) {
    if (prop == dataObjectProperties[i].name) {
      if (!dataObjectProperties[i].set) {
        break;
      }
      (this->*dataObjectProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindDataObject::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; dataObjectProperties[i].name; ++i) {
    if (prop == dataObjectProperties[i].name) {
      if (!dataObjectProperties[i].get) {
        break;
      }
      return (this->*dataObjectProperties[i].get)(exec);
    }
  }
  
  return KstBindObject::get(exec, propertyName);
}


KJS::Value KstBindDataObject::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindObject::methodCount();
  if (id > start) {
    KstBindDataObject *imp = dynamic_cast<KstBindDataObject*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*dataObjectBindings[id - start - 1].method)(exec, args);
  }

  return KstBindObject::call(exec, self, args);
}


void KstBindDataObject::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindObject::methodCount();
  for (int i = 0; dataObjectBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindDataObject(i + start + 1));
    obj.put(exec, dataObjectBindings[i].name, o, KJS::Function);
  }
}


#define makeDataObject(X) dynamic_cast<KstDataObject*>(const_cast<KstObject*>(X.data()))


KJS::Value KstBindDataObject::convertTo(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly one argument.");
    exec->setException(eobj);
    return KJS::Null();
  }

  if (args[0].type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Null();
  }

  QString type = args[0].toString(exec).qstring();
  if (type == "DataObject") {
    return KJS::Object(new KstBindDataObject(exec, kst_cast<KstDataObject>(_d)));
  }
  
  if (_factoryMap.contains(type)) {
    KstBindDataObject *o = (_factoryMap[type])(exec, kst_cast<KstDataObject>(_d));
    if (o) {
      return KJS::Object(o);
    }
  }

  return KJS::Null();
}


KJS::Value KstBindDataObject::type(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstDataObjectPtr d = makeDataObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::String(d->type());
  }
  return KJS::String();
}


KstBindDataObject *KstBindDataObject::bind(KJS::ExecState *exec, KstDataObjectPtr obj) {
  if (!obj) {
    return 0L;
  }
  
  if (_factoryMap.contains(obj->type())) {
    KstBindDataObject *o = (_factoryMap[obj->type()])(exec, obj);
    if (o) {
      return o;
    }
  }
  return new KstBindDataObject(exec, obj);
}


void KstBindDataObject::addFactory(const QString& typeName, KstBindDataObject*(*factory)(KJS::ExecState*, KstDataObjectPtr)) {
  _factoryMap[typeName] = factory;
}


QMap<QString, KstBindDataObject*(*)(KJS::ExecState*, KstDataObjectPtr)> KstBindDataObject::_factoryMap;

#undef makeDataObject

// vim: ts=2 sw=2 et
