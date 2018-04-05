/***************************************************************************
                          generatedvector.cpp - a vector from x0 to x1 with n pts
                             -------------------
    begin                : March, 2005
    copyright            : (C) 2005 by cbn
    email                : netterfield@astro.utoronto.ca
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "generatedvector.h"
#include "vectorscriptinterface.h"

#include <QDebug>
#include <QXmlStreamWriter>


namespace Kst {

const QString GeneratedVector::staticTypeString = "Generated Vector";
const QString GeneratedVector::staticTypeTag = "generatedvector";

GeneratedVector::GeneratedVector(ObjectStore *store)
    : Vector(store) {
  _saveable = true;
  _saveData = false;
}


const QString& GeneratedVector::typeString() const {
  return staticTypeString;
}


ScriptInterface* GeneratedVector::createScriptInterface() {
  return new GeneratedVectorSI(this);
}


void GeneratedVector::save(QXmlStreamWriter &s) {
  s.writeStartElement("generatedvector");
  s.writeAttribute("first", QString::number(value(0)));
  s.writeAttribute("last", QString::number(value(length()-1)));
  s.writeAttribute("min", QString::number(min()));
  s.writeAttribute("max", QString::number(max()));
  s.writeAttribute("count", QString::number(length()));
  saveNameInfo(s, VECTORNUM|SCALARNUM);

  s.writeEndElement();
}


void GeneratedVector::changeRange(double x0, double x1, int n) {
  if (n < 2) {
    n = 2;
  }
  if (n != length()) {
    resize(n, false);
  }

  for (int i = 0; i < n; i++) {
    _v_raw[i] = x0 + double(i) * (x1 - x0) / double(n - 1);
  }

  if (x0 < x1) {
    _min = x0;
    _max = x1;
  } else {
    _min = x1;
    _max = x0;
  }

  _scalars["min"]->setValue(_min);
  _scalars["max"]->setValue(_max);
  _scalars["first"]->setValue(x0);
  _scalars["last"]->setValue(x1);

  _numNew = length();
  registerChange();
}

void GeneratedVector::setSaveData(bool save) {
  Q_UNUSED(save)
}

QString GeneratedVector::_automaticDescriptiveName() const {
  return QString::number(_v_raw[0])+".."+QString::number(_v_raw[length()-1]);
}

QString GeneratedVector::descriptionTip() const {
  return tr("Generated Vector: %1\n"
      "  %2 values from %3 to %4").arg(Name()).arg(length()).arg(_v_raw[0]).arg(_v_raw[length()-1]);
}

QString GeneratedVector::propertyString() const {
  return tr("%3 points from %1 to %2").arg(_v_raw[0]).arg(_v_raw[length()-1]).arg(length());
}

}

// vim: ts=2 sw=2 et
