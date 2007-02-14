/***************************************************************************
                            bind_pluginmanager.cpp
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

#include "bind_pluginmanager.h"
#include "bind_pluginmodulecollection.h"

#include <pluginmanager.h>

#include <kdebug.h>

KstBindPluginManager::KstBindPluginManager(KJS::ExecState *exec)
: KstBinding("PluginManager", false) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindPluginManager::KstBindPluginManager(int id)
: KstBinding("PluginManager Method", id) {
}


KstBindPluginManager::~KstBindPluginManager() {
}


KJS::Object KstBindPluginManager::construct(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  return KJS::Object(new KstBindPluginManager(exec));
}


struct PluginManagerBindings {
  const char *name;
  KJS::Value (KstBindPluginManager::*method)(KJS::ExecState*, const KJS::List&);
};


struct PluginManagerProperties {
  const char *name;
  void (KstBindPluginManager::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindPluginManager::*get)(KJS::ExecState*) const;
};


static PluginManagerBindings pluginManagerBindings[] = {
  { 0L, 0L }
};


static PluginManagerProperties pluginManagerProperties[] = {
  { "modules", 0L, &KstBindPluginManager::modules },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindPluginManager::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; pluginManagerProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(pluginManagerProperties[i].name)));
  }

  return rc;
}


bool KstBindPluginManager::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; pluginManagerProperties[i].name; ++i) {
    if (prop == pluginManagerProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


void KstBindPluginManager::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  QString prop = propertyName.qstring();
  for (int i = 0; pluginManagerProperties[i].name; ++i) {
    if (prop == pluginManagerProperties[i].name) {
      if (!pluginManagerProperties[i].set) {
        break;
      }
      (this->*pluginManagerProperties[i].set)(exec, value);
      return;
    }
  }

  KstBinding::put(exec, propertyName, value, attr);
}


KJS::Value KstBindPluginManager::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; pluginManagerProperties[i].name; ++i) {
    if (prop == pluginManagerProperties[i].name) {
      if (!pluginManagerProperties[i].get) {
        break;
      }
      return (this->*pluginManagerProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindPluginManager::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindPluginManager *imp = dynamic_cast<KstBindPluginManager*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*pluginManagerBindings[id - 1].method)(exec, args);
}


void KstBindPluginManager::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; pluginManagerBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindPluginManager(i + 1));
    obj.put(exec, pluginManagerBindings[i].name, o, KJS::Function);
  }
}


KJS::Value KstBindPluginManager::modules(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindPluginModuleCollection(exec));
}


// vim: ts=2 sw=2 et
