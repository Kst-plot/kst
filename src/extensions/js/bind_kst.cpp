/***************************************************************************
                                bind_kst.cpp
                             -------------------
    begin                : Mar 28 2005
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

#define private public
#define protected public
#include <kjsembed/kjsembedpart.h>
#undef protected
#undef private

#include "bind_kst.h"
#include "bind_colorsequence.h"
#include "bind_dataobjectcollection.h"
#include "bind_datasource.h"
#include "bind_datasourcecollection.h"
#include "bind_document.h"
#include "bind_extensioncollection.h"
#include "bind_pluginmanager.h"
#include "bind_scalar.h"
#include "bind_scalarcollection.h"
#include "bind_string.h"
#include "bind_stringcollection.h"
#include "bind_vector.h"
#include "bind_vectorcollection.h"
#include "bind_window.h"
#include "bind_windowcollection.h"
#include "js.h"

#include <kst.h>
#include <kstdatacollection.h>
#include <kstdebug.h>
#include <kstdoc.h>
#include <kstviewwindow.h>

#include <kdebug.h>

#include <qfile.h>

using namespace KJSEmbed;

KstBindKst::KstBindKst(KJS::ExecState *exec, KJS::Object *globalObject, KstJS *ext)
: KstBinding("Kst", false), _ext(ext) {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    globalObject->put(exec, "Kst", o);
  }
}


KstBindKst::KstBindKst(int id)
: KstBinding("Kst Member", id), _ext(0L) {
}


KstBindKst::~KstBindKst() {
}


struct KstBindings {
  const char *name;
  KJS::Value (KstBindKst::*method)(KJS::ExecState*, const KJS::List&);
};


struct KstProperties {
  const char *name;
  void (KstBindKst::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindKst::*get)(KJS::ExecState*) const;
};


static KstBindings kstBindings[] = {
  { "resetInterpreter", &KstBindKst::resetInterpreter },
  { "loadScript", &KstBindKst::loadScript },
  { "purge", &KstBindKst::purge },
  { 0L, 0L }
};


static KstProperties kstProperties[] = {
  { "dataSources", 0L, &KstBindKst::dataSources },
  { "scalars", 0L, &KstBindKst::scalars },
  { "strings", 0L, &KstBindKst::strings },
  { "vectors", 0L, &KstBindKst::vectors },
  { "windows", 0L, &KstBindKst::windows },
  { "objects", 0L, &KstBindKst::objects },
  { "colors", 0L, &KstBindKst::colors },
  { "extensions", 0L, &KstBindKst::extensions },
  { "document", 0L, &KstBindKst::document },
  { "pluginManager", 0L, &KstBindKst::pluginManager },
  { "gui", 0L, &KstBindKst::gui },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindKst::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; kstProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(kstProperties[i].name)));
  }

  rc.append(KJS::Reference(this, KJS::Identifier("version")));
  rc.append(KJS::Reference(this, KJS::Identifier("scriptVersion")));

  return rc;
}


bool KstBindKst::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; kstProperties[i].name; ++i) {
    if (prop == kstProperties[i].name) {
      return true;
    }
  }

  if (prop == "version" || prop == "scriptVersion") {
    return true;
  }

  return KstBinding::hasProperty(exec, propertyName);
}


KJS::Value KstBindKst::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (propertyName.qstring() == "version") {
    return KJS::String(KSTVERSION);
  }

  if (propertyName.qstring() == "scriptVersion") {
    return KJS::Number(1);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; kstProperties[i].name; ++i) {
    if (prop == kstProperties[i].name) {
      if (!kstProperties[i].get) {
        break;
      }
      return (this->*kstProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindKst::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindKst *imp = dynamic_cast<KstBindKst*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*kstBindings[id - 1].method)(exec, args);
}


void KstBindKst::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; kstBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindKst(i + 1));
    obj.put(exec, kstBindings[i].name, o, KJS::Function);
  }
}


KJS::Value KstBindKst::loadScript(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args[0].type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  QString fn = args[0].toString(exec).qstring();
  if (!QFile::exists(fn)) { // One day make this support KIO FIXME
    return KJS::Boolean(false);
  }

  if (_ext->part()->runFile(fn)) {
    // FIXME: add to the script registry
  } else {
    KJS::Completion c = _ext->part()->completion();
    if (!c.isNull()) {
      QString err = c.toString(_ext->part()->globalExec()).qstring();
      KstDebug::self()->log(i18n("Error running script %1: %2").arg(fn).arg(err), KstDebug::Error);
    } else {
      KstDebug::self()->log(i18n("Unknown error running script %1.").arg(fn), KstDebug::Error);
    }
    return KJS::Boolean(false);
  }

  return KJS::Boolean(true);
}


KJS::Value KstBindKst::resetInterpreter(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (_ext) {
    QTimer::singleShot(0, _ext, SLOT(resetInterpreter()));
    return KJS::Boolean(true);
  }
  return KJS::Boolean(false);
}


KJS::Value KstBindKst::purge(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstApp::inst()->document()->purge();
  return KJS::Undefined();
}


KJS::Value KstBindKst::vectors(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindVectorCollection(exec));
}


KJS::Value KstBindKst::scalars(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindScalarCollection(exec));
}


KJS::Value KstBindKst::strings(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindStringCollection(exec));
}


KJS::Value KstBindKst::windows(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindWindowCollection(exec));
}


KJS::Value KstBindKst::dataSources(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindDataSourceCollection(exec));
}


KJS::Value KstBindKst::objects(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindDataObjectCollection(exec));
}


KJS::Value KstBindKst::colors(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindColorSequence(exec));
}


KJS::Value KstBindKst::extensions(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindExtensionCollection(exec));
}


KJS::Value KstBindKst::document(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindDocument(exec));
}


KJS::Value KstBindKst::pluginManager(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindPluginManager(exec));
}


KJS::Value KstBindKst::gui(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return _ext->part()->bind(_ext->app());
}


// vim: ts=2 sw=2 et
