/***************************************************************************
                         bind_borderedviewobject.cpp
                               ---------------
    begin                : Dec 08 2005
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

#include "bind_borderedviewobject.h"

#include <kst.h>
#include <kstviewwindow.h>

#include <kdebug.h>
#include <kjsembed/jsbinding.h>

KstBindBorderedViewObject::KstBindBorderedViewObject(KJS::ExecState *exec, KstBorderedViewObjectPtr d, const char *name)
: KstBindViewObject(exec, d.data(), name ? name : "BorderedViewObject") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindBorderedViewObject::KstBindBorderedViewObject(KJS::ExecState *exec, KJS::Object *globalObject, const char *name)
: KstBindViewObject(exec, globalObject, name ? name : "BorderedViewObject") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    KstBindViewObject::addFactory("BorderedViewObject", KstBindBorderedViewObject::bindFactory);
  }
}


KstBindViewObject *KstBindBorderedViewObject::bindFactory(KJS::ExecState *exec, KstViewObjectPtr obj) {
  KstBorderedViewObjectPtr v = kst_cast<KstBorderedViewObject>(obj);
  if (v) {
    return new KstBindBorderedViewObject(exec, v);
  }
  return 0L;
}


KstBindBorderedViewObject::KstBindBorderedViewObject(int id, const char *name)
: KstBindViewObject(id, name ? name : "BorderedViewObject Method") {
}


KstBindBorderedViewObject::~KstBindBorderedViewObject() {
}


KJS::Object KstBindBorderedViewObject::construct(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  return KJS::Object(new KstBindBorderedViewObject(exec));
}


struct BorderedViewObjectBindings {
  const char *name;
  KJS::Value (KstBindBorderedViewObject::*method)(KJS::ExecState*, const KJS::List&);
};


struct BorderedViewObjectProperties {
  const char *name;
  void (KstBindBorderedViewObject::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindBorderedViewObject::*get)(KJS::ExecState*) const;
};


static BorderedViewObjectBindings borderedViewObjectBindings[] = {
  { 0L, 0L }
};


static BorderedViewObjectProperties borderedViewObjectProperties[] = {
  { "padding", &KstBindBorderedViewObject::setPadding, &KstBindBorderedViewObject::padding },
  { "margin", &KstBindBorderedViewObject::setMargin, &KstBindBorderedViewObject::margin },
  { "borderWidth", &KstBindBorderedViewObject::setBorderWidth, &KstBindBorderedViewObject::borderWidth },
  { "borderColor", &KstBindBorderedViewObject::setBorderColor, &KstBindBorderedViewObject::borderColor },
  { 0L, 0L, 0L }
};


int KstBindBorderedViewObject::methodCount() const {
  return sizeof borderedViewObjectBindings + KstBindViewObject::methodCount();
}


int KstBindBorderedViewObject::propertyCount() const {
  return sizeof borderedViewObjectProperties + KstBindViewObject::propertyCount();
}


KJS::ReferenceList KstBindBorderedViewObject::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindViewObject::propList(exec, recursive);

  for (int i = 0; borderedViewObjectProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(borderedViewObjectProperties[i].name)));
  }

  return rc;
}


bool KstBindBorderedViewObject::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; borderedViewObjectProperties[i].name; ++i) {
    if (prop == borderedViewObjectProperties[i].name) {
      return true;
    }
  }

  return KstBindViewObject::hasProperty(exec, propertyName);
}


void KstBindBorderedViewObject::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindViewObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; borderedViewObjectProperties[i].name; ++i) {
    if (prop == borderedViewObjectProperties[i].name) {
      if (!borderedViewObjectProperties[i].set) {
        break;
      }
      (this->*borderedViewObjectProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindViewObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindBorderedViewObject::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindViewObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; borderedViewObjectProperties[i].name; ++i) {
    if (prop == borderedViewObjectProperties[i].name) {
      if (!borderedViewObjectProperties[i].get) {
        break;
      }
      return (this->*borderedViewObjectProperties[i].get)(exec);
    }
  }
  
  return KstBindViewObject::get(exec, propertyName);
}


KJS::Value KstBindBorderedViewObject::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindViewObject::methodCount();
  if (id > start) {
    KstBindBorderedViewObject *imp = dynamic_cast<KstBindBorderedViewObject*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*borderedViewObjectBindings[id - start - 1].method)(exec, args);
  } 

  return KstBindViewObject::call(exec, self, args);
}


void KstBindBorderedViewObject::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindViewObject::methodCount();
  for (int i = 0; borderedViewObjectBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindBorderedViewObject(i + start + 1));
    obj.put(exec, borderedViewObjectBindings[i].name, o, KJS::Function);
  }
}


#define makeBorderedViewObject(X) dynamic_cast<KstBorderedViewObject*>(const_cast<KstObject*>(X.data()))

void KstBindBorderedViewObject::setPadding(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned padding = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(padding)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstBorderedViewObjectPtr d = makeBorderedViewObject(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setPadding(padding);
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindBorderedViewObject::padding(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstBorderedViewObjectPtr d = makeBorderedViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->padding());
  }
  return KJS::Number(0);
}


void KstBindBorderedViewObject::setMargin(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned margin = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(margin)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstBorderedViewObjectPtr d = makeBorderedViewObject(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setMargin(margin);
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindBorderedViewObject::margin(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstBorderedViewObjectPtr d = makeBorderedViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->margin());
  }
  return KJS::Number(0);
}


void KstBindBorderedViewObject::setBorderWidth(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned borderWidth = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(borderWidth)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstBorderedViewObjectPtr d = makeBorderedViewObject(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setBorderWidth(borderWidth);
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindBorderedViewObject::borderWidth(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstBorderedViewObjectPtr d = makeBorderedViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->borderWidth());
  }
  return KJS::Number(0);
}


void KstBindBorderedViewObject::setBorderColor(KJS::ExecState *exec, const KJS::Value& value) {
  QVariant cv = KJSEmbed::convertToVariant(exec, value);
  if (!cv.canCast(QVariant::Color)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstBorderedViewObjectPtr d = makeBorderedViewObject(_d);
  if (d) {
    KstWriteLocker rl(d);
    d->setBorderColor(cv.toColor());
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindBorderedViewObject::borderColor(KJS::ExecState *exec) const {
  KstBorderedViewObjectPtr d = makeBorderedViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJSEmbed::convertToValue(exec, d->borderColor());
  }
  return KJSEmbed::convertToValue(exec, QColor());
}


#undef makeBorderedViewObject
// vim: ts=2 sw=2 et
