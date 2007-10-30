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

String::String(ObjectStore *store, ObjectTag tag, Object *provider, const QString& val, bool orphan)
    : Primitive(store, tag, provider), _value(val), _orphan(orphan), _editable(false) {
}


String::String(ObjectStore *store, QDomElement& e)
    : Primitive(store), _orphan(false), _editable(false) {
  QDomNode n = e.firstChild();

  while (!n.isNull()) {
    QDomElement e = n.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "tag") {
        setTagName(ObjectTag::fromString(e.text()));
      } else if (e.tagName() == "orphan") {
        _orphan = true;
      } else if (e.tagName() == "value") {
        setValue(e.text());
      } else if (e.tagName() == "editable") {
        _editable = true;
      }
    }
    n = n.nextSibling();
  }
}


String::~String() {
}


const QString& String::typeString() const {
  return staticTypeString;
}


void String::save(QXmlStreamWriter &s) {
  s.writeStartElement("string");
  s.writeAttribute("tag", tag().tagString());
  if (_orphan) {
    s.writeAttribute("orphan", "true");
  }
  if (_editable) {
    s.writeAttribute("editable", "true");
  }
  s.writeAttribute("value", value());
  s.writeEndElement();
}


Object::UpdateType String::update(int updateCounter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  if (Object::checkUpdateCounter(updateCounter) && !force) {
    return lastUpdateResult();
  }

  QString v = value();
  if (_provider) {
    _provider->update(updateCounter);
  }

  return setLastUpdateResult(v == value() ? NO_CHANGE : UPDATE);
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
  setDirty();
  _value = inV;
  emit trigger();
}

}

// vim: ts=2 sw=2 et
