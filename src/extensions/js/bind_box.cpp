/***************************************************************************
                                bind_box.cpp
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

#include "bind_box.h"

#include <kst.h>
#include <kstviewwindow.h>

#include <kdebug.h>
#include <kjsembed/jsbinding.h>

KstBindBox::KstBindBox(KJS::ExecState *exec, KstViewBoxPtr d, const char *name)
: KstBindViewObject(exec, d.data(), name ? name : "Box") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindBox::KstBindBox(KJS::ExecState *exec, KJS::Object *globalObject, const char *name)
: KstBindViewObject(exec, globalObject, name ? name : "Box") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    KstBindViewObject::addFactory("Box", KstBindBox::bindFactory);
  }
}


KstBindViewObject *KstBindBox::bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj) {
  KstViewBoxPtr v = kst_cast<KstViewBox>(obj);
  if (v) {
    return new KstBindBox(exec, v);
  }
  return 0L;
}


KstBindBox::KstBindBox(int id, const char *name)
: KstBindViewObject(id, name ? name : "Box Method") {
}


KstBindBox::~KstBindBox() {
}


KJS::Object KstBindBox::construct(KJS::ExecState *exec, const KJS::List& args) {
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

  KstViewBoxPtr b = new KstViewBox;
  view->appendChild(b.data());
  KstApp::inst()->paintAll(KstPainter::P_PAINT);
  return KJS::Object(new KstBindBox(exec, b));
}


struct BoxBindings {
  const char *name;
  KJS::Value (KstBindBox::*method)(KJS::ExecState*, const KJS::List&);
};


struct BoxProperties {
  const char *name;
  void (KstBindBox::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindBox::*get)(KJS::ExecState*) const;
};


static BoxBindings boxBindings[] = {
  { 0L, 0L }
};


static BoxProperties boxProperties[] = {
  { "xRound", &KstBindBox::setXRound, &KstBindBox::xRound },
  { "yRound", &KstBindBox::setYRound, &KstBindBox::yRound },
  { "borderWidth", &KstBindBox::setBorderWidth, &KstBindBox::borderWidth },
  { "borderColor", &KstBindBox::setBorderColor, &KstBindBox::borderColor },
  { 0L, 0L, 0L }
};


int KstBindBox::methodCount() const {
  return sizeof boxBindings + KstBindViewObject::methodCount();
}


int KstBindBox::propertyCount() const {
  return sizeof boxProperties + KstBindViewObject::propertyCount();
}


KJS::ReferenceList KstBindBox::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindViewObject::propList(exec, recursive);

  for (int i = 0; boxProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(boxProperties[i].name)));
  }

  return rc;
}


bool KstBindBox::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; boxProperties[i].name; ++i) {
    if (prop == boxProperties[i].name) {
      return true;
    }
  }

  return KstBindViewObject::hasProperty(exec, propertyName);
}


void KstBindBox::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindViewObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; boxProperties[i].name; ++i) {
    if (prop == boxProperties[i].name) {
      if (!boxProperties[i].set) {
        break;
      }
      (this->*boxProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindViewObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindBox::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindViewObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; boxProperties[i].name; ++i) {
    if (prop == boxProperties[i].name) {
      if (!boxProperties[i].get) {
        break;
      }
      return (this->*boxProperties[i].get)(exec);
    }
  }
  
  return KstBindViewObject::get(exec, propertyName);
}


KJS::Value KstBindBox::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindViewObject::methodCount();
  if (id > start) {
    KstBindBox *imp = dynamic_cast<KstBindBox*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*boxBindings[id - start - 1].method)(exec, args);
  } 

  return KstBindViewObject::call(exec, self, args);
}


void KstBindBox::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindViewObject::methodCount();
  for (int i = 0; boxBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindBox(i + start + 1));
    obj.put(exec, boxBindings[i].name, o, KJS::Function);
  }
}


#define makeBox(X) dynamic_cast<KstViewBox*>(const_cast<KstObject*>(X.data()))

void KstBindBox::setXRound(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned w = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(w)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewBoxPtr d = makeBox(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setXRound(w);
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindBox::xRound(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewBoxPtr d = makeBox(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->xRound());
  }
  return KJS::Number(0);
}


void KstBindBox::setYRound(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned w = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(w)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewBoxPtr d = makeBox(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setYRound(w);
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindBox::yRound(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewBoxPtr d = makeBox(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->yRound());
  }
  return KJS::Number(0);
}


void KstBindBox::setCornerStyle(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned w = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(w)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewBoxPtr d = makeBox(_d);
  if (d) {
    KstWriteLocker wl(d);
    switch (w) {
      case 0:
        d->setCornerStyle(Qt::MiterJoin);
        break;
      case 1:
        d->setCornerStyle(Qt::BevelJoin);
        break;
      case 2:
        d->setCornerStyle(Qt::RoundJoin);
        break;
      default:
        return;
    }
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindBox::cornerStyle(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewBoxPtr d = makeBox(_d);
  if (d) {
    KstReadLocker rl(d);
    switch (d->cornerStyle()) {
      case Qt::MiterJoin:
        return KJS::Number(0);
      case Qt::BevelJoin:
        return KJS::Number(1);
      case Qt::RoundJoin:
        return KJS::Number(2);
      default:
        break;
    }
  }
  return KJS::Number(0);
}


void KstBindBox::setBorderWidth(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned borderWidth = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(borderWidth)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewBoxPtr d = makeBox(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setBorderWidth(borderWidth);
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindBox::borderWidth(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewBoxPtr d = makeBox(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->borderWidth());
  }
  return KJS::Number(0);
}


void KstBindBox::setBorderColor(KJS::ExecState *exec, const KJS::Value& value) {
  QVariant cv = KJSEmbed::convertToVariant(exec, value);
  if (!cv.canCast(QVariant::Color)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewBoxPtr d = makeBox(_d);
  if (d) {
    KstWriteLocker rl(d);
    d->setBorderColor(cv.toColor());
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindBox::borderColor(KJS::ExecState *exec) const {
  KstViewBoxPtr d = makeBox(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJSEmbed::convertToValue(exec, d->borderColor());
  }
  return KJSEmbed::convertToValue(exec, QColor());
}


#undef makeBox

// vim: ts=2 sw=2 et
