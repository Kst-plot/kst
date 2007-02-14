/***************************************************************************
                               kstsvector.cpp
                             -------------------
    begin                : Sep 25, 2005
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
#include "kstgvector.h"
#include <ksdebug.h>

KstGVector::KstGVector(const QString& tag)
: KstVector(KstObjectTag(tag, KstObjectTag::globalTagContext), 1) {  // FIXME: do tag context properly
  _v[0] = 0;
  _cycles = 0;
  connect(&_t, SIGNAL(timeout()), SLOT(cycle()));
}


void KstGVector::save(QTextStream& ts, const QString& indent, bool saveAbsolutePosition) {
  Q_UNUSED(saveAbsolutePosition)
  Q_UNUSED(ts)
  Q_UNUSED(indent)
}


void KstGVector::changeRange(double x0, double x1, int n) {
  Q_UNUSED(x0)
  Q_UNUSED(x1)
  Q_UNUSED(n)
}


KstObject::UpdateType KstGVector::update(int update_counter) {
  bool force = dirty();

  if (KstObject::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  int oldS = length();
  int newS = oldS + _cycles;
  if (oldS != newS) {
    resize(newS);
    for (int i = oldS; i < newS; ++i) {
      _v[i] = double(i);
    }
    _cycles = 0;
    force = true;
  }

  KstObject::UpdateType baseRC = KstVector::update(update_counter);
  if (force) {
    baseRC = UPDATE;
  }

  return setLastUpdateResult(baseRC);
}


void KstGVector::setFrequency(int f) {
  if (f == 0) {
    _t.stop();
  } else {
    _t.start(1000000 / f);
  }
}


void KstGVector::cycle() {
  ++_cycles;
  setDirty();
}


#include "kstgvector.moc"
// vim: ts=2 sw=2 et
