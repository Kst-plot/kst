/***************************************************************************
                        bind_pluginmodulecollection.cpp
                             -------------------
    begin                : Apr 12 2005
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

#include "bind_pluginmodulecollection.h"
#include "bind_pluginmodule.h"

#include <plugincollection.h>

#include <kdebug.h>

KstBindPluginModuleCollection::KstBindPluginModuleCollection(KJS::ExecState *exec)
: KstBindCollection(exec, "PluginModuleCollection", true) {
}


KstBindPluginModuleCollection::~KstBindPluginModuleCollection() {
}


KJS::Value KstBindPluginModuleCollection::length(KJS::ExecState *exec) const {
  return KJS::Number(collection(exec).count());
}


QStringList KstBindPluginModuleCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  QStringList rc;
  const QMap<QString,Plugin::Data>& pluginList = PluginCollection::self()->pluginList();
  for (QMap<QString,Plugin::Data>::ConstIterator it = pluginList.begin(); it != pluginList.end(); ++it) {
    rc << it.data()._name;
  }

  return rc;
}


KJS::Value KstBindPluginModuleCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  const QMap<QString,Plugin::Data>& pluginList = PluginCollection::self()->pluginList();
  QString i = item.qstring();
  for (QMap<QString,Plugin::Data>::ConstIterator it = pluginList.begin(); it != pluginList.end(); ++it) {
    if (it.data()._name == i) {
      return KJS::Object(new KstBindPluginModule(exec, it.data()));
    }
  }
  return KJS::Undefined();
}


KJS::Value KstBindPluginModuleCollection::extract(KJS::ExecState *exec, unsigned item) const {
  const QMap<QString,Plugin::Data>& pluginList = PluginCollection::self()->pluginList();
  uint j = 0;
  for (QMap<QString,Plugin::Data>::ConstIterator it = pluginList.begin(); it != pluginList.end(); ++it) {
    if (j++ == item) {
      return KJS::Object(new KstBindPluginModule(exec, it.data()));
    }
  }
  return KJS::Undefined();
}


// vim: ts=2 sw=2 et
