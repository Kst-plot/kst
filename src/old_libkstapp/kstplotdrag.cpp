/***************************************************************************
                 kstplotdrag.cpp: class for plot drag objects
                             -------------------
    begin                : Apr 06, 2004
    copyright            : (C) 2003 The University of Toronto
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

#include "kstplotdrag.h"
#include "kstviewobjectfactory.h"

#include <stdlib.h>

KstPlotDrag::KstPlotDrag(QWidget *dragSource)
: KstDrag(mimeType(), dragSource)
{
}


const char *KstPlotDrag::mimeType() {
  return "application/x-kst-plots";
}

KstPlotDrag::~KstPlotDrag() {
}


void KstPlotDrag::setPlots(const KstViewObjectList& l) {
  QByteArray a;
  QDataStream ds(&a, QIODevice::WriteOnly);
  ds << l.count();
  for (KstViewObjectList::ConstIterator i = l.begin(); i != l.end(); ++i) {
    ds << *i;
  }
  setEncodedData(a);
}


KstViewObjectList KstPlotDrag::decodedContents(QByteArray& a) {
  uint x;
  QDataStream ds(&a, QIODevice::ReadOnly);
  ds >> x;
  KstViewObjectList c;
  for (uint i = 0; i < x; ++i) {
    QString type;
    ds >> type;
    KstViewObjectPtr p = KstViewObjectFactory::self()->createA(type);
    if (p) {
      ds >> p;
      c.append(p);
    } else {
      // FIXME: how to recover?
      abort();
    }
  }
  return c;
}


KstPlotDrag& operator<<(KstPlotDrag& drag, KstViewObjectList objs) {
  drag.setPlots(objs);
  return drag;
}


KstPlotDrag& operator<<(KstPlotDrag& drag, KstViewObjectPtr obj) {
  KstViewObjectList list;
  list.append(obj);
  drag.setPlots(list);
  return drag;
}

// vim: ts=2 sw=2 et

