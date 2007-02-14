/***************************************************************************
                        bind_extensioncollection.cpp
                             -------------------
    begin                : Apr 11 2005
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

#include "bind_extensioncollection.h"
#include "bind_extension.h"

#include <kservicetype.h>
#include <extensionmgr.h>
#include <kdebug.h>

KstBindExtensionCollection::KstBindExtensionCollection(KJS::ExecState *exec)
: KstBindCollection(exec, "ExtensionCollection", true) {
}


KstBindExtensionCollection::~KstBindExtensionCollection() {
}


KJS::Value KstBindExtensionCollection::length(KJS::ExecState *exec) const {
  return KJS::Number(collection(exec).count());
}


QStringList KstBindExtensionCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_extensions.isEmpty()) {
    QStringList& l = const_cast<KstBindExtensionCollection*>(this)->_extensions;
    KService::List sl = KServiceType::offers("Kst Extension");
    for (KService::List::ConstIterator it = sl.begin(); it != sl.end(); ++it) {
      l << (*it)->property("Name").toString();
    }
  }

  return _extensions;
}


KJS::Value KstBindExtensionCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  QStringList l = collection(exec);
  if (l.contains(item.qstring())) {
    return KJS::Object(new KstBindExtension(exec, item.qstring()));
  }
  return KJS::Undefined();
}


KJS::Value KstBindExtensionCollection::extract(KJS::ExecState *exec, unsigned item) const {
  QStringList l = collection(exec);
  if (item >= l.count()) {
    return KJS::Undefined();
  }

  return KJS::Object(new KstBindExtension(exec, l[item]));
}


// vim: ts=2 sw=2 et
