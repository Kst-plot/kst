/***************************************************************************
                          bind_stringcollection.cpp
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

#include "bind_stringcollection.h"
#include "bind_string.h"

#include <kst.h>
#include <kstdatacollection.h>

#include <kdebug.h>

KstBindStringCollection::KstBindStringCollection(KJS::ExecState *exec, const KstStringList& strings)
: KstBindCollection(exec, "StringCollection", true) {
  _isGlobal = false;
  _strings = strings.tagNames();
}


KstBindStringCollection::KstBindStringCollection(KJS::ExecState *exec)
: KstBindCollection(exec, "StringCollection", true) {
  _isGlobal = true;
}


KstBindStringCollection::~KstBindStringCollection() {
}


KJS::Value KstBindStringCollection::length(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_isGlobal) {
    KstReadLocker rl(&KST::stringList.lock());
    return KJS::Number(KST::stringList.count());
  }

  return KJS::Number(_strings.count());
}


QStringList KstBindStringCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_isGlobal) {
    KstReadLocker rl(&KST::stringList.lock());
    return KST::stringList.tagNames();
  }

  return _strings;
}


KJS::Value KstBindStringCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  KstReadLocker rl(&KST::stringList.lock());
  KstStringPtr sp = *KST::stringList.findTag(item.qstring());
  if (!sp) {
    return KJS::Undefined();
  }

  return KJS::Object(new KstBindString(exec, sp));
}


KJS::Value KstBindStringCollection::extract(KJS::ExecState *exec, unsigned item) const {
  if (_isGlobal) {
    KstReadLocker rl(&KST::stringList.lock());
    if (item >= KST::stringList.count()) {
      return KJS::Undefined();
    }

    KstStringPtr sp = KST::stringList[item];
    if (!sp) {
      return KJS::Undefined();
    }

    return KJS::Object(new KstBindString(exec, sp));
  }

  if (item >= _strings.count()) {
    return KJS::Undefined();
  }

  KstReadLocker rl(&KST::stringList.lock());
  KstStringPtr sp = *KST::stringList.findTag(_strings[item]);
  if (!sp) {
    return KJS::Undefined();
  }
  return KJS::Object(new KstBindString(exec, sp));
}


// vim: ts=2 sw=2 et
