/***************************************************************************
                    kststring.cpp  -  the base string type
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

#include "kststring.h"

#include "defaultprimitivenames.h"
#include "datacollection.h"
#include "kst_i18n.h"

#include <qtextdocument.h>
#include <QXmlStreamWriter>


static int anonymousStringCounter = 0;

KstString::KstString(Kst::ObjectTag in_tag, Kst::Object *provider, const QString& val, bool orphan)
: KstPrimitive(provider), _value(val), _orphan(orphan), _editable(false) {
  QString _tag = in_tag.tag();
  if (!in_tag.isValid()) {
    do {
      _tag = i18n("Anonymous String %1", anonymousStringCounter++);
    } while (Kst::Data::self()->vectorTagNameNotUniqueInternal(_tag));  // FIXME: why vector?
    Kst::Object::setTagName(Kst::ObjectTag(_tag, in_tag.context()));
  } else {
    Kst::Object::setTagName(suggestUniqueStringTag(in_tag));
  }

  Kst::stringList.lock().writeLock();
  Kst::stringList.append(this);
  Kst::stringList.lock().unlock();
}


KstString::KstString(QDomElement& e)
: KstPrimitive(), _orphan(false), _editable(false) {
  QDomNode n = e.firstChild();

  while (!n.isNull()) {
    QDomElement e = n.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "tag") {
        setTagName(Kst::ObjectTag::fromString(e.text()));
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
  Kst::stringList.append(this);
}


KstString::~KstString() {
}


void KstString::setTagName(const Kst::ObjectTag& tag) {
  if (tag == this->tag()) {
    return;
  }

  KstWriteLocker l(&Kst::stringList.lock());

  Kst::stringList.doRename(this, tag);
}


void KstString::save(QXmlStreamWriter &s) {
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


Kst::Object::UpdateType KstString::update(int updateCounter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  if (Kst::Object::checkUpdateCounter(updateCounter) && !force) {
    return lastUpdateResult();
  }

  QString v = value();
  if (_provider) {
    _provider->update(updateCounter);
  }
  
  return setLastUpdateResult(v == value() ? NO_CHANGE : UPDATE);
}


KstString& KstString::operator=(const QString& v) {
  setValue(v);
  return *this;
}


KstString& KstString::operator=(const char *v) {
  setValue(v);
  return *this;
}


void KstString::setValue(const QString& inV) {
  setDirty();
  _value = inV;
  emit trigger();
}

// vim: ts=2 sw=2 et
