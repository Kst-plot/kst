/***************************************************************************
                          bind_scalarcollection.cpp
                             -------------------
    begin                : Mar 31 2005
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

#include "bind_scalarcollection.h"
#include "bind_scalar.h"

#include <kst.h>
#include <kstdatacollection.h>

#include <kdebug.h>

KstBindScalarCollection::KstBindScalarCollection(KJS::ExecState *exec, const KstScalarList& scalars)
: KstBindCollection(exec, "ScalarCollection", true) {
  _isGlobal = false;
  _scalars = scalars.tagNames();
}


KstBindScalarCollection::KstBindScalarCollection(KJS::ExecState *exec)
: KstBindCollection(exec, "ScalarCollection", true) {
  _isGlobal = true;
}


KstBindScalarCollection::~KstBindScalarCollection() {
}


KJS::Value KstBindScalarCollection::length(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_isGlobal) {
    KstReadLocker rl(&KST::scalarList.lock());
    return KJS::Number(KST::scalarList.count());
  }

  return KJS::Number(_scalars.count());
}


QStringList KstBindScalarCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_isGlobal) {
    KstReadLocker rl(&KST::scalarList.lock());
    return KST::scalarList.tagNames();
  }

  return _scalars;
}


KJS::Value KstBindScalarCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  KstReadLocker rl(&KST::scalarList.lock());
  KstScalarPtr sp = *KST::scalarList.findTag(item.qstring());
  if (!sp) {
    return KJS::Undefined();
  }

  return KJS::Object(new KstBindScalar(exec, sp));
}


KJS::Value KstBindScalarCollection::extract(KJS::ExecState *exec, unsigned item) const {
  if (_isGlobal) {
    KstReadLocker rl(&KST::scalarList.lock());
    if (item >= KST::scalarList.count()) {
      return KJS::Undefined();
    }

    KstScalarPtr sp = KST::scalarList[item];
    if (!sp) {
      return KJS::Undefined();
    }

    return KJS::Object(new KstBindScalar(exec, sp));
  }

  if (item >= _scalars.count()) {
    return KJS::Undefined();
  }

  KstReadLocker rl(&KST::scalarList.lock());
  KstScalarPtr sp = *KST::scalarList.findTag(_scalars[item]);
  if (!sp) {
    return KJS::Undefined();
  }
  return KJS::Object(new KstBindScalar(exec, sp));
}


// vim: ts=2 sw=2 et
