/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2003 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "string_kst.h"

#include "defaultprimitivenames.h"
#include "datacollection.h"
#include "kst_i18n.h"

#include <qtextdocument.h>
#include <QXmlStreamWriter>


namespace Kst {

static int anonymousStringCounter = 0;

String::String(ObjectTag in_tag, Object *provider, const QString& val, bool orphan)
: Primitive(provider), _value(val), _orphan(orphan), _editable(false) {
  QString _tag = in_tag.tag();
  if (!in_tag.isValid()) {
    do {
      _tag = i18n("Anonymous String %1", anonymousStringCounter++);
    } while (Data::self()->vectorTagNameNotUniqueInternal(_tag));  // FIXME: why vector?
    Object::setTagName(ObjectTag(_tag, in_tag.context()));
  } else {
    Object::setTagName(suggestUniqueStringTag(in_tag));
  }

  stringList.lock().writeLock();
  stringList.append(this);
  stringList.lock().unlock();
}


String::String(QDomElement& e)
: Primitive(), _orphan(false), _editable(false) {
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
  stringList.append(this);
}


String::~String() {
}


void String::setTagName(const ObjectTag& tag) {
  if (tag == this->tag()) {
    return;
  }

  KstWriteLocker l(&stringList.lock());

  stringList.doRename(this, tag);
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
