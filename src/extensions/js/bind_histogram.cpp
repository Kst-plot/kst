/***************************************************************************
                              bind_histogram.cpp
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

#include "bind_histogram.h"
#include "bind_vector.h"

#include <kstdataobjectcollection.h>

#include <kdebug.h>

KstBindHistogram::KstBindHistogram(KJS::ExecState *exec, KstHistogramPtr d)
: KstBindDataObject(exec, d.data(), "Histogram") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindHistogram::KstBindHistogram(KJS::ExecState *exec, KJS::Object *globalObject)
: KstBindDataObject(exec, globalObject, "Histogram") {
  KJS::Object o(this);
  addBindings(exec, o);
  if (globalObject) {
    KstBindDataObject::addFactory("Histogram", KstBindHistogram::bindFactory);
  }
}


KstBindDataObject *KstBindHistogram::bindFactory(KJS::ExecState *exec, KstDataObjectPtr obj) {
  KstHistogramPtr v = kst_cast<KstHistogram>(obj);
  if (v) {
    return new KstBindHistogram(exec, v);
  }
  return 0L;
}


KstBindHistogram::KstBindHistogram(int id)
: KstBindDataObject(id, "Histogram Method") {
}


KstBindHistogram::~KstBindHistogram() {
}


KJS::Object KstBindHistogram::construct(KJS::ExecState *exec, const KJS::List& args) {
  if (args.size() < 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Object();
  }

  double xmin = -10.0;
  double xmax = 10.0;
  unsigned bins = 60;

  KstVectorPtr v = extractVector(exec, args[0]);
  
  if (!v) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Object();
  }

  if (args.size() == 2) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Object();
  }

  if (args.size() > 2) {
    if (args[1].type() != KJS::NumberType || args[2].type() != KJS::NumberType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }
    xmin = args[1].toNumber(exec);
    xmax = args[2].toNumber(exec);
  }

  if (args.size() > 3) {
    if (args[3].type() != KJS::NumberType || !args[3].toUInt32(bins)) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }
  }

  if (args.size() > 4) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Object();
  }

  KstHistogramPtr d = new KstHistogram(QString::null, v, xmin, xmax, bins, KST_HS_NUMBER);

  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(d.data());
  KST::dataObjectList.lock().unlock();

  return KJS::Object(new KstBindHistogram(exec, d));
}


struct HistogramBindings {
  const char *name;
  KJS::Value (KstBindHistogram::*method)(KJS::ExecState*, const KJS::List&);
};


struct HistogramProperties {
  const char *name;
  void (KstBindHistogram::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindHistogram::*get)(KJS::ExecState*) const;
};


static HistogramBindings histogramBindings[] = {
  { "setVector", &KstBindHistogram::setVector },
  { "setRange", &KstBindHistogram::setRange },
  { 0L, 0L }
};


static HistogramProperties histogramProperties[] = {
  { "realTimeAutoBin", &KstBindHistogram::setRealTimeAutoBin, &KstBindHistogram::realTimeAutoBin },
  { "bins", &KstBindHistogram::setBins, &KstBindHistogram::bins },
  { "xVector", 0L, &KstBindHistogram::xVector },
  { "yVector", 0L, &KstBindHistogram::yVector },
  { "xMin", 0L, &KstBindHistogram::xMin },
  { "xMax", 0L, &KstBindHistogram::xMax },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindHistogram::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindDataObject::propList(exec, recursive);

  for (int i = 0; histogramProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(histogramProperties[i].name)));
  }

  return rc;
}


bool KstBindHistogram::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; histogramProperties[i].name; ++i) {
    if (prop == histogramProperties[i].name) {
      return true;
    }
  }

  return KstBindDataObject::hasProperty(exec, propertyName);
}


void KstBindHistogram::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindDataObject::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();
  for (int i = 0; histogramProperties[i].name; ++i) {
    if (prop == histogramProperties[i].name) {
      if (!histogramProperties[i].set) {
        break;
      }
      (this->*histogramProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindDataObject::put(exec, propertyName, value, attr);
}


KJS::Value KstBindHistogram::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindDataObject::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; histogramProperties[i].name; ++i) {
    if (prop == histogramProperties[i].name) {
      if (!histogramProperties[i].get) {
        break;
      }
      return (this->*histogramProperties[i].get)(exec);
    }
  }
  
  return KstBindDataObject::get(exec, propertyName);
}


KJS::Value KstBindHistogram::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindDataObject::methodCount();
  if (id > start) {
    KstBindHistogram *imp = dynamic_cast<KstBindHistogram*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*histogramBindings[id - start - 1].method)(exec, args);
  }

  return KstBindDataObject::call(exec, self, args);
}


void KstBindHistogram::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindDataObject::methodCount();
  for (int i = 0; histogramBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindHistogram(i + start + 1));
    obj.put(exec, histogramBindings[i].name, o, KJS::Function);
  }
}


#define makeHistogram(X) dynamic_cast<KstHistogram*>(const_cast<KstObject*>(X.data()))

void KstBindHistogram::setRealTimeAutoBin(KJS::ExecState *exec, const KJS::Value& value) {
  if (value.type() != KJS::BooleanType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstHistogramPtr d = makeHistogram(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setRealTimeAutoBin(value.toBoolean(exec));
  }
}


KJS::Value KstBindHistogram::realTimeAutoBin(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstHistogramPtr d = makeHistogram(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Boolean(d->realTimeAutoBin());
  }
  return KJS::Boolean(false);
}


void KstBindHistogram::setBins(KJS::ExecState *exec, const KJS::Value& value) {
  unsigned i = 0;
  if (value.type() != KJS::NumberType || !value.toUInt32(i)) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return;
  }
  KstHistogramPtr d = makeHistogram(_d);
  if (d) {
    KstWriteLocker wl(d);
    d->setNBins(i);
  }
}


KJS::Value KstBindHistogram::bins(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstHistogramPtr d = makeHistogram(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->nBins());
  }
  return KJS::Number(0);
}


KJS::Value KstBindHistogram::xVector(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstHistogramPtr d = makeHistogram(_d);
  if (d) {
    KstReadLocker rl(d);
    KstVectorPtr vp = d->vX();
    if (vp) {
      return KJS::Object(new KstBindVector(exec, vp));
    }
  }
  return KJS::Null();
}


KJS::Value KstBindHistogram::yVector(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstHistogramPtr d = makeHistogram(_d);
  if (d) {
    KstReadLocker rl(d);
    KstVectorPtr vp = d->vY();
    if (vp) {
      return KJS::Object(new KstBindVector(exec, vp));
    }
  }
  return KJS::Null();
}


KJS::Value KstBindHistogram::setVector(KJS::ExecState *exec, const KJS::List& args) {
  KstHistogramPtr d = makeHistogram(_d);
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

  KstVectorPtr v = extractVector(exec, args[0]);
  if (v) {
    KstWriteLocker wl(d);
    d->setVector(v);
  }

  return KJS::Undefined();
}


KJS::Value KstBindHistogram::setRange(KJS::ExecState *exec, const KJS::List& args) {
  KstHistogramPtr d = makeHistogram(_d);
  if (!d) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args.size() != 2) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly two arguments.");
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args[0].type() != KJS::NumberType || args[1].type() != KJS::NumberType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  double x0 = args[0].toNumber(exec);
  double x1 = args[1].toNumber(exec);

  KstWriteLocker wl(d);
  d->setXRange(x0, x1);
  return KJS::Undefined();
}


KJS::Value KstBindHistogram::xMin(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstHistogramPtr d = makeHistogram(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->xMin());
  }
  return KJS::Number(0);
}


KJS::Value KstBindHistogram::xMax(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstHistogramPtr d = makeHistogram(_d);
  if (d) {
    KstReadLocker rl(d);
    return KJS::Number(d->xMax());
  }
  return KJS::Number(0);
}


#undef makeHistogram

// vim: ts=2 sw=2 et
