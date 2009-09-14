/***************************************************************************
                    string.cpp  -  the base string type
                             -------------------
    begin                : Sept 29, 2004
    copyright            : (C) 2004 by The University of Toronto
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

#include <QTextDocument>
#include <QXmlStreamWriter>

#include "kst_i18n.h"
#include "string_kst.h"


namespace Kst {

const QString String::staticTypeString = I18N_NOOP("String");
const QString String::staticTypeTag = I18N_NOOP("string");

String::String(ObjectStore *store)
    : Primitive(store, 0L), _value(QString::null), _orphan(false), _editable(false) {

}

void String::_initializeShortName() {
  _shortName = "T"+QString::number(_tnum);
  if (_tnum>max_tnum)
    max_tnum = _tnum;
  _tnum++;
}


String::~String() {
}


const QString& String::typeString() const {
  return staticTypeString;
}


void String::save(QXmlStreamWriter &s) {
  s.writeStartElement("string");
  if (_orphan) {
    s.writeAttribute("orphan", "true");
  }
  if (_editable) {
    s.writeAttribute("editable", "true");
  }
  s.writeAttribute("value", value());
  saveNameInfo(s, TNUM);
  s.writeEndElement();
}


Object::UpdateType String::update() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  return UPDATE;
}


String& String::operator=(const QString& v) {
  setValue(v);
  return *this;
}


String& String::operator=(const char *v) {
  setValue(v);
  return *this;
}


void String::setValue(const QString& inV) {
  _value = inV;
  emit updated(this);
}


QString String::_automaticDescriptiveName() const {
  if (_orphan) {
    return value();
  } else {
    return Primitive::_automaticDescriptiveName();
  }
}


QString String::descriptionTip() const {
  return i18n("String: %1").arg(Name());
}


QString String::sizeString() const {
  return QString::number(_value.size());
}


QString String::propertyString() const {
  return _value;
}

}

// vim: ts=2 sw=2 et
