/***************************************************************************
                               bind_debug.cpp
                             -------------------
    begin                : Apr 04 2005
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

#include "bind_debug.h"
#include "bind_debuglog.h"

#include <kstdebug.h>

#include <kdebug.h>

KstBindDebug::KstBindDebug(KJS::ExecState *exec, KJS::Object *globalObject)
: KstBinding("Debug", false) {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    globalObject->put(exec, "Debug", o);
  }
}


KstBindDebug::KstBindDebug(int id)
: KstBinding("Debug Member", id) {
}


KstBindDebug::~KstBindDebug() {
}


struct DebugBindings {
  const char *name;
  KJS::Value (KstBindDebug::*method)(KJS::ExecState*, const KJS::List&);
};


struct DebugProperties {
  const char *name;
  void (KstBindDebug::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindDebug::*get)(KJS::ExecState*) const;
};


static DebugBindings debugBindings[] = {
  { "clear", &KstBindDebug::clear },
  { "warning", &KstBindDebug::warning },
  { "error", &KstBindDebug::error },
  { "notice", &KstBindDebug::notice },
  { "debug", &KstBindDebug::debug },
  { 0L, 0L }
};


static DebugProperties debugProperties[] = {
  { "log", 0L, &KstBindDebug::log },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindDebug::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; debugProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(debugProperties[i].name)));
  }

  return rc;
}


bool KstBindDebug::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; debugProperties[i].name; ++i) {
    if (prop == debugProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


KJS::Value KstBindDebug::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; debugProperties[i].name; ++i) {
    if (prop == debugProperties[i].name) {
      if (!debugProperties[i].get) {
        break;
      }
      return (this->*debugProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindDebug::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindDebug *imp = dynamic_cast<KstBindDebug*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*debugBindings[id - 1].method)(exec, args);
}


void KstBindDebug::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; debugBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindDebug(i + 1));
    obj.put(exec, debugBindings[i].name, o, KJS::Function);
  }
}


KJS::Value KstBindDebug::warning(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args[0].type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstDebug::self()->log(args[0].toString(exec).qstring(), KstDebug::Warning);
  return KJS::Undefined();
}


KJS::Value KstBindDebug::error(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args[0].type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstDebug::self()->log(args[0].toString(exec).qstring(), KstDebug::Error);
  return KJS::Undefined();
}


KJS::Value KstBindDebug::notice(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args[0].type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstDebug::self()->log(args[0].toString(exec).qstring(), KstDebug::Notice);
  return KJS::Undefined();
}


KJS::Value KstBindDebug::debug(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args[0].type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstDebug::self()->log(args[0].toString(exec).qstring(), KstDebug::Debug);
  return KJS::Undefined();
}


KJS::Value KstBindDebug::clear(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstDebug::self()->clear();
  return KJS::Undefined();
}


KJS::Value KstBindDebug::log(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindDebugLog(exec));
}


// vim: ts=2 sw=2 et
