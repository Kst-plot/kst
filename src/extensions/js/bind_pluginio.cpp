/***************************************************************************
                              bind_pluginio.cpp
                             -------------------
    begin                : Apr 18 2005
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

#include "bind_pluginio.h"

#include <kdebug.h>

KstBindPluginIO::KstBindPluginIO(KJS::ExecState *exec, Plugin::Data::IOValue d, bool input)
: KstBinding(input ? "PluginInput" : "PluginOutput"), _d(d), _input(input) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindPluginIO::KstBindPluginIO(int id)
: KstBinding("PluginIO Method", id) {
}


KstBindPluginIO::~KstBindPluginIO() {
}


struct PluginIOBindings {
  const char *name;
  KJS::Value (KstBindPluginIO::*method)(KJS::ExecState*, const KJS::List&);
};


struct PluginIOProperties {
  const char *name;
  void (KstBindPluginIO::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindPluginIO::*get)(KJS::ExecState*) const;
};


static PluginIOBindings pluginIOBindings[] = {
  { 0L, 0L }
};


static PluginIOProperties pluginIOProperties[] = {
  { "name", 0L, &KstBindPluginIO::name }, 
  { "type", 0L, &KstBindPluginIO::type }, 
  { "subType", 0L, &KstBindPluginIO::subType }, 
  { "description", 0L, &KstBindPluginIO::description }, 
  { "defaultValue", 0L, &KstBindPluginIO::defaultValue }, 
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindPluginIO::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; pluginIOProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(pluginIOProperties[i].name)));
  }

  return rc;
}


bool KstBindPluginIO::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; pluginIOProperties[i].name; ++i) {
    if (prop == pluginIOProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


void KstBindPluginIO::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  QString prop = propertyName.qstring();
  for (int i = 0; pluginIOProperties[i].name; ++i) {
    if (prop == pluginIOProperties[i].name) {
      if (!pluginIOProperties[i].set) {
        break;
      }
      (this->*pluginIOProperties[i].set)(exec, value);
      return;
    }
  }

  KstBinding::put(exec, propertyName, value, attr);
}


KJS::Value KstBindPluginIO::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; pluginIOProperties[i].name; ++i) {
    if (prop == pluginIOProperties[i].name) {
      if (!pluginIOProperties[i].get) {
        break;
      }
      return (this->*pluginIOProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindPluginIO::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindPluginIO *imp = dynamic_cast<KstBindPluginIO*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*pluginIOBindings[id - 1].method)(exec, args);
}


void KstBindPluginIO::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; pluginIOBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindPluginIO(i + 1));
    obj.put(exec, pluginIOBindings[i].name, o, KJS::Function);
  }
}


KJS::Value KstBindPluginIO::name(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::String(_d._name);
}


KJS::Value KstBindPluginIO::type(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  switch (_d._type) {
    case Plugin::Data::IOValue::TableType:
      return KJS::String("Table");
    case Plugin::Data::IOValue::StringType:
      return KJS::String("String");
    case Plugin::Data::IOValue::MapType:
      return KJS::String("Map");
    case Plugin::Data::IOValue::IntegerType:
      return KJS::String("Integer");
    case Plugin::Data::IOValue::FloatType:
      return KJS::String("Float");
    case Plugin::Data::IOValue::PidType:
      return KJS::String("PID");
    case Plugin::Data::IOValue::UnknownType:
    default:
      return KJS::String("Unknown");
  }
}


KJS::Value KstBindPluginIO::subType(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  switch (_d._subType) {
    case Plugin::Data::IOValue::FloatNonVectorSubType:
      return KJS::String("FloatNonVector");
    case Plugin::Data::IOValue::StringSubType:
      return KJS::String("String");
    case Plugin::Data::IOValue::IntegerSubType:
      return KJS::String("Integer");
    case Plugin::Data::IOValue::FloatSubType:
      return KJS::String("Float");
    case Plugin::Data::IOValue::AnySubType:
      return KJS::String("Any");
    case Plugin::Data::IOValue::UnknownType:
    default:
      return KJS::String("Unknown");
  }
}


KJS::Value KstBindPluginIO::description(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::String(_d._description);
}


KJS::Value KstBindPluginIO::defaultValue(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::String(_d._default);
}



// vim: ts=2 sw=2 et
