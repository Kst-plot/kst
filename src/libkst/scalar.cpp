/***************************************************************************
                          scalar.cpp  -  the base scalar type
                             -------------------
    begin                : March 24, 2003
    copyright            : (C) 2003 by cbn
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

#include "scalar.h"

#include <QDebug>
#include <QTextDocument>
#include <QXmlStreamWriter>

#include "kst_i18n.h"

namespace Kst {


const QString Scalar::staticTypeString = I18N_NOOP("Scalar");
const QString Scalar::staticTypeTag = I18N_NOOP("scalar");


/** Create the base scalar */
Scalar::Scalar(ObjectStore *store)
    : Primitive(store, 0L), _value(0.0), _orphan(false), _displayable(true), _editable(false) {

  _initializeShortName();
}

void Scalar::_initializeShortName() {
  _shortName = 'X'+QString::number(_xnum);
  if (_xnum>max_xnum)
    max_xnum = _xnum;
  _xnum++;
}

Scalar::~Scalar() {
  //qDebug() << "scalar destructor for: " << Name();
}


const QString& Scalar::typeString() const {
  return staticTypeString;
}


void Scalar::internalUpdate() {
  // do nothing
}


void Scalar::save(QXmlStreamWriter &s) {
  if (provider()) {
    return;
  }
  s.writeStartElement("scalar");
  if (_orphan) {
    s.writeAttribute("orphan", "true");
  }
  if (_editable) {
    s.writeAttribute("editable", "true");
  }
  s.writeAttribute("value", QString::number(value()));
  saveNameInfo(s, XNUM);
  s.writeEndElement();
}


Scalar& Scalar::operator=(double v) {
  setValue(v);
  return *this;
}


void Scalar::setValue(double inV) {
  writeLock();
  if (_value != inV) {
    _value = inV;
    registerChange();
  }
  unlock();
}


QString Scalar::label() const {
  return QString::number(_value);
}


double Scalar::value() const {
  return _value;
}


bool Scalar::orphan() const {
  return _orphan;
}


void Scalar::setOrphan(bool orphan) {
  _orphan = orphan;
}


bool Scalar::displayable() const {
  return _displayable;
}


void Scalar::setDisplayable(bool displayable) {
  _displayable = displayable;
}


bool Scalar::editable() const {
  return _editable;
}


void Scalar::setEditable(bool editable) {
  _editable = editable;
}

QString Scalar::descriptionTip() const {
  if (_provider) {
    return i18n("Scalar: %1 = %2\n%3").arg(Name()).arg(value()).arg(_provider->descriptionTip());
  } else {
    return i18n("Scalar: %1 = %2").arg(Name()).arg(value());
  }
}

QString Scalar::_automaticDescriptiveName() const {
  if (_orphan) {
    return QString::number(value());
  } else {
    return Primitive::_automaticDescriptiveName();
  }
}

QString Scalar::sizeString() const {
  return QString("1");
}

QString Scalar::propertyString() const {
  return i18n("Value: %1").arg(value());
}
}
// vim: et ts=2 sw=2
