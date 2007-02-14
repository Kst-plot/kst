/***************************************************************************
                               bind_matrix.cpp
                             -------------------
    begin                : Jul 29 2005
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

#include "bind_matrix.h"

#include <kstamatrix.h>
#include <kstdatacollection.h>

#include <kdebug.h>

KstBindMatrix::KstBindMatrix(KJS::ExecState *exec, KstMatrixPtr d, const char *name)
: KstBindObject(exec, d.data(), name ? name : "Matrix") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindMatrix::KstBindMatrix(KJS::ExecState *exec, KJS::Object *globalObject, const char *name)
: KstBindObject(exec, globalObject, name ? name : "Matrix") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (!globalObject) {
    KstAMatrixPtr m = new KstAMatrix(KstObjectTag::invalidTag, 1, 1, 0, 0, 1, 1);
    _d = m.data();
  }
}


KstBindMatrix::KstBindMatrix(int id, const char *name)
: KstBindObject(id, name ? name : "Matrix Method") {
}


KstBindMatrix::~KstBindMatrix() {
}


KJS::Object KstBindMatrix::construct(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  return KJS::Object(new KstBindMatrix(exec));
}


struct MatrixBindings {
  const char *name;
  KJS::Value (KstBindMatrix::*method)(KJS::ExecState*, const KJS::List&);
};


struct MatrixProperties {
  const char *name;
  void (KstBindMatrix::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindMatrix::*get)(KJS::ExecState*) const;
};


static MatrixBindings matrixBindings[] = {
  { "resize", &KstBindMatrix::resize },
  { "zero", &KstBindMatrix::zero },
  { 0L, 0L }
};


static MatrixProperties matrixProperties[] = {
  { "editable", 0L, &KstBindMatrix::editable },
  { "min", 0L, &KstBindMatrix::min },
  { "max", 0L, &KstBindMatrix::max },
  { "mean", 0L, &KstBindMatrix::mean },
  { "numNew", 0L, &KstBindMatrix::numNew },
  { "rows", 0L, &KstBindMatrix::rows },
  { "columns", 0L, &KstBindMatrix::columns },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindMatrix::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindObject::propList(exec, recursive);

  for (int i = 0; matrixProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(matrixProperties[i].name)));
  }

  return rc;
}


bool KstBindMatrix::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; matrixProperties[i].name; ++i) {
    if (prop == matrixProperties[i].name) {
      return true;
    }
  }

  return KstBindObject::hasProperty(exec, propertyName);
}


void KstBindMatrix::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; matrixProperties[i].name; ++i) {
    if (prop == matrixProperties[i].name) {
      if (!matrixProperties[i].set) {
        break;
      }
      (this->*matrixProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindMatrix::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; matrixProperties[i].name; ++i) {
    if (prop == matrixProperties[i].name) {
      if (!matrixProperties[i].get) {
        break;
      }
      return (this->*matrixProperties[i].get)(exec);
    }
  }
  
  return KstBindObject::get(exec, propertyName);
}


KJS::Value KstBindMatrix::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindMatrix *imp = dynamic_cast<KstBindMatrix*>(self.imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  return (imp->*matrixBindings[id - 1].method)(exec, args);
}


void KstBindMatrix::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  for (int i = 0; matrixBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindMatrix(i + 1));
    obj.put(exec, matrixBindings[i].name, o, KJS::Function);
  }
}

#define makeMatrix(X) dynamic_cast<KstMatrix*>(const_cast<KstObject*>(X.data()))

KJS::Value KstBindMatrix::editable(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstMatrixPtr m = makeMatrix(_d);
  KstReadLocker rl(m);
  return KJS::Number(m->editable());
}


KJS::Value KstBindMatrix::min(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstMatrixPtr m = makeMatrix(_d);
  KstReadLocker rl(m);
  return KJS::Number(m->minValue());
}


KJS::Value KstBindMatrix::max(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstMatrixPtr m = makeMatrix(_d);
  KstReadLocker rl(m);
  return KJS::Number(m->maxValue());
}


KJS::Value KstBindMatrix::mean(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstMatrixPtr m = makeMatrix(_d);
  KstReadLocker rl(m);
  return KJS::Number(m->meanValue());
}


KJS::Value KstBindMatrix::numNew(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstMatrixPtr m = makeMatrix(_d);
  KstReadLocker rl(m);
  return KJS::Number(m->numNew());
}


KJS::Value KstBindMatrix::rows(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstMatrixPtr m = makeMatrix(_d);
  KstReadLocker rl(m);
  return KJS::Number(m->yNumSteps());
}


KJS::Value KstBindMatrix::columns(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstMatrixPtr m = makeMatrix(_d);
  KstReadLocker rl(m);
  return KJS::Number(m->xNumSteps());
}


KJS::Value KstBindMatrix::resize(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() != 2) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly two arguments.");
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstMatrixPtr m = makeMatrix(_d);
  if (!m || !m->editable()) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  unsigned cols = 1, rows = 1;

  if (args[0].type() != KJS::NumberType || !args[0].toUInt32(cols)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args[1].type() != KJS::NumberType || !args[1].toUInt32(rows)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  m->writeLock();
  m->resize(rows, cols);
  m->unlock();

  return KJS::Undefined();
}


KJS::Value KstBindMatrix::zero(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  KstMatrixPtr m = makeMatrix(_d);
  if (!m || !m->editable()) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstWriteLocker wl(m);
  m->zero();
  return KJS::Undefined();
}


int KstBindMatrix::methodCount() const {
  return sizeof matrixBindings + KstBindObject::methodCount();
}


int KstBindMatrix::propertyCount() const {
  return sizeof matrixProperties + KstBindObject::propertyCount();
}

#undef makeMatrix

// vim: ts=2 sw=2 et
