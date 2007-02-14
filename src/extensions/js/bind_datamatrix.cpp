/***************************************************************************
                             bind_datamatrix.cpp
                             -------------------
    begin                : Jul 27 2005
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

#include "bind_datamatrix.h"
#include "bind_datasource.h"

#include <kstdatasource.h>
#include <kstdatacollection.h>

#include <kdebug.h>

KstBindDataMatrix::KstBindDataMatrix(KJS::ExecState *exec, KstRMatrixPtr v)
: KstBindMatrix(exec, v.data(), "DataMatrix") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindDataMatrix::KstBindDataMatrix(KJS::ExecState *exec, KJS::Object *globalObject)
: KstBindMatrix(exec, globalObject, "DataMatrix") {
  KJS::Object o(this);
  addBindings(exec, o);
}


KstBindDataMatrix::KstBindDataMatrix(int id)
: KstBindMatrix(id, "DataMatrix Method") {
}


KstBindDataMatrix::~KstBindDataMatrix() {
}


inline int d2i(double x) {
    return int(floor(x+0.5));
}


KJS::Object KstBindDataMatrix::construct(KJS::ExecState *exec, const KJS::List& args) {
  KstRMatrixPtr v;

  KstDataSourcePtr dp = extractDataSource(exec, args[0]);
  // Constructor: DataMatrix(DataSource, field)
  if (args.size() == 2) {
    if (args[1].type() != KJS::StringType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }

    QString field = args[1].toString(exec).qstring();

    v = new KstRMatrix(dp, field, KstObjectTag::invalidTag, 0, 0, -1, -1, false, false, -1);
  }

  // Constructor: DataMatrix(DataSource, field, xStart, yStart, xCount, yCount)
  if (args.size() == 6) {
    if (args[1].type() != KJS::StringType ||
        args[2].type() != KJS::NumberType ||
        args[3].type() != KJS::NumberType ||
        args[4].type() != KJS::NumberType ||
        args[5].type() != KJS::NumberType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }

    QString field = args[1].toString(exec).qstring();
    int xStart = d2i(args[2].toNumber(exec));
    int yStart = d2i(args[3].toNumber(exec));
    int xCount = d2i(args[4].toNumber(exec));
    int yCount = d2i(args[5].toNumber(exec));

    v = new KstRMatrix(dp, field, KstObjectTag::invalidTag, xStart, yStart, xCount, yCount,false, false, -1);
  }

  // Constructor: DataMatrix(DataSource, field, xStart, yStart, xCount, yCount, skip)
  if (args.size() == 7) {
    if (args[1].type() != KJS::StringType ||
        args[2].type() != KJS::NumberType ||
        args[3].type() != KJS::NumberType ||
        args[4].type() != KJS::NumberType ||
        args[5].type() != KJS::NumberType ||
        args[6].type() != KJS::NumberType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }

    QString field = args[1].toString(exec).qstring();
    int xStart = d2i(args[2].toNumber(exec));
    int yStart = d2i(args[3].toNumber(exec));
    int xCount = d2i(args[4].toNumber(exec));
    int yCount = d2i(args[5].toNumber(exec));
    int skip = d2i(args[6].toNumber(exec));

    v = new KstRMatrix(dp, field, KstObjectTag::invalidTag, xStart, yStart, xCount, yCount, false, true, skip);
  }


  // Constructor: DataMatrix(DataSource, field, xStart, yStart, xCount, yCount, skip, ave)
  if (args.size() == 8) {
    if (args[1].type() != KJS::StringType ||
        args[2].type() != KJS::NumberType ||
        args[3].type() != KJS::NumberType ||
        args[4].type() != KJS::NumberType ||
        args[5].type() != KJS::NumberType ||
        args[6].type() != KJS::NumberType ||
        args[7].type() != KJS::BooleanType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Object();
    }

    QString field = args[1].toString(exec).qstring();
    int xStart = d2i(args[2].toNumber(exec));
    int yStart = d2i(args[3].toNumber(exec));
    int xCount = d2i(args[4].toNumber(exec));
    int yCount = d2i(args[5].toNumber(exec));
    int skip = d2i(args[6].toNumber(exec));
    bool ave = d2i(args[7].toBoolean(exec));

    v = new KstRMatrix(dp, field, KstObjectTag::invalidTag, xStart, yStart, xCount, yCount, ave, true, skip);
  }

  if (!v) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
    exec->setException(eobj);
    return KJS::Object();
  }

  return KJS::Object(new KstBindDataMatrix(exec, v));
}


struct DataMatrixBindings {
  const char *name;
  KJS::Value (KstBindDataMatrix::*method)(KJS::ExecState*, const KJS::List&);
};


struct DataMatrixProperties {
  const char *name;
  void (KstBindDataMatrix::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindDataMatrix::*get)(KJS::ExecState*) const;
};


static DataMatrixBindings dataMatrixBindings[] = {
  { "changeFile", &KstBindDataMatrix::changeFile },
  { "change", &KstBindDataMatrix::change },
  { "reload", &KstBindDataMatrix::reload },
  { 0L, 0L }
};


static DataMatrixProperties dataMatrixProperties[] = {
  { "valid", 0L, &KstBindDataMatrix::valid },
  { "skip", 0L, &KstBindDataMatrix::skip },
  { "boxcar", 0L, &KstBindDataMatrix::boxcar },
  { "xReadToEnd", 0L, &KstBindDataMatrix::xReadToEnd },
  { "yReadToEnd", 0L, &KstBindDataMatrix::yReadToEnd },
  { "xCountFromEnd", 0L, &KstBindDataMatrix::xCountFromEnd },
  { "yCountFromEnd", 0L, &KstBindDataMatrix::yCountFromEnd },
  { "skipLength", 0L, &KstBindDataMatrix::skipLength },
  { "field", 0L, &KstBindDataMatrix::field },
  { "dataSource", 0L, &KstBindDataMatrix::dataSource },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindDataMatrix::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindMatrix::propList(exec, recursive);

  for (int i = 0; dataMatrixProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(dataMatrixProperties[i].name)));
  }

  return rc;
}


bool KstBindDataMatrix::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; dataMatrixProperties[i].name; ++i) {
    if (prop == dataMatrixProperties[i].name) {
      return true;
    }
  }

  return KstBindMatrix::hasProperty(exec, propertyName);
}


void KstBindDataMatrix::put(KJS::ExecState *exec, const KJS::Identifier& propertyName, const KJS::Value& value, int attr) {
  if (!_d) {
    KstBindMatrix::put(exec, propertyName, value, attr);
    return;
  }

  QString prop = propertyName.qstring();

  for (int i = 0; dataMatrixProperties[i].name; ++i) {
    if (prop == dataMatrixProperties[i].name) {
      if (!dataMatrixProperties[i].set) {
        break;
      }
      (this->*dataMatrixProperties[i].set)(exec, value);
      return;
    }
  }

  KstBindMatrix::put(exec, propertyName, value, attr);
}


KJS::Value KstBindDataMatrix::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (!_d) {
    return KstBindMatrix::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; dataMatrixProperties[i].name; ++i) {
    if (prop == dataMatrixProperties[i].name) {
      if (!dataMatrixProperties[i].get) {
        break;
      }
      return (this->*dataMatrixProperties[i].get)(exec);
    }
  }

  return KstBindMatrix::get(exec, propertyName);
}


KJS::Value KstBindDataMatrix::call(KJS::ExecState *exec, KJS::Object& self, const KJS::List& args) {
  int id = this->id();
  if (id <= 0) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  int start = KstBindMatrix::methodCount();
  if (id > start) {
    KstBindDataMatrix *imp = dynamic_cast<KstBindDataMatrix*>(self.imp());
    if (!imp) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    return (imp->*dataMatrixBindings[id - start - 1].method)(exec, args);
  }

  return KstBindMatrix::call(exec, self, args);
}


void KstBindDataMatrix::addBindings(KJS::ExecState *exec, KJS::Object& obj) {
  int start = KstBindMatrix::methodCount();
  for (int i = 0; dataMatrixBindings[i].name != 0L; ++i) {
    KJS::Object o = KJS::Object(new KstBindDataMatrix(i + start + 1));
    obj.put(exec, dataMatrixBindings[i].name, o, KJS::Function);
  }
}


#define makeDataMatrix(X) dynamic_cast<KstRMatrix*>(const_cast<KstObject*>(X.data()))

KJS::Value KstBindDataMatrix::reload(KJS::ExecState *exec, const KJS::List& args) {
  Q_UNUSED(args)
  KstRMatrixPtr v = makeDataMatrix(_d);
  if (!v) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }
  KstWriteLocker wl(v);
  v->reload();
  return KJS::Undefined();
}


KJS::Value KstBindDataMatrix::changeFile(KJS::ExecState *exec, const KJS::List& args) {
  KstRMatrixPtr v = makeDataMatrix(_d);
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


KJS::Value KstBindDataMatrix::change(KJS::ExecState *exec, const KJS::List& args) {
  KstRMatrixPtr v = makeDataMatrix(_d);
  if (!v) {
    KJS::Object eobj = KJS::Error::create(exec, KJS::GeneralError);
    exec->setException(eobj);
    return KJS::Undefined();
  }

  if (args.size() > 3) {
    if (args[0].type() != KJS::NumberType ||
        args[1].type() != KJS::NumberType) {
      KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
      exec->setException(eobj);
      return KJS::Undefined();
    }

    KstWriteLocker wl(v);
    int xStart = d2i(args[0].toNumber(exec));
    int yStart = d2i(args[1].toNumber(exec));
    int xCount = d2i(args[2].toNumber(exec));
    int yCount = d2i(args[3].toNumber(exec));
    int skip = v->doSkip() ? v->skip() : -1;
    bool ave = v->doAverage();

    if (args.size() > 4) {
      if (args[4].type() != KJS::NumberType) {
        KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
        exec->setException(eobj);
        return KJS::Undefined();
      }

      skip = d2i(args[4].toNumber(exec));

      if (args.size() > 5) {
        if (args[5].type() != KJS::BooleanType) {
          KJS::Object eobj = KJS::Error::create(exec, KJS::TypeError);
          exec->setException(eobj);
          return KJS::Undefined();
        }

        ave = d2i(args[5].toBoolean(exec));
      }
    }

    v->change(v->dataSource(), v->field(), v->tag(), xStart, yStart, xCount, yCount, skip, skip >= 0, ave);
    return KJS::Undefined();
  }

  KJS::Object eobj = KJS::Error::create(exec, KJS::SyntaxError);
  exec->setException(eobj);
  return KJS::Undefined();
}


KJS::Value KstBindDataMatrix::valid(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRMatrixPtr v = makeDataMatrix(_d);
  KstReadLocker rl(v);
  return KJS::Boolean(v->isValid());
}


KJS::Value KstBindDataMatrix::skip(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRMatrixPtr v = makeDataMatrix(_d);
  KstReadLocker rl(v);
  return KJS::Boolean(v->doSkip());
}


KJS::Value KstBindDataMatrix::boxcar(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRMatrixPtr v = makeDataMatrix(_d);
  KstReadLocker rl(v);
  return KJS::Boolean(v->doAverage());
}


KJS::Value KstBindDataMatrix::xReadToEnd(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRMatrixPtr v = makeDataMatrix(_d);
  KstReadLocker rl(v);
  return KJS::Boolean(v->xReadToEnd());
}


KJS::Value KstBindDataMatrix::yReadToEnd(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRMatrixPtr v = makeDataMatrix(_d);
  KstReadLocker rl(v);
  return KJS::Boolean(v->yReadToEnd());
}


KJS::Value KstBindDataMatrix::xCountFromEnd(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRMatrixPtr v = makeDataMatrix(_d);
  KstReadLocker rl(v);
  return KJS::Boolean(v->xCountFromEnd());
}


KJS::Value KstBindDataMatrix::yCountFromEnd(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRMatrixPtr v = makeDataMatrix(_d);
  KstReadLocker rl(v);
  return KJS::Boolean(v->yCountFromEnd());
}


KJS::Value KstBindDataMatrix::skipLength(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRMatrixPtr v = makeDataMatrix(_d);
  KstReadLocker rl(v);
  return KJS::Number(v->skip());
}


KJS::Value KstBindDataMatrix::field(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstRMatrixPtr v = makeDataMatrix(_d);
  KstReadLocker rl(v);
  return KJS::String(v->field());
}


KJS::Value KstBindDataMatrix::dataSource(KJS::ExecState *exec) const {
  KstRMatrixPtr v = makeDataMatrix(_d);
  KstReadLocker rl(v);
  KstDataSourcePtr dp = v->dataSource();
  if (!dp) {
    return KJS::Undefined();
  }

  return KJS::Object(new KstBindDataSource(exec, dp));
}

#undef makeDataMatrix

// vim: ts=2 sw=2 et
