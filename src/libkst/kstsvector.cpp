/***************************************************************************
                          kstsvector.cpp - a vector from x0 to x1 with n pts
                             -------------------
    begin                : March, 2005
    copyright            : (C) 2005 by cbn
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
#include "kstsvector.h"
#include <qdebug.h>
#include <qtextstream.h>
#include <QXmlStreamWriter>

KstSVector::KstSVector(const QString &tag, const QByteArray &data, double x0, double x1, int n)
  : KstVector(tag, data) {
  _saveable = true;
  _saveData = false;
  changeRange( x0,  x1,  n );
}


KstSVector::KstSVector(double x0, double x1, int n, KstObjectTag tag)
  : KstVector(tag, n) {
  _saveable = true;
  _saveData = false;
  changeRange( x0, x1, n );
}


void KstSVector::save(QXmlStreamWriter &s) {
  s.writeStartElement("svector");
  s.writeAttribute("tag", tag().tagString());
  s.writeAttribute("min", QString::number(min()));
  s.writeAttribute("max", QString::number(max()));
  s.writeAttribute("count", QString::number(length()));
  s.writeEndElement();
}


void KstSVector::changeRange(double x0, double x1, int n) {
  if (n < 2) {
    n = 2;
  }
  if (n != length()) {
    resize(n, false);
  }
  if (x0 > x1) {
    double tx;
    tx = x0;
    x0 = x1;
    x1 = tx;
  } else if (x0 == x1) {
    x1 = x0 + 0.1;
  }

  for (int i = 0; i < n; i++) {
    _v[i] = x0 + double(i) * (x1 - x0) / double(n - 1);
  }

  _scalars["min"]->setValue(x0);
  _scalars["max"]->setValue(x1);
  
  internalUpdate(KstObject::UPDATE);
  
  setDirty(false);
}


KstObject::UpdateType KstSVector::update(int update_counter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  KstObject::UpdateType baseRC = KstVector::update(update_counter);
  if (force) {
    baseRC = UPDATE;
  }

  return baseRC;
}


void KstSVector::setSaveData(bool save) {
  Q_UNUSED(save)
}


// vim: ts=2 sw=2 et
