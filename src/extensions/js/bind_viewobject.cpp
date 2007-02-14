/***************************************************************************
                              bind_viewobject.cpp
                             -------------------
    begin                : May 29 2005
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

#include "bind_viewobject.h"
#include "bind_viewobjectcollection.h"

#include "bind_point.h"
#include "bind_size.h"

#include <kst.h>

#include <kdebug.h>
#include <kjsembed/jsbinding.h>

KstBindViewObject::KstBindViewObject(KJS::ExecState *exec, KstViewObjectPtr d, const char *name)
: KstBindObject(exec, d.data(), name ? name : "ViewObject") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindViewObject::KstBindViewObject(KJS::ExecState *exec, KJS::Object *globalObject, const char *name)
: KstBindObject(exec, globalObject, name ? name : "ViewObject") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindViewObject::KstBindViewObject(int id, const char *name)
: KstBindObject(id, name ? name : "ViewObject Method") {
}


KstBindViewObject::~KstBindViewObject() {
}


KJS::Object KstBindViewObject::construct(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  return KJS::Object(new KstBindViewObject(exec));
}


struct ViewObjectBindings {
  const char *name;
  KJS::Value (KstBindViewObject::*method)(KJS::ExecState*, const KJS::List&);
};


struct ViewObjectProperties {
  const char *name;
  void (KstBindViewObject::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindViewObject::*get)(KJS::ExecState*) const;
};


static ViewObjectBindings viewObjectBindings[] = {
  { "findChild", &KstBindViewObject::findChild },
  { "resize", &KstBindViewObject::resize },
  { "move", &KstBindViewObject::move },
  { "convertTo", &KstBindViewObject::convertTo },
  { 0L, 0L }
};


static ViewObjectProperties viewObjectProperties[] = {
  { "size", &KstBindViewObject::setSize, &KstBindViewObject::size },
  { "position", &KstBindViewObject::setPosition, &KstBindViewObject::position },
  { "transparent", &KstBindViewObject::setTransparent, &KstBindViewObject::transparent },
  { "onGrid", &KstBindViewObject::setOnGrid, &KstBindViewObject::onGrid },
  { "columns", &KstBindViewObject::setColumns, &KstBindViewObject::columns },
  { "color", &KstBindViewObject::setColor, &KstBindViewObject::color },
  { "backgroundColor", &KstBindViewObject::setBackgroundColor, &KstBindViewObject::backgroundColor },
  { "children", 0L, &KstBindViewObject::children },
  { "minimumSize", 0L, &KstBindViewObject::minimumSize },
  { "type", 0L, &KstBindViewObject::type },
  { 0L, 0L, 0L }
};


int KstBindViewObject::methodCount() const {
  return sizeof viewObjectBindings + KstBindObject::methodCount();
}


int KstBindViewObject::propertyCount() const {
  return sizeof viewObjectProperties + KstBindObject::propertyCount();
}


KJS::ReferenceList KstBindViewObject::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindObject::propList(exec, recursive);

  for (int i = 0; viewObjectProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(viewObjectProperties[i].name)));
  }

  return rc;
}


bool KstBindViewObject::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; viewObjectProperties[i].name; ++i) {
    if (prop == viewObjectProperties[i].name) {
      return true;
    }
  }

  return KstBindObject::hasProperty(exec, propertyName);
}


void KstBindViewObject::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; viewObjectProperties[i].name; ++i) {
    if (prop == viewObjectProperties[i].name) {
      if (!viewObjectProperties[i].set) {
        break;
      }
      (this->*viewObjectProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindViewObject::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; viewObjectProperties[i].name; ++i) {
    if (prop == viewObjectProperties[i].name) {
      if (!viewObjectProperties[i].get) {
        break;
      }
      return (this->*viewObjectProperties[i].get)(exec);
    }
  }
  
  return KstBindObject::get(exec, propertyName);
}


KJS::Value KstBindViewObject::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindObject::methodCount();
  if (id > start) {
    KstBindViewObject *imp = dynamic_cast<KstBindViewObject*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*viewObjectBindings[id - start - 1].method)(exec, args);
  } 

  return KstBindObject::call(exec, self, args);
}


void KstBindViewObject::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindObject::methodCount();
  for (int i = 0; viewObjectBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindViewObject(i + start + 1));
    obj.put(exec, viewObjectBindings[i].name, o, KJS::Function);
  }
}


#define makeViewObject(X) dynamic_cast<KstViewObject*>(const_cast<KstObject*>(X.data()))

void KstBindViewObject::setColor(KJS::ExecState *exec, const KJS::Value& value) {
  QVariant cv = KJSEmbed::convertToVariant(exec, value);
  if (!cv.canCast(QVariant::Color)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstWriteLocker rl(d);
    d->setForegroundColor(cv.toColor());
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindViewObject::color(KJS::ExecState *exec) const {
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJSEmbed::convertToValue(exec, d->foregroundColor());
  }
  return KJSEmbed::convertToValue(exec, QColor());
}


void KstBindViewObject::setBackgroundColor(KJS::ExecState *exec, const KJS::Value& value) {
  QVariant cv = KJSEmbed::convertToVariant(exec, value);
  if (!cv.canCast(QVariant::Color)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstWriteLocker rl(d);
    d->setBackgroundColor(cv.toColor());
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindViewObject::backgroundColor(KJS::ExecState *exec) const {
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJSEmbed::convertToValue(exec, d->backgroundColor());
  }
  return KJSEmbed::convertToValue(exec, QColor());
}


void KstBindViewObject::setMaximized(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setMaximized(value.toBoolean(exec));
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindViewObject::maximized(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Boolean(d->maximized());
  }
  return KJS::Boolean(false);
}


void KstBindViewObject::setSize(KJS::ExecState *exec, const KJS::Value& value) {
  KstBindSize *imp = 0L;
  if (value.type() != KJS::ObjectType || !(imp = dynamic_cast<KstBindSize*>(value.toObject(exec).imp()))) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->resize(imp->_sz);
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindViewObject::size(KJS::ExecState *exec) const {
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Object(new KstBindSize(exec, d->size()));
  }
  return KJS::Null();
}


void KstBindViewObject::setTransparent(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setTransparent(value.toBoolean(exec));
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindViewObject::transparent(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Boolean(d->transparent());
  }
  return KJS::Boolean(false);
}


void KstBindViewObject::setOnGrid(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setOnGrid(value.toBoolean(exec));
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindViewObject::onGrid(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Boolean(d->onGrid());
  }
  return KJS::Boolean(false);
}


void KstBindViewObject::setColumns(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned cols = 1;
  if (value.type() != KJS::NumberType || !value.toUInt32(cols)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setOnGrid(true);
    d->setColumns(cols);
    d->cleanup(cols);
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindViewObject::columns(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->columns());
  }
  return KJS::Number(0);
}


void KstBindViewObject::setPosition(KJS::ExecState *exec, const KJS::Value& value) {
  KstBindPoint *imp = 0L;
  if (value.type() != KJS::ObjectType || !(imp = dynamic_cast<KstBindPoint*>(value.toObject(exec).imp()))) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->move(QPoint(int(imp->_x), int(imp->_y)));
    KstApp::inst()->paintAll(KstPainter::P_PAINT);
  }
}


KJS::Value KstBindViewObject::position(KJS::ExecState *exec) const {
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Object(new KstBindPoint(exec, d->position().x(), d->position().y()));
  }
  return KJS::Null();
}


KJS::Value KstBindViewObject::children(KJS::ExecState *exec) const {
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Object(new KstBindViewObjectCollection(exec, d));
  }
  return KJS::Null();
}


KJS::Value KstBindViewObject::resize(KJS::ExecState *exec, const KJS::List& args) {
  unsigned x = 0, y = 0;
  if (args.size() == 1) {
    KstBindSize *imp = 0L;
    if (args[0].type() != KJS::ObjectType || !(imp = dynamic_cast<KstBindSize*>(args[0].toObject(exec).imp()))) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Undefined();
    }
    x = unsigned(imp->_sz.width());
    y = unsigned(imp->_sz.height());
  } else if (args.size() == 2) {
    if (args[0].type() != KJS::NumberType || !args[0].toUInt32(x) ||
      args[1].type() != KJS::NumberType || !args[1].toUInt32(y)) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Undefined();
    }
  } else {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstWriteLocker rl(d);
    d->resize(QSize(x, y));
  }
  
  return KJS::Undefined();
}


KJS::Value KstBindViewObject::move(KJS::ExecState *exec, const KJS::List& args) {
  unsigned x = 0, y = 0;
  if (args.size() == 1) {
    KstBindPoint *imp = 0L;
    if (args[0].type() != KJS::ObjectType || !(imp = dynamic_cast<KstBindPoint*>(args[0].toObject(exec).imp()))) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Undefined();
    }
    x = unsigned(imp->_x);
    y = unsigned(imp->_y);
  } else if (args.size() == 2) {
    if (args[0].type() != KJS::NumberType || !args[0].toUInt32(x) ||
      args[1].type() != KJS::NumberType || !args[1].toUInt32(y)) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Undefined();
    }
  } else {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstWriteLocker rl(d);
    d->move(QPoint(x, y));
  }
  
  return KJS::Undefined();
}


KJS::Value KstBindViewObject::convertTo(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly one argument.");
    exec->setException(eobj);
    return KJS::Null();
  }

  if (args[0].type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Null();
  }

  QString type = args[0].toString(exec).qstring();
  if (type == "ViewObject") {
    return KJS::Object(new KstBindViewObject(exec, kst_cast<KstViewObject>(_d)));
  }
  
  if (_factoryMap.contains(type)) {
    KstBindViewObject *o = (_factoryMap[type])(exec, kst_cast<KstViewObject>(_d));
    if (o) {
      return KJS::Object(o);
    }
  }

  return KJS::Null();
}


KJS::Value KstBindViewObject::findChild(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly one argument.");
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindPoint *imp = 0L;
  if (args[0].type() != KJS::ObjectType || !(imp = dynamic_cast<KstBindPoint*>(args[0].toObject(exec).imp()))) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    KstViewObjectPtr vop = d->findChild(QPoint(int(imp->_x), int(imp->_y)));
    if (vop) {
      return KJS::Object(KstBindViewObject::bind(exec, vop));
    }
  }
  
  return KJS::Null();
}


KJS::Value KstBindViewObject::minimumSize(KJS::ExecState *exec) const {
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Object(new KstBindSize(exec, d->minimumSize()));
  }
  return KJS::Null();
}


KJS::Value KstBindViewObject::type(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstViewObjectPtr d = makeViewObject(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::String(d->type());
  }
  return KJS::String();
}


KstBindViewObject *KstBindViewObject::bind(KJS::ExecState *exec, KstViewObjectPtr obj) {
  if (!obj) {
    return 0L;
  }
  
  if (_factoryMap.contains(obj->type())) {
    KstBindViewObject *o = (_factoryMap[obj->type()])(exec, obj);
    if (o) {
      return o;
    }
  }
  return new KstBindViewObject(exec, obj);
}


void KstBindViewObject::addFactory(const QString& typeName, KstBindViewObject*(*factory)(KJS::ExecState*, KstViewObjectPtr)) {
  _factoryMap[typeName] = factory;
}


QMap<QString, KstBindViewObject*(*)(KJS::ExecState*, KstViewObjectPtr)> KstBindViewObject::_factoryMap;

#undef makeViewObject


// vim: ts=2 sw=2 et
