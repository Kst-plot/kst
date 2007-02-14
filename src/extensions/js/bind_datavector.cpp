/***************************************************************************
                             bind_datavector.cpp
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

#include "bind_datavector.h"
#include "bind_datasource.h"

#include <kstdatasource.h>
#include <kstdatacollection.h>

#include <kdebug.h>

KstBindDataVector::KstBindDataVector(KJS::ExecState *exec, KstRVectorPtr v)
: KstBindVector(exec, v.data(), "DataVector") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindDataVector::KstBindDataVector(KJS::ExecState *exec, KJS::Object *globalObject)
: KstBindVector(exec, globalObject, "DataVector") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindDataVector::KstBindDataVector(int id)
: KstBindVector(id, "DataVector Method") {
}


KstBindDataVector::~KstBindDataVector() {
}


inline int d2i(double x) {
    return int(floor(x+0.5));
}


KJS::Object KstBindDataVector::construct(KJS::ExecState *exec, const KJS::List& args) {
  KstRVectorPtr v;

  KstDataSourcePtr dp = extractDataSource(exec, args[0]);
  // Constructor: DataVector(DataSource, field)
  if (args.size() == 2) {
    if (args[1].type() != KJS::StringType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }

    QString field = args[1].toString(exec).qstring();

    v = new KstRVector(dp, field, KstObjectTag::invalidTag, 0, -1, -1, false, false);
  }

  // Constructor: DataVector(DataSource, field, start, n)
  if (args.size() == 4) {
    if (args[1].type() != KJS::StringType ||
        args[2].type() != KJS::NumberType ||
        args[3].type() != KJS::NumberType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }

    QString field = args[1].toString(exec).qstring();
    int start = d2i(args[2].toNumber(exec));
    int count = d2i(args[3].toNumber(exec));

    v = new KstRVector(dp, field, KstObjectTag::invalidTag, start, count, -1, false, false);
  }

  // Constructor: DataVector(DataSource, field, start, n, skip)
  if (args.size() == 5) {
    if (args[1].type() != KJS::StringType ||
        args[2].type() != KJS::NumberType ||
        args[3].type() != KJS::NumberType ||
        args[4].type() != KJS::NumberType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }

    QString field = args[1].toString(exec).qstring();
    int start = d2i(args[2].toNumber(exec));
    int count = d2i(args[3].toNumber(exec));
    int skip = d2i(args[4].toNumber(exec));

    v = new KstRVector(dp, field, KstObjectTag::invalidTag, start, count, skip, true, false);
  }


  // Constructor: DataVector(DataSource, field, start, n, skip, ave)
  if (args.size() == 6) {
    if (args[1].type() != KJS::StringType ||
        args[2].type() != KJS::NumberType ||
        args[3].type() != KJS::NumberType ||
        args[4].type() != KJS::NumberType ||
        args[5].type() != KJS::BooleanType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }

    QString field = args[1].toString(exec).qstring();
    int start = d2i(args[2].toNumber(exec));
    int count = d2i(args[3].toNumber(exec));
    int skip = d2i(args[4].toNumber(exec));
    bool ave = d2i(args[5].toBoolean(exec));

    v = new KstRVector(dp, field, KstObjectTag::invalidTag, start, count, skip, true, ave);
  }

  if (!v) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Object();
  }

  return KJS::Object(new KstBindDataVector(exec, v));
}


struct DataVectorBindings {
  const char *name;
  KJS::Value (KstBindDataVector::*method)(KJS::ExecState*, const KJS::List&);
};


struct DataVectorProperties {
  const char *name;
  void (KstBindDataVector::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindDataVector::*get)(KJS::ExecState*) const;
};


static DataVectorBindings dataVectorBindings[] = {
  { "changeFile", &KstBindDataVector::changeFile },
  { "changeFrames", &KstBindDataVector::changeFrames },
  { "reload", &KstBindDataVector::reload },
  { 0L, 0L }
};


static DataVectorProperties dataVectorProperties[] = {
  { "valid", 0L, &KstBindDataVector::valid },
  { "skip", 0L, &KstBindDataVector::skip },
  { "boxcar", 0L, &KstBindDataVector::boxcar },
  { "readToEnd", 0L, &KstBindDataVector::readToEnd },
  { "countFromEnd", 0L, &KstBindDataVector::countFromEnd },
  { "skipLength", 0L, &KstBindDataVector::skipLength },
  { "startFrame", 0L, &KstBindDataVector::startFrame },
  { "startFrameRequested", 0L, &KstBindDataVector::startFrameRequested },
  { "frames", 0L, &KstBindDataVector::frames },
  { "framesRequested", 0L, &KstBindDataVector::framesRequested },
  { "samplesPerFrame", 0L, &KstBindDataVector::samplesPerFrame },
  { "field", 0L, &KstBindDataVector::field },
  { "dataSource", 0L, &KstBindDataVector::dataSource },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindDataVector::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindVector::propList(exec, recursive);

  for (int i = 0; dataVectorProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(dataVectorProperties[i].name)));
  }

  return rc;
}


bool KstBindDataVector::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; dataVectorProperties[i].name; ++i) {
    if (prop == dataVectorProperties[i].name) {
      return true;
    }
  }

  return KstBindVector::hasProperty(exec, propertyName);
}


void KstBindDataVector::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindVector::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();

  for (int i = 0; dataVectorProperties[i].name; ++i) {
    if (prop == dataVectorProperties[i].name) {
      if (!dataVectorProperties[i].set) {
        break;
      }
      (this->*dataVectorProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindVector::put(exec, propertyName, value, attr);
}


KJS::Value KstBindDataVector::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindVector::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; dataVectorProperties[i].name; ++i) {
    if (prop == dataVectorProperties[i].name) {
      if (!dataVectorProperties[i].get) {
        break;
      }
      return (this->*dataVectorProperties[i].get)(exec);
    }
  }

  return KstBindVector::get(exec, propertyName);
}


KJS::Value KstBindDataVector::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindVector::methodCount();
  if (id > start) {
    KstBindDataVector *imp = dynamic_cast<KstBindDataVector*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*dataVectorBindings[id - start - 1].method)(exec, args);
  }

  return KstBindVector::call(exec, self, args);
}


void KstBindDataVector::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindVector::methodCount();
  for (int i = 0; dataVectorBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindDataVector(i + start + 1));
    obj.put(exec, dataVectorBindings[i].name, o, KJS::Function);
  }
}


#define makeDataVector(X) dynamic_cast<KstRVector*>(const_cast<KstObject*>(X.data()))

KJS::Value KstBindDataVector::reload(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  KstRVectorPtr v = makeDataVector(_d);
  if (!v) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstWriteLocker wl(v);
  v->reload();
  return KJS::Undefined();
}


KJS::Value KstBindDataVector::changeFile(KJS::ExecState *exec, const KJS::List& args) {
  KstRVectorPtr v = makeDataVector(_d);
  if (!v) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args.size() != 1) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError, "Requires exactly one argument.");
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args[0].type() != KJS::ObjectType) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  KstBindDataSource *imp = dynamic_cast<KstBindDataSource*>(args[0].toObject(exec).imp());
  if (!imp) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

#define makeSource(X) dynamic_cast<KstDataSource*>(const_cast<KstObject*>(X.data()))
  KstDataSourcePtr s = makeSource(imp->_d);
  if (!s) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  v->writeLock();
  s->writeLock();
  v->changeFile(s);
  s->unlock();
  v->unlock();
#undef makeSource

  return KJS::Undefined();
}


KJS::Value KstBindDataVector::changeFrames(KJS::ExecState *exec, const KJS::List& args) {
  KstRVectorPtr v = makeDataVector(_d);
  if (!v) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args.size() > 1) {
    if (args[0].type() != KJS::NumberType ||
        args[1].type() != KJS::NumberType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    KstWriteLocker wl(v);
    int start = d2i(args[0].toNumber(exec));
    int count = d2i(args[1].toNumber(exec));
    int skip = v->doSkip() ? v->skip() : -1;
    bool ave = v->doAve();

    if (args.size() > 2) {
      if (args[2].type() != KJS::NumberType) {
        KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
        exec->setException(eobj);
        return KJS::Undefined();
      }

      skip = d2i(args[2].toNumber(exec));

      if (args.size() > 3) {
        if (args[3].type() != KJS::BooleanType) {
          KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
          exec->setException(eobj);
          return KJS::Undefined();
        }

        ave = d2i(args[3].toBoolean(exec));
      }
    }

    v->changeFrames(start, count, skip, skip >= 0, ave);
    return KJS::Undefined();
  }

  KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
  exec->setException(eobj);
  return KJS::Undefined();
}


KJS::Value KstBindDataVector::valid(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRVectorPtr v = makeDataVector(_d);
  KstReadLocker rl(v);
  return KJS::Boolean(v->isValid());
}


KJS::Value KstBindDataVector::skip(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRVectorPtr v = makeDataVector(_d);
  KstReadLocker rl(v);
  return KJS::Boolean(v->doSkip());
}


KJS::Value KstBindDataVector::boxcar(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRVectorPtr v = makeDataVector(_d);
  KstReadLocker rl(v);
  return KJS::Boolean(v->doAve());
}


KJS::Value KstBindDataVector::readToEnd(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRVectorPtr v = makeDataVector(_d);
  KstReadLocker rl(v);
  return KJS::Boolean(v->readToEOF());
}


KJS::Value KstBindDataVector::countFromEnd(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRVectorPtr v = makeDataVector(_d);
  KstReadLocker rl(v);
  return KJS::Boolean(v->countFromEOF());
}


KJS::Value KstBindDataVector::skipLength(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRVectorPtr v = makeDataVector(_d);
  KstReadLocker rl(v);
  return KJS::Number(v->skip());
}


KJS::Value KstBindDataVector::frames(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRVectorPtr v = makeDataVector(_d);
  KstReadLocker rl(v);
  return KJS::Number(v->numFrames());
}


KJS::Value KstBindDataVector::startFrame(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRVectorPtr v = makeDataVector(_d);
  KstReadLocker rl(v);
  return KJS::Number(v->startFrame());
}


KJS::Value KstBindDataVector::framesRequested(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRVectorPtr v = makeDataVector(_d);
  KstReadLocker rl(v);
  return KJS::Number(v->reqNumFrames());
}


KJS::Value KstBindDataVector::startFrameRequested(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRVectorPtr v = makeDataVector(_d);
  KstReadLocker rl(v);
  return KJS::Number(v->reqStartFrame());
}


KJS::Value KstBindDataVector::samplesPerFrame(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRVectorPtr v = makeDataVector(_d);
  KstReadLocker rl(v);
  return KJS::Number(v->samplesPerFrame());
}


KJS::Value KstBindDataVector::field(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRVectorPtr v = makeDataVector(_d);
  KstReadLocker rl(v);
  return KJS::String(v->field());
}


KJS::Value KstBindDataVector::dataSource(KJS::ExecState *exec) const {
  KstRVectorPtr v = makeDataVector(_d);
  KstReadLocker rl(v);
  KstDataSourcePtr dp = v->dataSource();
  if (!dp) {
    return KJS::Undefined();
  }

  return KJS::Object(new KstBindDataSource(exec, dp));
}

#undef makeDataVector

// vim: ts=2 sw=2 et
