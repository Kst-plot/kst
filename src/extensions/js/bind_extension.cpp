/***************************************************************************
                             bind_extension.cpp
                             -------------------
    begin                : Apr 11 2005
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

#include "bind_extension.h"

#include <kstdatacollection.h>

#include <kservicetype.h>
#include <extensionmgr.h>
#include <kdebug.h>

KstBindExtension::KstBindExtension(KJS::ExecState *exec, const QString& d)
: KstBinding("Extension", false), _d(d) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindExtension::KstBindExtension(int id)
: KstBinding("Extension Method", id) {
}


KstBindExtension::~KstBindExtension() {
}


struct ExtensionBindings {
  const char *name;
  KJS::Value (KstBindExtension::*method)(KJS::ExecState*, const KJS::List&);
};


struct ExtensionProperties {
  const char *name;
  void (KstBindExtension::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindExtension::*get)(KJS::ExecState*) const;
};


static ExtensionBindings extensionBindings[] = {
  { "load", &KstBindExtension::load },
  { "unload", &KstBindExtension::unload },
  { 0L, 0L }
};


static ExtensionProperties extensionProperties[] = {
  { "name", 0L, &KstBindExtension::name },
  { "loaded", 0L, &KstBindExtension::loaded },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindExtension::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; extensionProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(extensionProperties[i].name)));
  }

  return rc;
}


bool KstBindExtension::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; extensionProperties[i].name; ++i) {
    if (prop == extensionProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


void KstBindExtension::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  QString prop = propertyName.qstring();
  for (int i = 0; extensionProperties[i].name; ++i) {
    if (prop == extensionProperties[i].name) {
      if (!extensionProperties[i].set) {
        break;
      }
      (this->*extensionProperties[i].set)(exec, value);
      return;
    }
  }

  KstBinding::put(exec, propertyName, value, attr);
}


KJS::Value KstBindExtension::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; extensionProperties[i].name; ++i) {
    if (prop == extensionProperties[i].name) {
      if (!extensionProperties[i].get) {
        break;
      }
      return (this->*extensionProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindExtension::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindExtension *imp = dynamic_cast<KstBindExtension*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*extensionBindings[id - 1].method)(exec, args);
}


void KstBindExtension::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; extensionBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindExtension(i + 1));
    obj.put(exec, extensionBindings[i].name, o, KJS::Function);
  }
}


KJS::Value KstBindExtension::name(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::String(_d);
}


KJS::Value KstBindExtension::loaded(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Boolean(ExtensionMgr::self()->enabled(_d));
}


KJS::Value KstBindExtension::load(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  ExtensionMgr::self()->setEnabled(_d, true);
  ExtensionMgr::self()->updateExtensions();
  return loaded(exec);
}


KJS::Value KstBindExtension::unload(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  ExtensionMgr::self()->setEnabled(_d, false);
  ExtensionMgr::self()->updateExtensions();
  return KJS::Undefined();
}

// vim: ts=2 sw=2 et
