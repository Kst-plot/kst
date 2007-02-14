/***************************************************************************
                            bind_debuglogentry.cpp
                             -------------------
    begin                : Apr 07 2005
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

#include "bind_debuglogentry.h"

#include <kdebug.h>

KstBindDebugLogEntry::KstBindDebugLogEntry(KJS::ExecState *exec, KstDebug::LogMessage d)
: KstBinding("DebugLogEntry", false), _d(d) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindDebugLogEntry::KstBindDebugLogEntry(int id)
: KstBinding("DebugLogEntry Method", id) {
}


KstBindDebugLogEntry::~KstBindDebugLogEntry() {
}


struct DebugLogEntryBindings {
  const char *name;
  KJS::Value (KstBindDebugLogEntry::*method)(KJS::ExecState*, const KJS::List&);
};


struct DebugLogEntryProperties {
  const char *name;
  void (KstBindDebugLogEntry::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindDebugLogEntry::*get)(KJS::ExecState*) const;
};


static DebugLogEntryBindings debugLogEntryBindings[] = {
  { 0L, 0L }
};


static DebugLogEntryProperties debugLogEntryProperties[] = {
  { "text", 0L, &KstBindDebugLogEntry::text },
  { "date", 0L, &KstBindDebugLogEntry::date },
  { "level", 0L, &KstBindDebugLogEntry::level },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindDebugLogEntry::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; debugLogEntryProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(debugLogEntryProperties[i].name)));
  }

  return rc;
}


bool KstBindDebugLogEntry::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; debugLogEntryProperties[i].name; ++i) {
    if (prop == debugLogEntryProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


KJS::Value KstBindDebugLogEntry::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; debugLogEntryProperties[i].name; ++i) {
    if (prop == debugLogEntryProperties[i].name) {
      if (!debugLogEntryProperties[i].get) {
        break;
      }
      return (this->*debugLogEntryProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindDebugLogEntry::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindDebugLogEntry *imp = dynamic_cast<KstBindDebugLogEntry*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*debugLogEntryBindings[id - 1].method)(exec, args);
}


void KstBindDebugLogEntry::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; debugLogEntryBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindDebugLogEntry(i + 1));
    obj.put(exec, debugLogEntryBindings[i].name, o, KJS::Function);
  }
}


KJS::Value KstBindDebugLogEntry::text(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::String(_d.msg);
}


KJS::Value KstBindDebugLogEntry::date(KJS::ExecState *exec) const {
  KJS::List c;
  c.append(KJS::Number(_d.date.date().year()));
  c.append(KJS::Number(_d.date.date().month() - 1));
  c.append(KJS::Number(_d.date.date().day()));
  c.append(KJS::Number(_d.date.time().hour()));
  c.append(KJS::Number(_d.date.time().minute()));
  c.append(KJS::Number(_d.date.time().second()));
  c.append(KJS::Number(_d.date.time().msec()));
  return KJS::Object(exec->interpreter()->builtinDate().construct(exec, c));
}


KJS::Value KstBindDebugLogEntry::level(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  // Don't i18n so that it's parseable
  QString lev;
  switch (_d.level) {
    case KstDebug::Notice:
      lev = "N";
      break;
    case KstDebug::Warning:
      lev = "W";
      break;
    case KstDebug::Error:
      lev = "E";
      break;
    case KstDebug::Debug:
      lev = "D";
      break;
    default:
      lev = " ";
  }
  return KJS::String(lev);
}

// vim: ts=2 sw=2 et
