/***************************************************************************
                               bind_arrow.cpp
                               ---------------
    begin                : Jun 14 2005
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

#include "bind_arrow.h"

#include <kst.h>
#include <kstviewwindow.h>

#include <kdebug.h>

KstBindArrow::KstBindArrow(KJS::ExecState *exec, KstViewArrowPtr d, const char *name)
: KstBindLine(exec, d.data(), name ? name : "Arrow") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindArrow::KstBindArrow(KJS::ExecState *exec, KJS::Object *globalObject, const char *name)
: KstBindLine(exec, globalObject, name ? name : "Arrow") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    KstBindViewObject::addFactory("Arrow", KstBindArrow::bindFactory);
  }
}


KstBindViewObject *KstBindArrow::bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj) {
  KstViewArrowPtr v = kst_cast<KstViewArrow>(obj);
  if (v) {
    return new KstBindArrow(exec, v);
  }
  return 0L;
}


KstBindArrow::KstBindArrow(int id, const char *name)
: KstBindLine(id, name ? name : "Arrow Method") {
}


KstBindArrow::~KstBindArrow() {
}


KJS::Object KstBindArrow::construct(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Object();
  }

  KstViewObjectPtr view = extractViewObject(exec, args[0]);
  if (!view) {
    KstViewWindow *w = extractWindow(exec, args[0]);
    if (w) {
      view = w->view();
    } else {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }
  }

  KstViewArrowPtr b = new KstViewArrow;
  view->appendChild(b.data());
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
  return KJS::Object(new KstBindArrow(exec, b));
}


struct ArrowBindings {
  const char *name;
  KJS::Value (KstBindArrow::*method)(KJS::ExecState*, const KJS::List&);
};


struct ArrowProperties {
  const char *name;
  void (KstBindArrow::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindArrow::*get)(KJS::ExecState*) const;
};


static ArrowBindings arrowBindings[] = {
  { 0L, 0L }
};


static ArrowProperties arrowProperties[] = {
  { 0L, 0L, 0L }
};


int KstBindArrow::methodCount() const {
  return sizeof arrowBindings + KstBindLine::methodCount();
}


int KstBindArrow::propertyCount() const {
  return sizeof arrowProperties + KstBindLine::propertyCount();
}


KJS::ReferenceList KstBindArrow::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindLine::propList(exec, recursive);

  for (int i = 0; arrowProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(arrowProperties[i].name)));
  }

  return rc;
}


bool KstBindArrow::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; arrowProperties[i].name; ++i) {
    if (prop == arrowProperties[i].name) {
      return true;
    }
  }

  return KstBindLine::hasProperty(exec, propertyName);
}


void KstBindArrow::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindLine::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; arrowProperties[i].name; ++i) {
    if (prop == arrowProperties[i].name) {
      if (!arrowProperties[i].set) {
        break;
      }
      (this->*arrowProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindLine::put(exec, propertyName, value, attr);
}


KJS::Value KstBindArrow::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindLine::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; arrowProperties[i].name; ++i) {
    if (prop == arrowProperties[i].name) {
      if (!arrowProperties[i].get) {
        break;
      }
      return (this->*arrowProperties[i].get)(exec);
    }
  }
  
  return KstBindLine::get(exec, propertyName);
}


KJS::Value KstBindArrow::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindLine::methodCount();
  if (id > start) {
    KstBindArrow *imp = dynamic_cast<KstBindArrow*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*arrowBindings[id - start - 1].method)(exec, args);
  } 

  return KstBindLine::call(exec, self, args);
}


void KstBindArrow::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindLine::methodCount();
  for (int i = 0; arrowBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindArrow(i + start + 1));
    obj.put(exec, arrowBindings[i].name, o, KJS::Function);
  }
}

// vim: ts=2 sw=2 et
