/***************************************************************************
                              bind_document.cpp
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

#include "bind_document.h"

#include <kst.h>
#include <kstdoc.h>

#include <kdebug.h>

#include <qfile.h>

KstBindDocument::KstBindDocument(KJS::ExecState *exec)
: KstBinding("Document", false) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindDocument::KstBindDocument(int id)
: KstBinding("Document Method", id) {
}


KstBindDocument::~KstBindDocument() {
}


struct DocumentBindings {
  const char *name;
  KJS::Value (KstBindDocument::*method)(KJS::ExecState*, const KJS::List&);
};


struct DocumentProperties {
  const char *name;
  void (KstBindDocument::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindDocument::*get)(KJS::ExecState*) const;
};


static DocumentBindings documentBindings[] = {
  { "clear", &KstBindDocument::newDocument },
  { "load", &KstBindDocument::load },
  { "save", &KstBindDocument::save },
  { 0L, 0L }
};


static DocumentProperties documentProperties[] = {
  { "name", &KstBindDocument::setName, &KstBindDocument::name },
  { "text", 0L, &KstBindDocument::text },
  { "modified", &KstBindDocument::setModified, &KstBindDocument::modified },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindDocument::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; documentProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(documentProperties[i].name)));
  }

  return rc;
}


bool KstBindDocument::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; documentProperties[i].name; ++i) {
    if (prop == documentProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


void KstBindDocument::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  QString prop = propertyName.qstring();
  for (int i = 0; documentProperties[i].name; ++i) {
    if (prop == documentProperties[i].name) {
      if (!documentProperties[i].set) {
        break;
      }
      (this->*documentProperties[i].set)(exec, value);
      return;
    }
  }

  KstBinding::put(exec, propertyName, value, attr);
}


KJS::Value KstBindDocument::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; documentProperties[i].name; ++i) {
    if (prop == documentProperties[i].name) {
      if (!documentProperties[i].get) {
        break;
      }
      return (this->*documentProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindDocument::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindDocument *imp = dynamic_cast<KstBindDocument*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*documentBindings[id - 1].method)(exec, args);
}


void KstBindDocument::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; documentBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindDocument(i + 1));
    obj.put(exec, documentBindings[i].name, o, KJS::Function);
  }
}


void KstBindDocument::setName(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstApp::inst()->document()->setTitle(value.toString(exec).qstring());
}


KJS::Value KstBindDocument::name(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::String(KstApp::inst()->document()->title());
}


KJS::Value KstBindDocument::text(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  QString rc;
  QTextStream ts(&rc, IO_WriteOnly);
  KstApp::inst()->document()->saveDocument(ts);
  return KJS::String(rc);
}


KJS::Value KstBindDocument::modified(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Boolean(KstApp::inst()->document()->isModified());
}


void KstBindDocument::setModified(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstApp::inst()->document()->setModified(value.toBoolean(exec));
}


KJS::Value KstBindDocument::newDocument(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return KJS::Boolean(KstApp::inst()->document()->newDocument());
}


KJS::Value KstBindDocument::load(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Boolean(false);
  }

  if (args[0].type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Boolean(false);
  }

  KURL u;
  QString p = args[0].toString(exec).qstring();
  if (QFile::exists(p)) {
    u.setPath(p);
  } else {
    u = KURL::fromPathOrURL(p);
  }

  return KJS::Boolean(KstApp::inst()->slotFileOpenRecent(u));
}


KJS::Value KstBindDocument::save(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() > 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Boolean(false);
  }

  KstApp *app = KstApp::inst();

  if (args.size() == 1) {
    if (args[0].type() != KJS::StringType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Boolean(false);
    }
    app->slotUpdateStatusMsg(i18n("Saving file..."));
    bool rc = app->document()->saveDocument(args[0].toString(exec).qstring(), false, false);
    app->slotUpdateStatusMsg(i18n("Ready"));
    return KJS::Boolean(rc);
  }

  if (app->document()->title() == "Untitled") {
    return KJS::Boolean(false);
  }

  app->slotUpdateStatusMsg(i18n("Saving file..."));
  bool rc = app->document()->saveDocument(app->document()->absFilePath(), false, false);
  app->slotUpdateStatusMsg(i18n("Ready"));
  return KJS::Boolean(rc);
}


// vim: ts=2 sw=2 et
