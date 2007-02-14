/***************************************************************************
                                bind_line.cpp
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

#include "bind_line.h"
#include "bind_point.h"

#include <kst.h>
#include <kstviewwindow.h>

#include <kdebug.h>

KstBindLine::KstBindLine(KJS::ExecState *exec, KstViewLinePtr d, const char *name)
: KstBindViewObject(exec, d.data(), name ? name : "Line") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindLine::KstBindLine(KJS::ExecState *exec, KJS::Object *globalObject, const char *name)
: KstBindViewObject(exec, globalObject, name ? name : "Line") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    KstBindViewObject::addFactory("Line", KstBindLine::bindFactory);
  }
}


KstBindViewObject *KstBindLine::bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj) {
  KstViewLinePtr v = kst_cast<KstViewLine>(obj);
  if (v) {
    return new KstBindLine(exec, v);
  }
  return 0L;
}


KstBindLine::KstBindLine(int id, const char *name)
: KstBindViewObject(id, name ? name : "Line Method") {
}


KstBindLine::~KstBindLine() {
}


KJS::Object KstBindLine::construct(KJS::ExecState *exec, const KJS::List& args) {
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

  KstViewLinePtr b = new KstViewLine;
  view->appendChild(b.data());
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
  return KJS::Object(new KstBindLine(exec, b));
}


struct LineBindings {
  const char *name;
  KJS::Value (KstBindLine::*method)(KJS::ExecState*, const KJS::List&);
};


struct LineProperties {
  const char *name;
  void (KstBindLine::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindLine::*get)(KJS::ExecState*) const;
};


static LineBindings lineBindings[] = {
  { 0L, 0L }
};


static LineProperties lineProperties[] = {
  { "from", &KstBindLine::setFrom, &KstBindLine::from },
  { "to", &KstBindLine::setTo, &KstBindLine::to },
  { "width", &KstBindLine::setWidth, &KstBindLine::width },
  { "lineStyle", &KstBindLine::setLineStyle, &KstBindLine::lineStyle },
  { "capStyle", &KstBindLine::setCapStyle, &KstBindLine::capStyle },
  { 0L, 0L, 0L }
};


int KstBindLine::methodCount() const {
  return sizeof lineBindings + KstBindViewObject::methodCount();
}


int KstBindLine::propertyCount() const {
  return sizeof lineProperties + KstBindViewObject::propertyCount();
}


KJS::ReferenceList KstBindLine::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindViewObject::propList(exec, recursive);

  for (int i = 0; lineProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(lineProperties[i].name)));
  }

  return rc;
}


bool KstBindLine::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; lineProperties[i].name; ++i) {
    if (prop == lineProperties[i].name) {
      return true;
    }
  }

  return KstBindViewObject::hasProperty(exec, propertyName);
}


void KstBindLine::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindViewObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; lineProperties[i].name; ++i) {
    if (prop == lineProperties[i].name) {
      if (!lineProperties[i].set) {
        break;
      }
      (this->*lineProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindViewObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindLine::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindViewObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; lineProperties[i].name; ++i) {
    if (prop == lineProperties[i].name) {
      if (!lineProperties[i].get) {
        break;
      }
      return (this->*lineProperties[i].get)(exec);
    }
  }
  
  return KstBindViewObject::get(exec, propertyName);
}


KJS::Value KstBindLine::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindViewObject::methodCount();
  if (id > start) {
    KstBindLine *imp = dynamic_cast<KstBindLine*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*lineBindings[id - start - 1].method)(exec, args);
  } 

  return KstBindViewObject::call(exec, self, args);
}


void KstBindLine::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindViewObject::methodCount();
  for (int i = 0; lineBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindLine(i + start + 1));
    obj.put(exec, lineBindings[i].name, o, KJS::Function);
  }
}


#define makeLine(X) dynamic_cast<KstViewLine*>(const_cast<KstObject*>(X.data()))

void KstBindLine::setWidth(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned w = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(w)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewLinePtr d = makeLine(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setWidth(w);
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindLine::width(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewLinePtr d = makeLine(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->width());
  }
  return KJS::Number(0);
}


void KstBindLine::setFrom(KJS::ExecState *exec, const KJS::Value& value) {
  KstBindPoint *imp = 0L;
  if (value.type() != KJS::ObjectType || !(imp = dynamic_cast<KstBindPoint*>(value.toObject(exec).imp()))) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewLinePtr d = makeLine(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setFrom(QPoint(int(imp->_x), int(imp->_y)));
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindLine::from(KJS::ExecState *exec) const {
  KstViewLinePtr d = makeLine(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Object(new KstBindPoint(exec, d->from().x(), d->from().y()));
  }
  return KJS::Null();
}


void KstBindLine::setTo(KJS::ExecState *exec, const KJS::Value& value) {
  KstBindPoint *imp = 0L;
  if (value.type() != KJS::ObjectType || !(imp = dynamic_cast<KstBindPoint*>(value.toObject(exec).imp()))) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewLinePtr d = makeLine(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setTo(QPoint(int(imp->_x), int(imp->_y)));
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindLine::to(KJS::ExecState *exec) const {
  KstViewLinePtr d = makeLine(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Object(new KstBindPoint(exec, d->to().x(), d->to().y()));
  }
  return KJS::Null();
}


void KstBindLine::setCapStyle(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned w = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(w)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewLinePtr d = makeLine(_d);
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
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindLine::capStyle(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewLinePtr d = makeLine(_d);
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


void KstBindLine::setLineStyle(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned w = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(w)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewLinePtr d = makeLine(_d);
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
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindLine::lineStyle(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewLinePtr d = makeLine(_d);
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


#undef makeLine

// vim: ts=2 sw=2 et
