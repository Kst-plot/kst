/***************************************************************************
                      bind_powerspectrumcollection.cpp
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

#include "bind_powerspectrumcollection.h"
#include "bind_powerspectrum.h"

#include <kst.h>
#include <kstdataobjectcollection.h>

#include <kdebug.h>

KstBindPowerSpectrumCollection::KstBindPowerSpectrumCollection(KJS::ExecState *exec)
: KstBindCollection(exec, "PowerSpectrumCollection", true) {
  _psds = kstObjectSubList<KstDataObject,KstPSD>(KST::dataObjectList).tagNames();
}


KstBindPowerSpectrumCollection::~KstBindPowerSpectrumCollection() {
}


KJS::Value KstBindPowerSpectrumCollection::length(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Number(_psds.count());
}


QStringList KstBindPowerSpectrumCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return _psds;
}


KJS::Value KstBindPowerSpectrumCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  KstPSDList pl = kstObjectSubList<KstDataObject,KstPSD>(KST::dataObjectList);
  KstPSDPtr p = *pl.findTag(item.qstring());
  if (p) {
    return KJS::Object(new KstBindPowerSpectrum(exec, p));
  }
  return KJS::Undefined();
}


KJS::Value KstBindPowerSpectrumCollection::extract(KJS::ExecState *exec, unsigned item) const {
  KstPSDList pl = kstObjectSubList<KstDataObject,KstPSD>(KST::dataObjectList);
  KstPSDPtr p;
  if (item < pl.count()) {
    p = pl[item];
  }
  if (p) {
    return KJS::Object(new KstBindPowerSpectrum(exec, p));
  }
  return KJS::Undefined();
}


// vim: ts=2 sw=2 et
