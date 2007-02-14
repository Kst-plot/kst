/***************************************************************************
                         bind_datasourcecollection.cpp
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

#include "bind_datasourcecollection.h"
#include "bind_datasource.h"

#include <kstdatasource.h>
#include <kstdatacollection.h>

#include <kdebug.h>

KstBindDataSourceCollection::KstBindDataSourceCollection(KJS::ExecState *exec, const KstDataSourceList& dataSources)
: KstBindCollection(exec, "DataSourceCollection", true) {
  _isGlobal = false;
  _sources = dataSources.fileNames();
}


KstBindDataSourceCollection::KstBindDataSourceCollection(KJS::ExecState *exec)
: KstBindCollection(exec, "DataSourceCollection", true) {
  _isGlobal = true;
}


KstBindDataSourceCollection::~KstBindDataSourceCollection() {
}


KJS::Value KstBindDataSourceCollection::length(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_isGlobal) {
    KstReadLocker rl(&KST::dataSourceList.lock());
    return KJS::Number(KST::dataSourceList.count());
  }

  return KJS::Number(_sources.count());
}


QStringList KstBindDataSourceCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  if (_isGlobal) {
    KstReadLocker rl(&KST::dataSourceList.lock());
    return KST::dataSourceList.fileNames();
  }

  return _sources;
}


KJS::Value KstBindDataSourceCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  KstReadLocker rl(&KST::dataSourceList.lock());
  KstDataSourcePtr vp = *KST::dataSourceList.findFileName(item.qstring());
  if (!vp) {
    return KJS::Undefined();
  }

  return KJS::Object(new KstBindDataSource(exec, vp));
}


KJS::Value KstBindDataSourceCollection::extract(KJS::ExecState *exec, unsigned item) const {
  if (_isGlobal) {
    KstReadLocker rl(&KST::dataSourceList.lock());
    if (item >= KST::dataSourceList.count()) {
      return KJS::Undefined();
    }

    KstDataSourcePtr vp = KST::dataSourceList[item];
    if (!vp) {
      return KJS::Undefined();
    }

    return KJS::Object(new KstBindDataSource(exec, vp));
  }

  if (item >= _sources.count()) {
    return KJS::Undefined();
  }

  KstReadLocker rl(&KST::dataSourceList.lock());
  KstDataSourcePtr vp = *KST::dataSourceList.findFileName(_sources[item]);
  if (!vp) {
    return KJS::Undefined();
  }
  return KJS::Object(new KstBindDataSource(exec, vp));
}


// vim: ts=2 sw=2 et
