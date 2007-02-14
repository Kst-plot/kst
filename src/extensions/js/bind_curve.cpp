/***************************************************************************
                               bind_curve.cpp
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

#include "bind_curve.h"
#include "bind_point.h"
#include "bind_vector.h"

#include <kstcolorsequence.h>
#include <kstdataobjectcollection.h>

#include <kdebug.h>
#include <kjsembed/jsbinding.h>

KstBindCurve::KstBindCurve(KJS::ExecState *exec, KstVCurvePtr d)
: KstBindDataObject(exec, d.data(), "Curve") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindCurve::KstBindCurve(KJS::ExecState *exec, KJS::Object *globalObject)
: KstBindDataObject(exec, globalObject, "Curve") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    KstBindDataObject::addFactory("Curve", KstBindCurve::bindFactory);
  }
}


KstBindDataObject *KstBindCurve::bindFactory(KJS::ExecState *exec, KstDataObjectPtr obj) {
  KstVCurvePtr v = kst_cast<KstVCurve>(obj);
  if (v) {
    return new KstBindCurve(exec, v);
  }
  return 0L;
}


KstBindCurve::KstBindCurve(int id)
: KstBindDataObject(id, "Curve Method") {
}


KstBindCurve::~KstBindCurve() {
}


KJS::Object KstBindCurve::construct(KJS::ExecState *exec, const KJS::List& args) {
  KstVectorPtr x, y, ex, ey, exm, eym;
  if (args.size() > 0) {
    x = extractVector(exec, args[0]);
    if (!x) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }
  }

  if (args.size() > 1) {
    y = extractVector(exec, args[1]);
    if (!y) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }
  }

  if (args.size() > 2) {
    ex = extractVector(exec, args[2]);
    if (!ex) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }
  }

  if (args.size() > 3) {
    ey = extractVector(exec, args[3]);
    if (!ey) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }
  }

  if (args.size() > 4) {
    exm = extractVector(exec, args[4]);
    if (!exm) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }
  }

  if (args.size() > 5) {
    eym = extractVector(exec, args[5]);
    if (!eym) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }
  }

  if (args.size() > 6) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Object();
  }

  if (!x || !y) { // force at least X and Y vectors
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Object();
  }

  QColor color = KstColorSequence::next();
  KstVCurvePtr d = new KstVCurve(QString::null, x, y, ex, ey, exm, eym, color);

  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(d.data());
  KST::dataObjectList.lock().unlock();

  return KJS::Object(new KstBindCurve(exec, d));
}


struct CurveBindings {
  const char *name;
  KJS::Value (KstBindCurve::*method)(KJS::ExecState*, const KJS::List&);
};


struct CurveProperties {
  const char *name;
  void (KstBindCurve::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindCurve::*get)(KJS::ExecState*) const;
};


static CurveBindings curveBindings[] = {
  { "point", &KstBindCurve::point },
  { "xErrorPoint", &KstBindCurve::xErrorPoint },
  { "yErrorPoint", &KstBindCurve::yErrorPoint },
  { "xMinusErrorPoint", &KstBindCurve::xMinusErrorPoint },
  { "yMinusErrorPoint", &KstBindCurve::yMinusErrorPoint },
  { 0L, 0L }
};


static CurveProperties curveProperties[] = {
  { "color", &KstBindCurve::setColor, &KstBindCurve::color },
  { "xVector", &KstBindCurve::setXVector, &KstBindCurve::xVector },
  { "yVector", &KstBindCurve::setYVector, &KstBindCurve::yVector },
  { "xErrorVector", &KstBindCurve::setXErrorVector, &KstBindCurve::xErrorVector },
  { "yErrorVector", &KstBindCurve::setYErrorVector, &KstBindCurve::yErrorVector },
  { "xMinusErrorVector", &KstBindCurve::setXMinusErrorVector, &KstBindCurve::xMinusErrorVector },
  { "yMinusErrorVector", &KstBindCurve::setYMinusErrorVector, &KstBindCurve::yMinusErrorVector },
  { "samplesPerFrame", 0L, &KstBindCurve::samplesPerFrame },
  { "ignoreAutoScale", &KstBindCurve::setIgnoreAutoScale, &KstBindCurve::ignoreAutoScale },
  { "hasPoints", &KstBindCurve::setHasPoints, &KstBindCurve::hasPoints },
  { "hasLines", &KstBindCurve::setHasLines, &KstBindCurve::hasLines },
  { "hasBars", &KstBindCurve::setHasBars, &KstBindCurve::hasBars },
  { "lineWidth", &KstBindCurve::setLineWidth, &KstBindCurve::lineWidth },
  { "lineStyle", &KstBindCurve::setLineStyle, &KstBindCurve::lineStyle },
  { "barStyle", &KstBindCurve::setBarStyle, &KstBindCurve::barStyle },
  { "pointDensity", &KstBindCurve::setPointDensity, &KstBindCurve::pointDensity },
  { "topLabel", 0L, &KstBindCurve::topLabel },
  { "xLabel", 0L, &KstBindCurve::xLabel },
  { "yLabel", 0L, &KstBindCurve::yLabel },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindCurve::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindDataObject::propList(exec, recursive);

  for (int i = 0; curveProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(curveProperties[i].name)));
  }

  return rc;
}


bool KstBindCurve::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; curveProperties[i].name; ++i) {
    if (prop == curveProperties[i].name) {
      return true;
    }
  }

  return KstBindDataObject::hasProperty(exec, propertyName);
}


void KstBindCurve::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindDataObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; curveProperties[i].name; ++i) {
    if (prop == curveProperties[i].name) {
      if (!curveProperties[i].set) {
        break;
      }
      (this->*curveProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindDataObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindCurve::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindDataObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; curveProperties[i].name; ++i) {
    if (prop == curveProperties[i].name) {
      if (!curveProperties[i].get) {
        break;
      }
      return (this->*curveProperties[i].get)(exec);
    }
  }
  
  return KstBindDataObject::get(exec, propertyName);
}


KJS::Value KstBindCurve::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindDataObject::methodCount();
  if (id > start) {
    KstBindCurve *imp = dynamic_cast<KstBindCurve*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*curveBindings[id - start - 1].method)(exec, args);
  }

  return KstBindDataObject::call(exec, self, args);
}


void KstBindCurve::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindDataObject::methodCount();
  for (int i = 0; curveBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindCurve(i + start + 1));
    obj.put(exec, curveBindings[i].name, o, KJS::Function);
  }
}


#define makeCurve(X) dynamic_cast<KstVCurve*>(const_cast<KstObject*>(X.data()))

KJS::Value KstBindCurve::point(KJS::ExecState *exec, const KJS::List& args) {
  KstVCurvePtr d = makeCurve(_d);
  if (!d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly one argument.");
    exec->setException(eobj);
    return KJS::Undefined();
  }

  unsigned i = 0;
  if (args[0].type() != KJS::NumberType || !args[0].toUInt32(i)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstReadLocker rl(d);
  double x, y;
  d->point(i, x, y);
  return KJS::Object(new KstBindPoint(exec, x, y));
}


KJS::Value KstBindCurve::xErrorPoint(KJS::ExecState *exec, const KJS::List& args) {
  KstVCurvePtr d = makeCurve(_d);
  if (!d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly one argument.");
    exec->setException(eobj);
    return KJS::Undefined();
  }

  unsigned i = 0;
  if (args[0].type() != KJS::NumberType || !args[0].toUInt32(i)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstReadLocker rl(d);
  double x, y, e;
  d->getEXPoint(i, x, y, e);
  return KJS::Number(e);
}


KJS::Value KstBindCurve::yErrorPoint(KJS::ExecState *exec, const KJS::List& args) {
  KstVCurvePtr d = makeCurve(_d);
  if (!d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly one argument.");
    exec->setException(eobj);
    return KJS::Undefined();
  }

  unsigned i = 0;
  if (args[0].type() != KJS::NumberType || !args[0].toUInt32(i)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstReadLocker rl(d);
  double x, y, e;
  d->getEYPoint(i, x, y, e);
  return KJS::Number(e);
}


KJS::Value KstBindCurve::xMinusErrorPoint(KJS::ExecState *exec, const KJS::List& args) {
  KstVCurvePtr d = makeCurve(_d);
  if (!d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly one argument.");
    exec->setException(eobj);
    return KJS::Undefined();
  }

  unsigned i = 0;
  if (args[0].type() != KJS::NumberType || !args[0].toUInt32(i)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstReadLocker rl(d);
  double x, y, e;
  d->getEXMinusPoint(i, x, y, e);
  return KJS::Number(e);
}


KJS::Value KstBindCurve::yMinusErrorPoint(KJS::ExecState *exec, const KJS::List& args) {
  KstVCurvePtr d = makeCurve(_d);
  if (!d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly one argument.");
    exec->setException(eobj);
    return KJS::Undefined();
  }

  unsigned i = 0;
  if (args[0].type() != KJS::NumberType || !args[0].toUInt32(i)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstReadLocker rl(d);
  double x, y, e;
  d->getEYMinusPoint(i, x, y, e);
  return KJS::Number(e);
}


void KstBindCurve::setColor(KJS::ExecState *exec, const KJS::Value& value) {
  QVariant cv = KJSEmbed::convertToVariant(exec, value);
  if (!cv.canCast(QVariant::Color)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstWriteLocker rl(d);
    d->setColor(cv.toColor());
  }
}


KJS::Value KstBindCurve::color(KJS::ExecState *exec) const {
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJSEmbed::convertToValue(exec, d->color());
  }

  return KJSEmbed::convertToValue(exec, QColor());
}


void KstBindCurve::setXVector(KJS::ExecState *exec, const KJS::Value& value) {
  KstVectorPtr vp = extractVector(exec, value);
  if (vp) {
    KstVCurvePtr d = makeCurve(_d);
    if (d) {
      KstWriteLocker wl(d);
      d->setXVector(vp);
    }
  }
}


KJS::Value KstBindCurve::xVector(KJS::ExecState *exec) const {
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    KstVectorPtr vp = d->xVector();
    if (vp) {
      return KJS::Object(new KstBindVector(exec, vp));
    }
  }
  return KJS::Null();
}


void KstBindCurve::setYVector(KJS::ExecState *exec, const KJS::Value& value) {
  KstVectorPtr vp = extractVector(exec, value);
  if (vp) {
    KstVCurvePtr d = makeCurve(_d);
    if (d) {
      KstWriteLocker wl(d);
      d->setYVector(vp);
    }
  }
}


KJS::Value KstBindCurve::yVector(KJS::ExecState *exec) const {
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    KstVectorPtr vp = d->yVector();
    if (!vp) {
      return KJS::Object(new KstBindVector(exec, vp));
    }
  }
  return KJS::Null();
}


void KstBindCurve::setXErrorVector(KJS::ExecState *exec, const KJS::Value& value) {
  KstVectorPtr vp = extractVector(exec, value);
  if (vp) {
    KstVCurvePtr d = makeCurve(_d);
    if (d) {
      KstWriteLocker wl(d);
      d->setXError(vp);
    }
  }
}


KJS::Value KstBindCurve::xErrorVector(KJS::ExecState *exec) const {
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    KstVectorPtr vp = d->xErrorVector();
    if (vp) {
      return KJS::Object(new KstBindVector(exec, vp));
    }
  }
  return KJS::Null();
}


void KstBindCurve::setYErrorVector(KJS::ExecState *exec, const KJS::Value& value) {
  KstVectorPtr vp = extractVector(exec, value);
  if (vp) {
    KstVCurvePtr d = makeCurve(_d);
    if (d) {
      KstWriteLocker wl(d);
      d->setYError(vp);
    }
  }
}


KJS::Value KstBindCurve::yErrorVector(KJS::ExecState *exec) const {
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    KstVectorPtr vp = d->yErrorVector();
    if (vp) {
      return KJS::Object(new KstBindVector(exec, vp));
    }
  }
  return KJS::Null();
}


void KstBindCurve::setXMinusErrorVector(KJS::ExecState *exec, const KJS::Value& value) {
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstVectorPtr vp = extractVector(exec, value);
    if (vp) {
      KstWriteLocker wl(d);
      d->setXMinusError(vp);
    }
  }
}


KJS::Value KstBindCurve::xMinusErrorVector(KJS::ExecState *exec) const {
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    KstVectorPtr vp = d->xMinusErrorVector();
    if (vp) {
      return KJS::Object(new KstBindVector(exec, vp));
    }
  }
  return KJS::Null();
}


void KstBindCurve::setYMinusErrorVector(KJS::ExecState *exec, const KJS::Value& value) {
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstVectorPtr vp = extractVector(exec, value);
    if (vp) {
      KstWriteLocker wl(d);
      d->setYMinusError(vp);
    }
  }
}


KJS::Value KstBindCurve::yMinusErrorVector(KJS::ExecState *exec) const {
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    KstVectorPtr vp = d->yMinusErrorVector();
    if (vp) {
      return KJS::Object(new KstBindVector(exec, vp));
    }
  }
  return KJS::Null();
}


KJS::Value KstBindCurve::samplesPerFrame(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->samplesPerFrame());
  }
  return KJS::Number(0);
}


void KstBindCurve::setIgnoreAutoScale(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setIgnoreAutoScale(value.toBoolean(exec));
  }
}


KJS::Value KstBindCurve::ignoreAutoScale(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Boolean(d->ignoreAutoScale());
  }
  return KJS::Boolean(false);
}


void KstBindCurve::setHasPoints(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setHasPoints(value.toBoolean(exec));
  }
}


KJS::Value KstBindCurve::hasPoints(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Boolean(d->hasPoints());
  }
  return KJS::Boolean(false);
}


void KstBindCurve::setHasLines(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setHasLines(value.toBoolean(exec));
  }
}


KJS::Value KstBindCurve::hasLines(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Boolean(d->hasLines());
  }
  return KJS::Boolean(false);
}


void KstBindCurve::setHasBars(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setHasBars(value.toBoolean(exec));
  }
}


KJS::Value KstBindCurve::hasBars(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Boolean(d->hasBars());
  }
  return KJS::Boolean(false);
}


void KstBindCurve::setLineWidth(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned i = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(i)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setLineWidth(i);
  }
}


KJS::Value KstBindCurve::lineWidth(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->lineWidth());
  }
  return KJS::Number(0);
}


void KstBindCurve::setPointDensity(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned i = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(i)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setPointDensity(i);
  }
}


KJS::Value KstBindCurve::pointDensity(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->pointDensity());
  }
  return KJS::Number(0);
}


void KstBindCurve::setLineStyle(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned i = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(i)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setLineStyle(i);
  }
}


KJS::Value KstBindCurve::lineStyle(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->lineStyle());
  }
  return KJS::Number(0);
}


void KstBindCurve::setBarStyle(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned i = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(i)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setBarStyle(i);
  }
}


KJS::Value KstBindCurve::barStyle(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->barStyle());
  }
  return KJS::Number(0);
}


KJS::Value KstBindCurve::topLabel(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::String(d->topLabel());
  }
  return KJS::String();
}


KJS::Value KstBindCurve::xLabel(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::String(d->xLabel());
  }
  return KJS::String();
}


KJS::Value KstBindCurve::yLabel(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstVCurvePtr d = makeCurve(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::String(d->yLabel());
  }
  return KJS::String();
}


#undef makeCurve

// vim: ts=2 sw=2 et
