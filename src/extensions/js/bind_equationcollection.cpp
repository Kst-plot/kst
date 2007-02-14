/***************************************************************************
                         bind_equationcollection.cpp
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

#include "bind_equationcollection.h"
#include "bind_equation.h"

#include <kst.h>
#include <kstdataobjectcollection.h>

#include <kdebug.h>

KstBindEquationCollection::KstBindEquationCollection(KJS::ExecState *exec)
: KstBindCollection(exec, "EquationCollection", true) {
  _equations = kstObjectSubList<KstDataObject,KstEquation>(KST::dataObjectList).tagNames();
}


KstBindEquationCollection::~KstBindEquationCollection() {
}


KJS::Value KstBindEquationCollection::length(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return KJS::Number(_equations.count());
}


QStringList KstBindEquationCollection::collection(KJS::ExecState *exec) const {
  Q_UNUSED(exec)
  return _equations;
}


KJS::Value KstBindEquationCollection::extract(KJS::ExecState *exec, const KJS::Identifier& item) const {
  KstEquationList pl = kstObjectSubList<KstDataObject,KstEquation>(KST::dataObjectList);
  KstEquationPtr p = *pl.findTag(item.qstring());
  if (p) {
    return KJS::Object(new KstBindEquation(exec, p));
  }
  return KJS::Undefined();
}


KJS::Value KstBindEquationCollection::extract(KJS::ExecState *exec, unsigned item) const {
  KstEquationList pl = kstObjectSubList<KstDataObject,KstEquation>(KST::dataObjectList);
  KstEquationPtr p;
  if (item < pl.count()) {
    p = pl[item];
  }
  if (p) {
    return KJS::Object(new KstBindEquation(exec, p));
  }
  return KJS::Undefined();
}


// vim: ts=2 sw=2 et
