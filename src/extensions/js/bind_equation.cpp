/***************************************************************************
                              bind_equation.cpp
                             -------------------
    begin                : Mar 29 2005
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

#include "bind_equation.h"
#include "bind_datavector.h"
#include "bind_vector.h"

#include <kstdataobjectcollection.h>

#include <kdebug.h>

KstBindEquation::KstBindEquation(KJS::ExecState *exec, KstEquationPtr d)
: KstBindDataObject(exec, d.data(), "Equation") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindEquation::KstBindEquation(KJS::ExecState *exec, KJS::Object *globalObject)
: KstBindDataObject(exec, globalObject, "Equation") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    KstBindDataObject::addFactory("Equation", KstBindEquation::bindFactory);
  }
}


KstBindDataObject *KstBindEquation::bindFactory(KJS::ExecState *exec, KstDataObjectPtr obj) {
  KstEquationPtr v = kst_cast<KstEquation>(obj);
  if (v) {
    return new KstBindEquation(exec, v);
  }
  return 0L;
}


KstBindEquation::KstBindEquation(int id)
: KstBindDataObject(id, "Equation Method") {
}


KstBindEquation::~KstBindEquation() {
}


KJS::Object KstBindEquation::construct(KJS::ExecState *exec, const KJS::List& args) {
  KstEquationPtr d;

  // KstEquation(equation, xVector[, interp = true])
  if (args.size() == 2 || args.size() == 3) {
    if (args[0].type() != KJS::StringType ||
        (args.size() == 3 && args[2].type() != KJS::BooleanType)) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }

    KstVectorPtr vp = extractVector(exec, args[1]);

    if (!vp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }

    bool interp = true;

    QString eq = args[0].toString(exec).qstring();
    if (args.size() == 3) {
      interp = args[2].toBoolean(exec);
    }

    d = new KstEquation(QString::null, eq, vp, interp);
  }

  // KstEquation(equation, x0, x1, nx)
  if (args.size() == 4) {
    if (args[0].type() != KJS::StringType ||
        args[1].type() != KJS::NumberType ||
        args[2].type() != KJS::NumberType ||
        args[3].type() != KJS::NumberType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }

    unsigned npts;
    if (!args[3].toUInt32(npts)) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }

    d = new KstEquation(QString::null, args[0].toString(exec).qstring(), args[1].toNumber(exec), args[2].toNumber(exec), npts);
  }

  if (!d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Object();
  }

  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(d.data());
  KST::dataObjectList.lock().unlock();

  return KJS::Object(new KstBindEquation(exec, d));
}


struct EquationBindings {
  const char *name;
  KJS::Value (KstBindEquation::*method)(KJS::ExecState*, const KJS::List&);
};


struct EquationProperties {
  const char *name;
  void (KstBindEquation::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindEquation::*get)(KJS::ExecState*) const;
};


static EquationBindings equationBindings[] = {
  { 0L, 0L }
};


static EquationProperties equationProperties[] = {
  { "equation", &KstBindEquation::setEquation, &KstBindEquation::equation },
  { "valid", 0L, &KstBindEquation::valid },
  { "xVector", 0L, &KstBindEquation::xVector },
  { "yVector", 0L, &KstBindEquation::yVector },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindEquation::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindDataObject::propList(exec, recursive);

  for (int i = 0; equationProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(equationProperties[i].name)));
  }

  return rc;
}


bool KstBindEquation::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; equationProperties[i].name; ++i) {
    if (prop == equationProperties[i].name) {
      return true;
    }
  }

  return KstBindDataObject::hasProperty(exec, propertyName);
}


void KstBindEquation::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindDataObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; equationProperties[i].name; ++i) {
    if (prop == equationProperties[i].name) {
      if (!equationProperties[i].set) {
        break;
      }
      (this->*equationProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindDataObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindEquation::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindDataObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; equationProperties[i].name; ++i) {
    if (prop == equationProperties[i].name) {
      if (!equationProperties[i].get) {
        break;
      }
      return (this->*equationProperties[i].get)(exec);
    }
  }
  
  return KstBindDataObject::get(exec, propertyName);
}


KJS::Value KstBindEquation::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindDataObject::methodCount();
  if (id > start) {
    KstBindEquation *imp = dynamic_cast<KstBindEquation*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*equationBindings[id - start - 1].method)(exec, args);
  }

  return KstBindDataObject::call(exec, self, args);
}


void KstBindEquation::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindDataObject::methodCount();
  for (int i = 0; equationBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindEquation(i + start + 1));
    obj.put(exec, equationBindings[i].name, o, KJS::Function);
  }
}


#define makeEquation(X) dynamic_cast<KstEquation*>(const_cast<KstObject*>(X.data()))

void KstBindEquation::setEquation(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::StringType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstEquationPtr d = makeEquation(_d);
  KstWriteLocker wl(d);
  d->setEquation(value.toString(exec).qstring());
}


KJS::Value KstBindEquation::equation(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstEquationPtr d = makeEquation(_d);
  KstReadLocker rl(d);
  return KJS::String(d->equation());
}


KJS::Value KstBindEquation::valid(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstEquationPtr d = makeEquation(_d);
  KstReadLocker rl(d);
  return KJS::Boolean(d->isValid());
}


KJS::Value KstBindEquation::xVector(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstEquationPtr d = makeEquation(_d);
  KstReadLocker rl(d);
  KstVectorPtr vp = d->vX();
  if (!vp) {
    return KJS::Undefined();
  }

  return KJS::Object(new KstBindVector(exec, vp));
}


KJS::Value KstBindEquation::yVector(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstEquationPtr d = makeEquation(_d);
  KstReadLocker rl(d);
  KstVectorPtr vp = d->vY();
  if (!vp) {
    return KJS::Undefined();
  }

  return KJS::Object(new KstBindVector(exec, vp));
}


#undef makeEquation

// vim: ts=2 sw=2 et
