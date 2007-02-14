/***************************************************************************
                            bind_pluginmodule.cpp
                             -------------------
    begin                : Apr 12 2005
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

#include "bind_pluginmodule.h"
#include "bind_pluginiocollection.h"

#include <kdebug.h>

KstBindPluginModule::KstBindPluginModule(KJS::ExecState *exec, const Plugin::Data& d)
: KstBinding("PluginModule", false), _d(d) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindPluginModule::KstBindPluginModule(int id)
: KstBinding("PluginModule Method", id) {
}


KstBindPluginModule::~KstBindPluginModule() {
}


struct PluginModuleBindings {
  const char *name;
  KJS::Value (KstBindPluginModule::*method)(KJS::ExecState*, const KJS::List&);
};


struct PluginModuleProperties {
  const char *name;
  void (KstBindPluginModule::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindPluginModule::*get)(KJS::ExecState*) const;
};


static PluginModuleBindings pluginModuleBindings[] = {
  { 0L, 0L }
};


static PluginModuleProperties pluginModuleProperties[] = {
  { "usesLocalData", 0L, &KstBindPluginModule::usesLocalData }, 
  { "isFit", 0L, &KstBindPluginModule::isFit }, 
  { "isFilter", 0L, &KstBindPluginModule::isFilter }, 
  { "name", 0L, &KstBindPluginModule::name }, 
  { "readableName", 0L, &KstBindPluginModule::readableName }, 
  { "author", 0L, &KstBindPluginModule::author }, 
  { "description", 0L, &KstBindPluginModule::description }, 
  { "version", 0L, &KstBindPluginModule::version }, 
  { "inputs", 0L, &KstBindPluginModule::inputs },
  { "outputs", 0L, &KstBindPluginModule::outputs }, 
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindPluginModule::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; pluginModuleProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(pluginModuleProperties[i].name)));
  }

  return rc;
}


bool KstBindPluginModule::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; pluginModuleProperties[i].name; ++i) {
    if (prop == pluginModuleProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


void KstBindPluginModule::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  QString prop = propertyName.qstring();
  for (int i = 0; pluginModuleProperties[i].name; ++i) {
    if (prop == pluginModuleProperties[i].name) {
      if (!pluginModuleProperties[i].set) {
        break;
      }
      (this->*pluginModuleProperties[i].set)(exec, value);
      return;
    }
  }

  KstBinding::put(exec, propertyName, value, attr);
}


KJS::Value KstBindPluginModule::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; pluginModuleProperties[i].name; ++i) {
    if (prop == pluginModuleProperties[i].name) {
      if (!pluginModuleProperties[i].get) {
        break;
      }
      return (this->*pluginModuleProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindPluginModule::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindPluginModule *imp = dynamic_cast<KstBindPluginModule*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*pluginModuleBindings[id - 1].method)(exec, args);
}


void KstBindPluginModule::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; pluginModuleBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindPluginModule(i + 1));
    obj.put(exec, pluginModuleBindings[i].name, o, KJS::Function);
  }
}


KJS::Value KstBindPluginModule::usesLocalData(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Boolean(_d._localdata);
}


KJS::Value KstBindPluginModule::isFit(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Boolean(_d._isFit);
}


KJS::Value KstBindPluginModule::isFilter(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Boolean(_d._isFilter);
}


KJS::Value KstBindPluginModule::name(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::String(_d._name);
}


KJS::Value KstBindPluginModule::readableName(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::String(_d._readableName);
}


KJS::Value KstBindPluginModule::author(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::String(_d._author);
}


KJS::Value KstBindPluginModule::description(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::String(_d._description);
}


KJS::Value KstBindPluginModule::version(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::String(_d._version);
}


KJS::Value KstBindPluginModule::inputs(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindPluginIOCollection(exec, _d._inputs, true));
}


KJS::Value KstBindPluginModule::outputs(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindPluginIOCollection(exec, _d._outputs, false));
}


// vim: ts=2 sw=2 et
