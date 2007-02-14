/***************************************************************************
                          bind_vectorcollection.cpp
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

#include "bind_vectorcollection.h"
#include "bind_vector.h"

#include <kst.h>
#include <kstdatacollection.h>

#include <kdebug.h>

KstBindVectorCollection::KstBindVectorCollection(KJS::ExecState *exec, const KstVectorList& vectors)
: KstBindCollection(exec, "VectorCollection", true) {
  _isGlobal = false;
  _vectors = vectors.tagNames();
}


KstBindVectorCollection::KstBindVectorCollection(KJS::ExecState *exec)
: KstBindCollection(exec, "VectorCollection", true) {
  _isGlobal = true;
}


KstBindVectorCollection::~KstBindVectorCollection() {
}


KJS::Value KstBindVectorCollection::length(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_isGlobal) {
    KstReadLocker rl(&KST::vectorList.lock());
    return KJS::Number(KST::vectorList.count());
  }

  return KJS::Number(_vectors.count());
}


QStringList KstBindVectorCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_isGlobal) {
    KstReadLocker rl(&KST::vectorList.lock());
    return KST::vectorList.tagNames();
  }

  return _vectors;
}


KJS::Value KstBindVectorCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  KstReadLocker rl(&KST::vectorList.lock());
  KstVectorPtr vp = *KST::vectorList.findTag(item.qstring());
  if (!vp) {
    return KJS::Undefined();
  }

  return KJS::Object(new KstBindVector(exec, vp));
}


KJS::Value KstBindVectorCollection::extract(KJS::ExecState *exec, unsigned item) const {
  if (_isGlobal) {
    KstReadLocker rl(&KST::vectorList.lock());
    if (item >= KST::vectorList.count()) {
      return KJS::Undefined();
    }

    KstVectorPtr vp = KST::vectorList[item];
    if (!vp) {
      return KJS::Undefined();
    }

    return KJS::Object(new KstBindVector(exec, vp));
  }

  if (item >= _vectors.count()) {
    return KJS::Undefined();
  }

  KstReadLocker rl(&KST::vectorList.lock());
  KstVectorPtr vp = *KST::vectorList.findTag(_vectors[item]);
  if (!vp) {
    return KJS::Undefined();
  }
  return KJS::Object(new KstBindVector(exec, vp));
}


// vim: ts=2 sw=2 et
