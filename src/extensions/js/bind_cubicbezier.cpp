/***************************************************************************
                             bind_cubicbezier.cpp
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

#include "bind_cubicbezier.h"
#include "bind_point.h"

#include <kst.h>
#include <kstviewwindow.h>

#include <kdebug.h>

KstBindCubicBezier::KstBindCubicBezier(KJS::ExecState *exec, KstViewBezierPtr d, const char *name)
: KstBindViewObject(exec, d.data(), name ? name : "CubicBezier") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindCubicBezier::KstBindCubicBezier(KJS::ExecState *exec, KJS::Object *globalObject, const char *name)
: KstBindViewObject(exec, globalObject, name ? name : "CubicBezier") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    KstBindViewObject::addFactory("Bezier", KstBindCubicBezier::bindFactory);
  }
}


KstBindViewObject *KstBindCubicBezier::bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj) {
  KstViewBezierPtr v = kst_cast<KstViewBezier>(obj);
  if (v) {
    return new KstBindCubicBezier(exec, v);
  }
  return 0L;
}


KstBindCubicBezier::KstBindCubicBezier(int id, const char *name)
: KstBindViewObject(id, name ? name : "CubicBezier Method") {
}


KstBindCubicBezier::~KstBindCubicBezier() {
}


KJS::Object KstBindCubicBezier::construct(KJS::ExecState *exec, const KJS::List& args) {
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

  KstViewBezierPtr b = new KstViewBezier;
  view->appendChild(b.data());
  KstApp::inst()->paintAll(P_PAINT);
  return KJS::Object(new KstBindCubicBezier(exec, b));
}


struct BezierBindings {
  const char *name;
  KJS::Value (KstBindCubicBezier::*method)(KJS::ExecState*, const KJS::List&);
};


struct BezierProperties {
  const char *name;
  void (KstBindCubicBezier::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindCubicBezier::*get)(KJS::ExecState*) const;
};


static BezierBindings bezierBindings[] = {
  { 0L, 0L }
};


static BezierProperties bezierProperties[] = {
  { "a", &KstBindCubicBezier::setA, &KstBindCubicBezier::a },
  { "b", &KstBindCubicBezier::setB, &KstBindCubicBezier::b },
  { "c", &KstBindCubicBezier::setC, &KstBindCubicBezier::c },
  { "d", &KstBindCubicBezier::setD, &KstBindCubicBezier::d },
  { "width", &KstBindCubicBezier::setWidth, &KstBindCubicBezier::width },
  { "lineStyle", &KstBindCubicBezier::setLineStyle, &KstBindCubicBezier::lineStyle },
  { "capStyle", &KstBindCubicBezier::setCapStyle, &KstBindCubicBezier::capStyle },
  { 0L, 0L, 0L }
};


int KstBindCubicBezier::methodCount() const {
  return sizeof bezierBindings + KstBindViewObject::methodCount();
}


int KstBindCubicBezier::propertyCount() const {
  return sizeof bezierProperties + KstBindViewObject::propertyCount();
}


KJS::ReferenceList KstBindCubicBezier::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindViewObject::propList(exec, recursive);

  for (int i = 0; bezierProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(bezierProperties[i].name)));
  }

  return rc;
}


bool KstBindCubicBezier::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; bezierProperties[i].name; ++i) {
    if (prop == bezierProperties[i].name) {
      return true;
    }
  }

  return KstBindViewObject::hasProperty(exec, propertyName);
}


void KstBindCubicBezier::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindViewObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; bezierProperties[i].name; ++i) {
    if (prop == bezierProperties[i].name) {
      if (!bezierProperties[i].set) {
        break;
      }
      (this->*bezierProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindViewObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindCubicBezier::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindViewObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; bezierProperties[i].name; ++i) {
    if (prop == bezierProperties[i].name) {
      if (!bezierProperties[i].get) {
        break;
      }
      return (this->*bezierProperties[i].get)(exec);
    }
  }
  
  return KstBindViewObject::get(exec, propertyName);
}


KJS::Value KstBindCubicBezier::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindViewObject::methodCount();
  if (id > start) {
    KstBindCubicBezier *imp = dynamic_cast<KstBindCubicBezier*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*bezierBindings[id - start - 1].method)(exec, args);
  } 

  return KstBindViewObject::call(exec, self, args);
}


void KstBindCubicBezier::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindViewObject::methodCount();
  for (int i = 0; bezierBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindCubicBezier(i + start + 1));
    obj.put(exec, bezierBindings[i].name, o, KJS::Function);
  }
}


#define makeBezier(X) dynamic_cast<KstViewBezier*>(const_cast<KstObject*>(X.data()))

void KstBindCubicBezier::setWidth(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned w = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(w)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewBezierPtr d = makeBezier(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setWidth(w);
    KstApp::inst()->paintAll(P_PAINT);
  }
}


KJS::Value KstBindCubicBezier::width(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewBezierPtr d = makeBezier(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->width());
  }
  return KJS::Number(0);
}


void KstBindCubicBezier::setA(KJS::ExecState *exec, const KJS::Value& value) {
  KstBindPoint *imp = 0L;
  if (value.type() != KJS::ObjectType || !(imp = dynamic_cast<KstBindPoint*>(value.toObject(exec).imp()))) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewBezierPtr d = makeBezier(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setPointA(QPoint(int(imp->_x), int(imp->_y))); // FIXME: Point is the wrong
                                                 // type to use here.  It's
                                                 // a double, for one...
    KstApp::inst()->paintAll(P_PAINT);
  }
}


KJS::Value KstBindCubicBezier::a(KJS::ExecState *exec) const {
  KstViewBezierPtr d = makeBezier(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Object(new KstBindPoint(exec, d->pointA().x(), d->pointA().y()));
  }
  return KJS::Null();
}


void KstBindCubicBezier::setB(KJS::ExecState *exec, const KJS::Value& value) {
  KstBindPoint *imp = 0L;
  if (value.type() != KJS::ObjectType || !(imp = dynamic_cast<KstBindPoint*>(value.toObject(exec).imp()))) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewBezierPtr d = makeBezier(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setPointB(QPoint(int(imp->_x), int(imp->_y))); // FIXME: Point is the wrong
                                                 // type to use here.  It's
                                                 // a double, for one...
    KstApp::inst()->paintAll(P_PAINT);
  }
}


KJS::Value KstBindCubicBezier::b(KJS::ExecState *exec) const {
  KstViewBezierPtr d = makeBezier(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Object(new KstBindPoint(exec, d->pointB().x(), d->pointB().y()));
  }
  return KJS::Null();
}


void KstBindCubicBezier::setC(KJS::ExecState *exec, const KJS::Value& value) {
  KstBindPoint *imp = 0L;
  if (value.type() != KJS::ObjectType || !(imp = dynamic_cast<KstBindPoint*>(value.toObject(exec).imp()))) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewBezierPtr d = makeBezier(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setPointC(QPoint(int(imp->_x), int(imp->_y))); // FIXME: Point is the wrong
                                                 // type to use here.  It's
                                                 // a double, for one...
    KstApp::inst()->paintAll(P_PAINT);
  }
}


KJS::Value KstBindCubicBezier::c(KJS::ExecState *exec) const {
  KstViewBezierPtr d = makeBezier(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Object(new KstBindPoint(exec, d->pointC().x(), d->pointC().y()));
  }
  return KJS::Null();
}


void KstBindCubicBezier::setD(KJS::ExecState *exec, const KJS::Value& value) {
  KstBindPoint *imp = 0L;
  if (value.type() != KJS::ObjectType || !(imp = dynamic_cast<KstBindPoint*>(value.toObject(exec).imp()))) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewBezierPtr d = makeBezier(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setPointD(QPoint(int(imp->_x), int(imp->_y))); // FIXME: Point is the wrong
                                                 // type to use here.  It's
                                                 // a double, for one...
    KstApp::inst()->paintAll(P_PAINT);
  }
}


KJS::Value KstBindCubicBezier::d(KJS::ExecState *exec) const {
  KstViewBezierPtr d = makeBezier(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Object(new KstBindPoint(exec, d->pointD().x(), d->pointD().y()));
  }
  return KJS::Null();
}


void KstBindCubicBezier::setCapStyle(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned w = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(w)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewBezierPtr d = makeBezier(_d);
  if (d) {
    KstWriteLocker wl(d);
    switch (w) {
      case 0:
        d->setCapStyle(Qt::FlatCap);
        break;
      case 1:
        d->setCapStyle(Qt::SquareCap);
        break;
      case 2:
        d->setCapStyle(Qt::RoundCap);
        break;
      default:
        return;
    }
    KstApp::inst()->paintAll(P_PAINT);
  }
}


KJS::Value KstBindCubicBezier::capStyle(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewBezierPtr d = makeBezier(_d);
  if (d) {
    KstReadLocker rl(d);
    switch (d->capStyle()) {
      case Qt::FlatCap:
        return KJS::Number(0);
      case Qt::SquareCap:
        return KJS::Number(1);
      case Qt::RoundCap:
        return KJS::Number(2);
      default:
        break;
    }
  }
  return KJS::Number(0);
}


void KstBindCubicBezier::setLineStyle(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned w = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(w)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewBezierPtr d = makeBezier(_d);
  if (d) {
    KstWriteLocker wl(d);
    switch (w) {
      case 0:
        d->setPenStyle(Qt::SolidLine);
        break;
      case 1:
        d->setPenStyle(Qt::DashLine);
        break;
      case 2:
        d->setPenStyle(Qt::DotLine);
        break;
      case 3:
        d->setPenStyle(Qt::DashDotLine);
        break;
      case 4:
        d->setPenStyle(Qt::DashDotDotLine);
        break;
      default:
        return;
    }
    KstApp::inst()->paintAll(P_PAINT);
  }
}


KJS::Value KstBindCubicBezier::lineStyle(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewBezierPtr d = makeBezier(_d);
  if (d) {
    KstReadLocker rl(d);
    switch (d->penStyle()) {
      case Qt::SolidLine:
        return KJS::Number(0);
      case Qt::DashLine:
        return KJS::Number(1);
      case Qt::DotLine:
        return KJS::Number(2);
      case Qt::DashDotLine:
        return KJS::Number(3);
      case Qt::DashDotDotLine:
        return KJS::Number(4);
      default:
        break;
    }
  }
  return KJS::Number(0);
}


#undef makeBezier

// vim: ts=2 sw=2 et
