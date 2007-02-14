/***************************************************************************
                                bind_ellipse.cpp
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

#include "bind_ellipse.h"

#include <kst.h>
#include <kstviewwindow.h>

#include <kdebug.h>
#include <kjsembed/jsbinding.h>

KstBindEllipse::KstBindEllipse(KJS::ExecState *exec, KstViewEllipsePtr d, const char *name)
: KstBindViewObject(exec, d.data(), name ? name : "Ellipse") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindEllipse::KstBindEllipse(KJS::ExecState *exec, KJS::Object *globalObject, const char *name)
: KstBindViewObject(exec, globalObject, name ? name : "Ellipse") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    KstBindViewObject::addFactory("Ellipse", KstBindEllipse::bindFactory);
  }
}


KstBindViewObject *KstBindEllipse::bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj) {
  KstViewEllipsePtr v = kst_cast<KstViewEllipse>(obj);
  if (v) {
    return new KstBindEllipse(exec, v);
  }
  return 0L;
}


KstBindEllipse::KstBindEllipse(int id, const char *name)
: KstBindViewObject(id, name ? name : "Ellipse Method") {
}


KstBindEllipse::~KstBindEllipse() {
}


KJS::Object KstBindEllipse::construct(KJS::ExecState *exec, const KJS::List& args) {
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

  KstViewEllipsePtr b = new KstViewEllipse;
  view->appendChild(b.data());
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
  return KJS::Object(new KstBindEllipse(exec, b));
}


struct EllipseBindings {
  const char *name;
  KJS::Value (KstBindEllipse::*method)(KJS::ExecState*, const KJS::List&);
};


struct EllipseProperties {
  const char *name;
  void (KstBindEllipse::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindEllipse::*get)(KJS::ExecState*) const;
};


static EllipseBindings ellipseBindings[] = {
  { 0L, 0L }
};


static EllipseProperties ellipseProperties[] = {
  { "borderColor", &KstBindEllipse::setBorderColor, &KstBindEllipse::borderColor },
  { "borderWidth", &KstBindEllipse::setBorderWidth, &KstBindEllipse::borderWidth },
  { 0L, 0L, 0L }
};


int KstBindEllipse::methodCount() const {
  return sizeof ellipseBindings + KstBindViewObject::methodCount();
}


int KstBindEllipse::propertyCount() const {
  return sizeof ellipseProperties + KstBindViewObject::propertyCount();
}


KJS::ReferenceList KstBindEllipse::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindViewObject::propList(exec, recursive);

  for (int i = 0; ellipseProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(ellipseProperties[i].name)));
  }

  return rc;
}


bool KstBindEllipse::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; ellipseProperties[i].name; ++i) {
    if (prop == ellipseProperties[i].name) {
      return true;
    }
  }

  return KstBindViewObject::hasProperty(exec, propertyName);
}


void KstBindEllipse::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindViewObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; ellipseProperties[i].name; ++i) {
    if (prop == ellipseProperties[i].name) {
      if (!ellipseProperties[i].set) {
        break;
      }
      (this->*ellipseProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindViewObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindEllipse::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindViewObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; ellipseProperties[i].name; ++i) {
    if (prop == ellipseProperties[i].name) {
      if (!ellipseProperties[i].get) {
        break;
      }
      return (this->*ellipseProperties[i].get)(exec);
    }
  }
  
  return KstBindViewObject::get(exec, propertyName);
}


KJS::Value KstBindEllipse::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindViewObject::methodCount();
  if (id > start) {
    KstBindEllipse *imp = dynamic_cast<KstBindEllipse*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*ellipseBindings[id - start - 1].method)(exec, args);
  } 

  return KstBindViewObject::call(exec, self, args);
}


void KstBindEllipse::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindViewObject::methodCount();
  for (int i = 0; ellipseBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindEllipse(i + start + 1));
    obj.put(exec, ellipseBindings[i].name, o, KJS::Function);
  }
}


#define makeEllipse(X) dynamic_cast<KstViewEllipse*>(const_cast<KstObject*>(X.data()))

void KstBindEllipse::setBorderColor(KJS::ExecState *exec, const KJS::Value& value) {
  QVariant cv = KJSEmbed::convertToVariant(exec, value);
  if (!cv.canCast(QVariant::Color)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewEllipsePtr d = makeEllipse(_d);
  if (d) {
    KstWriteLocker rl(d);
    d->setBorderColor(cv.toColor());
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindEllipse::borderColor(KJS::ExecState *exec) const {
  KstViewEllipsePtr d = makeEllipse(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJSEmbed::convertToValue(exec, d->borderColor());
  }
  return KJSEmbed::convertToValue(exec, QColor());
}


void KstBindEllipse::setBorderWidth(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned w = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(w)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewEllipsePtr d = makeEllipse(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setBorderWidth(w);
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindEllipse::borderWidth(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewEllipsePtr d = makeEllipse(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->borderWidth());
  }
  return KJS::Number(0);
}

#undef makeEllipse

// vim: ts=2 sw=2 et
