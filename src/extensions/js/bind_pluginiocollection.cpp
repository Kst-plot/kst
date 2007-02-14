/***************************************************************************
                         bind_pluginiocollection.cpp
                             -------------------
    begin                : Apr 18 2005
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

#include "bind_pluginiocollection.h"
#include "bind_pluginio.h"

#include <plugin.h>

#include <kdebug.h>

KstBindPluginIOCollection::KstBindPluginIOCollection(KJS::ExecState *exec, const QValueList<Plugin::Data::IOValue>& data, bool input)
: KstBindCollection(exec, input ? "PluginInputCollection" : "PluginOutputCollection", true), _d(data), _input(input) {
}


KstBindPluginIOCollection::~KstBindPluginIOCollection() {
}


KJS::Value KstBindPluginIOCollection::length(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Number(_d.count());
}


QStringList KstBindPluginIOCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  QStringList rc;
  for (QValueList<Plugin::Data::IOValue>::ConstIterator it = _d.begin(); it != _d.end(); ++it) {
    rc << (*it)._name;
  }

  return rc;
}


KJS::Value KstBindPluginIOCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  QString i = item.qstring();
  for (QValueList<Plugin::Data::IOValue>::ConstIterator it = _d.begin(); it != _d.end(); ++it) {
    if ((*it)._name == i) {
      return KJS::Object(new KstBindPluginIO(exec, *it, _input));
    }
  }
  return KJS::Undefined();
}


KJS::Value KstBindPluginIOCollection::extract(KJS::ExecState *exec, unsigned item) const {
  if (item < _d.count()) {
    return KJS::Object(new KstBindPluginIO(exec, _d[item], _input));
  }
  return KJS::Undefined();
}


// vim: ts=2 sw=2 et
