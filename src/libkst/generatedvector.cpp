/***************************************************************************
                          generatedvector.cpp - a vector from x0 to x1 with n pts
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
#include <QDebug>
#include <QXmlStreamWriter>

#include "kst_i18n.h"

#include "generatedvector.h"

namespace Kst {

const QString GeneratedVector::staticTypeString = I18N_NOOP("Generated Vector");
const QString GeneratedVector::staticTypeTag = I18N_NOOP("generatedvector");

GeneratedVector::GeneratedVector(ObjectStore *store, const QByteArray &data, double x0, double x1, int n)
    : Vector(store, data) {
  _saveable = true;
  _saveData = false;
  changeRange(x0, x1, n);
}


GeneratedVector::GeneratedVector(ObjectStore *store, double x0, double x1, int n)
    : Vector(store, n) {
  _saveable = true;
  _saveData = false;
  changeRange(x0, x1, n);
}


const QString& GeneratedVector::typeString() const {
  return staticTypeString;
}


void GeneratedVector::save(QXmlStreamWriter &s) {
  s.writeStartElement("generatedvector");
  s.writeAttribute("min", QString::number(min()));
  s.writeAttribute("max", QString::number(max()));
  s.writeAttribute("count", QString::number(length()));
  saveNameInfo(s, VNUM|XNUM);

  s.writeEndElement();
}


void GeneratedVector::changeRange(double x0, double x1, int n) {
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

  internalUpdate(Object::UPDATE);

  setDirty(false);
}


Object::UpdateType GeneratedVector::update() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  Object::UpdateType baseRC = Vector::update();
  if (force) {
    baseRC = UPDATE;
  }

  return baseRC;
}


void GeneratedVector::setSaveData(bool save) {
  Q_UNUSED(save)
}

QString GeneratedVector::_automaticDescriptiveName() const {
  return QString::number(_v[0])+".."+QString::number(_v[length()-1]);
}

QString GeneratedVector::description() {
  return i18n("Generated Vector: %1\n"
      "  %2 values from %3 to %4").arg(Name()).arg(length()).arg(_v[0]).arg(_v[length()-1]);
}

}

// vim: ts=2 sw=2 et
