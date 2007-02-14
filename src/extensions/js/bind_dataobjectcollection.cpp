/***************************************************************************
                        bind_dataobjectcollection.cpp
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

#include "bind_dataobjectcollection.h"
#include "bind_dataobject.h"
#include "bind_equationcollection.h"
#include "bind_histogramcollection.h"
#include "bind_plugincollection.h"
#include "bind_powerspectrumcollection.h"

#include <kstdataobjectcollection.h>

#include <kdebug.h>

KstBindDataObjectCollection::KstBindDataObjectCollection(KJS::ExecState *exec)
: KstBindCollection(exec, "DataObjectCollection", true) {
}


KstBindDataObjectCollection::~KstBindDataObjectCollection() {
}


struct DataObjectCollectionProperties {
  const char *name;
  void (KstBindDataObjectCollection::*set)(KJS::ExecState*, const KJS::Value&);
  KJS::Value (KstBindDataObjectCollection::*get)(KJS::ExecState*) const;
};


static DataObjectCollectionProperties dataObjectCollectionProperties[] = {
  { "powerSpectrums", 0, &KstBindDataObjectCollection::powerSpectrums },
  { "equations", 0, &KstBindDataObjectCollection::equations },
  { "histograms", 0, &KstBindDataObjectCollection::histograms },
  { "plugins", 0, &KstBindDataObjectCollection::plugins },
  { 0L, 0L, 0L }
};


KJS::ReferenceList KstBindDataObjectCollection::propList(KJS::ExecState *exec, bool recursive) {
  KJS::ReferenceList rc = KstBindCollection::propList(exec, recursive);

  for (int i = 0; dataObjectCollectionProperties[i].name; ++i) {
    rc.append(KJS::Reference(this, KJS::Identifier(dataObjectCollectionProperties[i].name)));
  }

  return rc;
}


bool KstBindDataObjectCollection::hasProperty(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  QString prop = propertyName.qstring();
  for (int i = 0; dataObjectCollectionProperties[i].name; ++i) {
    if (prop == dataObjectCollectionProperties[i].name) {
      return true;
    }
  }

  return KstBindCollection::hasProperty(exec, propertyName);
}


KJS::Value KstBindDataObjectCollection::get(KJS::ExecState *exec, const KJS::Identifier& propertyName) const {
  if (id() > 0) {
    return KstBindCollection::get(exec, propertyName);
  }

  QString prop = propertyName.qstring();
  for (int i = 0; dataObjectCollectionProperties[i].name; ++i) {
    if (prop == dataObjectCollectionProperties[i].name) {
      if (!dataObjectCollectionProperties[i].get) {
        break;
      }
      return (this->*dataObjectCollectionProperties[i].get)(exec);
    }
  }
  
  return KstBindCollection::get(exec, propertyName);
}


KJS::Value KstBindDataObjectCollection::length(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstReadLocker rl(&KST::dataObjectList.lock());
  return KJS::Number(KST::dataObjectList.count());
}


QStringList KstBindDataObjectCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  KstReadLocker rl(&KST::dataObjectList.lock());
  return KST::dataObjectList.tagNames();
}


KJS::Value KstBindDataObjectCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  KstReadLocker rl(&KST::dataObjectList.lock());
  KstDataObjectPtr d = *KST::dataObjectList.findTag(item.qstring());
  if (d) {
    return KJS::Object(KstBindDataObject::bind(exec, d));
  }
  return KJS::Undefined();
}


KJS::Value KstBindDataObjectCollection::extract(KJS::ExecState *exec, unsigned item) const {
  KstReadLocker rl(&KST::dataObjectList.lock());
  KstDataObjectPtr d;
  if (item < KST::dataObjectList.count()) {
    d = KST::dataObjectList[item];
  }
  if (d) {
    return KJS::Object(KstBindDataObject::bind(exec, d));
  }
  return KJS::Undefined();
}


KJS::Value KstBindDataObjectCollection::powerSpectrums(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindPowerSpectrumCollection(exec));
}


KJS::Value KstBindDataObjectCollection::equations(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindEquationCollection(exec));
}


KJS::Value KstBindDataObjectCollection::histograms(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindHistogramCollection(exec));
}


KJS::Value KstBindDataObjectCollection::plugins(KJS::ExecState *exec) const {
  return KJS::Object(new KstBindPluginCollection(exec));
}


int KstBindDataObject::methodCount() const {
  return KstBindObject::methodCount();
}


int KstBindDataObject::propertyCount() const {
  return KstBindObject::propertyCount();
}


// vim: ts=2 sw=2 et
