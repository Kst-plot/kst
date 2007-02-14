/***************************************************************************
                               bind_point.cpp
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

#include "bind_point.h"

KstBindPoint::KstBindPoint(KJS::ExecState *exec, double x, double y)
: KstBinding("Point"), _x(x), _y(y) {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindPoint::KstBindPoint(KJS::ExecState *exec, KJS::Object *globalObject)
: KstBinding("Point"), _x(0), _y(0) {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    globalObject->put(exec, "Point", o);
  }
}


KstBindPoint::KstBindPoint(int id)
: KstBinding("Point Method", id), _x(0), _y(0) {
}


KstBindPoint::~KstBindPoint() {
}


KJS::Object KstBindPoint::construct(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() == 0) {
    return KJS::Object(new KstBindPoint(exec, 0, 0));
  }

  if (args.size() != 2) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Object();
  }

  if (args[0].type() != KJS::NumberType || args[1].type() != KJS::NumberType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Object();
  }

  return KJS::Object(new KstBindPoint(exec, args[0].toNumber(exec), args[1].toNumber(exec)));
}


struct PointBindings {
  const char *name;
  KJS::Value (KstBindPoint::*method)(KJS::ExecState*, const KJS::List&);
};


struct PointProperties {
  const char *name;
  void (KstBindPoint::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindPoint::*get)(KJS::ExecState*) const;
};


static PointBindings pointBindings[] = {
  { 0L, 0L }
};


static PointProperties pointProperties[] = {
  { "x", &KstBindPoint::setX, &KstBindPoint::x },
  { "y", &KstBindPoint::setY, &KstBindPoint::y },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindPoint::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBinding::propList(exec, recursive);

  for (int i = 0; pointProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(pointProperties[i].name)));
  }

  return rc;
}


bool KstBindPoint::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; pointProperties[i].name; ++i) {
    if (prop == pointProperties[i].name) {
      return true;
    }
  }

  return KstBinding::hasProperty(exec, propertyName);
}


void KstBindPoint::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  QString prop = propertyName.qstring();
  for (int i = 0; pointProperties[i].name; ++i) {
    if (prop == pointProperties[i].name) {
      if (!pointProperties[i].set) {
        break;
      }
      (this->*pointProperties[i].set)(exec, value);
      return;
    }
  }

  KstBinding::put(exec, propertyName, value, attr);
}


KJS::Value KstBindPoint::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; pointProperties[i].name; ++i) {
    if (prop == pointProperties[i].name) {
      if (!pointProperties[i].get) {
        break;
      }
      return (this->*pointProperties[i].get)(exec);
    }
  }
  
  return KstBinding::get(exec, propertyName);
}


KJS::Value KstBindPoint::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindPoint *imp = dynamic_cast<KstBindPoint*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*pointBindings[id - 1].method)(exec, args);
}


void KstBindPoint::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; pointBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindPoint(i + 1));
    obj.put(exec, pointBindings[i].name, o, KJS::Function);
  }
}


void KstBindPoint::setX(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::NumberType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  _x = value.toNumber(exec);
}


KJS::Value KstBindPoint::x(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Number(_x);
}


void KstBindPoint::setY(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::NumberType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  _y = value.toNumber(exec);
}


KJS::Value KstBindPoint::y(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Number(_y);
}


KJS::UString KstBindPoint::toString(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::UString(QString("(%1, %2)").arg(_x).arg(_y));
}

// vim: ts=2 sw=2 et
