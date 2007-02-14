/***************************************************************************
                           bind_objectcollection.cpp
                             -------------------
    begin                : May 31 2005
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

#include "bind_objectcollection.h"
#include "bind_object.h"

#include <qdeepcopy.h>

#include <kstobject.h>

#include <kdebug.h>

KstBindObjectCollection::KstBindObjectCollection(KJS::ExecState *exec, const KstObjectList<KstObjectPtr>& objects)
: KstBindCollection(exec, "ObjectCollection", true) {
  _objects = QDeepCopy<KstObjectList<KstObjectPtr> >(objects);
}


KstBindObjectCollection::KstBindObjectCollection(KJS::ExecState *exec)
: KstBindCollection(exec, "ObjectCollection", true) {
}


KstBindObjectCollection::~KstBindObjectCollection() {
}


KJS::Value KstBindObjectCollection::length(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Number(_objects.count());
}


QStringList KstBindObjectCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return _objects.tagNames();
}


KJS::Value KstBindObjectCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  KstObjectPtr vp = *_objects.findTag(item.qstring());

  if (!vp) {
    return KJS::Undefined();
  }
  return KJS::Object(new KstBindObject(exec, vp));
}


KJS::Value KstBindObjectCollection::extract(KJS::ExecState *exec, unsigned item) const {
  KstObjectPtr vp;
  if (item < _objects.count()) {
    vp = _objects[item];
  }

  if (!vp) {
    return KJS::Undefined();
  }
  return KJS::Object(new KstBindObject(exec, vp));
}


// vim: ts=2 sw=2 et
