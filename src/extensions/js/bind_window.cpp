/***************************************************************************
                              bind_window.cpp
                             -------------------
    begin                : Mar 30 2005
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

#include "bind_window.h"
#include "bind_plotcollection.h"
#include "bind_viewobject.h"

#include <kst.h>
#include <kstviewwindow.h>

#include <kdebug.h>

KstBindWindow::KstBindWindow(KJS::ExecState *exec, KstViewWindow *d)
: KstBinding("Window"), _d(d) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindWindow::KstBindWindow(KJS::ExecState *exec, KJS::Object *globalObject)
: KstBinding("Window") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    globalObject->put(exec, "Window", o);
  } else {
    _d = new KstViewWindow;
  }
}


KstBindWindow::KstBindWindow(int id)
: KstBinding("Window Method", id) {
}


KstBindWindow::~KstBindWindow() {
}


KJS::Object KstBindWindow::construct(KJS::ExecState *exec, const KJS::List& args) {
  QString name;
  if (args.size() > 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Object();
  }

  if (args.size() == 1) {
    if (args[0].type() != KJS::StringType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }
    name = args[0].toString(exec).qstring();
  }

  name = KstApp::inst()->newWindow(name);
  KstViewWindow *w = dynamic_cast<KstViewWindow*>(KstApp::inst()->findWindow(name));
  if (!w) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Object();
  }

  return KJS::Object(new KstBindWindow(exec, w));
}


struct WindowBindings {
  const char *name;
  KJS::Value (KstBindWindow::*method)(KJS::ExecState*, const KJS::List&);
};


struct WindowProperties {
  const char *name;
  void (KstBindWindow::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindWindow::*get)(KJS::ExecState*) const;
};


static WindowBindings windowBindings[] = {
  { "close", &KstBindWindow::close },
  { 0L, 0L }
};


static WindowProperties windowProperties[] = {
  { "name", &KstBindWindow::setWindowName, &KstBindWindow::windowName },
  { "plots", 0L, &KstBindWindow::plots },
  { "view", 0L, &KstBindWindow::view },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindWindow::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; windowProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(windowProperties[i].name)));
  }

  return rc;
}


bool KstBindWindow::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; windowProperties[i].name; ++i) {
    if (prop == windowProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


void KstBindWindow::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBinding::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; windowProperties[i].name; ++i) {
    if (prop == windowProperties[i].name) {
      if (!windowProperties[i].set) {
        break;
      }
      (this->*windowProperties[i].set)(exec, value);
      return;
    }
  }

  KstBinding::put(exec, propertyName, value, attr);
}


KJS::Value KstBindWindow::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBinding::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; windowProperties[i].name; ++i) {
    if (prop == windowProperties[i].name) {
      if (!windowProperties[i].get) {
        break;
      }
      return (this->*windowProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindWindow::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindWindow *imp = dynamic_cast<KstBindWindow*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*windowBindings[id - 1].method)(exec, args);
}


void KstBindWindow::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; windowBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindWindow(i + 1));
    obj.put(exec, windowBindings[i].name, o, KJS::Function);
  }
}


void KstBindWindow::setWindowName(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  _d->setCaption(value.toString(exec).qstring());
}


KJS::Value KstBindWindow::windowName(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::String(_d->caption());
}


KJS::Value KstBindWindow::close(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  if (!_d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  _d->view()->children().clear();
  _d->close();
  return KJS::Undefined();
}


KJS::Value KstBindWindow::plots(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindPlotCollection(exec, _d));
}


KJS::Value KstBindWindow::view(KJS::ExecState *exec) const {
  return KJS::Object(KstBindViewObject::bind(exec, _d->view().data()));
}


// vim: ts=2 sw=2 et
