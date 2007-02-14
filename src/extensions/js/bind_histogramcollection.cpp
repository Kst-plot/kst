/***************************************************************************
                        bind_histogramcollection.cpp
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

#include "bind_histogramcollection.h"
#include "bind_histogram.h"

#include <kst.h>
#include <kstdataobjectcollection.h>

#include <kdebug.h>

KstBindHistogramCollection::KstBindHistogramCollection(KJS::ExecState *exec)
: KstBindCollection(exec, "HistogramCollection", true) {
  _histograms = kstObjectSubList<KstDataObject,KstHistogram>(KST::dataObjectList).tagNames();
}


KstBindHistogramCollection::~KstBindHistogramCollection() {
}


KJS::Value KstBindHistogramCollection::length(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Number(_histograms.count());
}


QStringList KstBindHistogramCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return _histograms;
}


KJS::Value KstBindHistogramCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  KstHistogramList pl = kstObjectSubList<KstDataObject,KstHistogram>(KST::dataObjectList);
  KstHistogramPtr p = *pl.findTag(item.qstring());
  if (p) {
    return KJS::Object(new KstBindHistogram(exec, p));
  }
  return KJS::Undefined();
}


KJS::Value KstBindHistogramCollection::extract(KJS::ExecState *exec, unsigned item) const {
  KstHistogramList pl = kstObjectSubList<KstDataObject,KstHistogram>(KST::dataObjectList);
  KstHistogramPtr p;
  if (item < pl.count()) {
    p = pl[item];
  }
  if (p) {
    return KJS::Object(new KstBindHistogram(exec, p));
  }
  return KJS::Undefined();
}


// vim: ts=2 sw=2 et
