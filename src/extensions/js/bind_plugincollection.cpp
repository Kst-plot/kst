/***************************************************************************
                         bind_plugincollection.cpp
                             -------------------
    begin                : Apr 10 2005
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

#include "bind_plugincollection.h"
#include "bind_plugin.h"

#include <kst.h>
#include <kstdataobjectcollection.h>
#include <kstcplugin.h>

#include <kdebug.h>

KstBindPluginCollection::KstBindPluginCollection(KJS::ExecState *exec)
: KstBindCollection(exec, "PluginCollection", true) {
  _plugins = kstObjectSubList<KstDataObject,KstCPlugin>(KST::dataObjectList).tagNames();
}


KstBindPluginCollection::~KstBindPluginCollection() {
}


KJS::Value KstBindPluginCollection::length(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Number(_plugins.count());
}


QStringList KstBindPluginCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return _plugins;
}


KJS::Value KstBindPluginCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  KstCPluginList pl = kstObjectSubList<KstDataObject,KstCPlugin>(KST::dataObjectList);
  KstCPluginPtr p = *pl.findTag(item.qstring());
  if (p) {
    return KJS::Object(new KstBindPlugin(exec, p));
  }
  return KJS::Undefined();
}


KJS::Value KstBindPluginCollection::extract(KJS::ExecState *exec, unsigned item) const {
  KstCPluginList pl = kstObjectSubList<KstDataObject,KstCPlugin>(KST::dataObjectList);
  KstCPluginPtr p;
  if (item < pl.count()) {
    p = pl[item];
  }
  if (p) {
    return KJS::Object(new KstBindPlugin(exec, p));
  }
  return KJS::Undefined();
}


// vim: ts=2 sw=2 et
