/***************************************************************************
                          scalar.cpp  -  the base scalar type
                             -------------------
    begin                : March 24, 2003
    copyright            : (C) 2003 by cbn
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

#include "scalar.h"

#include "kst_i18n.h"

#include "datacollection.h"
#include "defaultprimitivenames.h"

#include <qdebug.h>
#include <qtextdocument.h>
#include <QXmlStreamWriter>


namespace Kst {

static int iAnonymousScalarCounter = 0;

static bool dirtyScalars = false;

bool Scalar::scalarsDirty() {
  // Should use a mutex, but let's play with fire to be fast
  return dirtyScalars;
}


void Scalar::clearScalarsDirty() {
  // Should use a mutex, but let's play with fire to be fast
  dirtyScalars = false;
}

/** Create the base scalar */
Scalar::Scalar(KstObjectTag in_tag, KstObject *provider, double val, bool orphan, bool displayable, bool editable)
: KstPrimitive(provider), _value(val), _orphan(orphan), _displayable(displayable), _editable(editable) {
  QString _tag = in_tag.tag();
  if (_tag.isEmpty()) {
    do {
      _tag = i18n("Anonymous Scalar %1", iAnonymousScalarCounter++);
    } while (Data::self()->vectorTagNameNotUniqueInternal(_tag));  // FIXME: why vector?
    KstObject::setTagName(KstObjectTag(_tag, in_tag.context()));
  } else {
    KstObject::setTagName(KST::suggestUniqueScalarTag(in_tag));
  }


  scalarList.lock().writeLock();
  scalarList.append(this);
  scalarList.lock().unlock();
}


Scalar::Scalar(const QDomElement& e)
: KstPrimitive(), _orphan(false), _displayable(true), _editable(false) {
  QDomNode n = e.firstChild();
  bool ok;

  _value = 0.0;  // must init this first

  while (!n.isNull()) {
    QDomElement e = n.toElement();
    if(!e.isNull()) {
      if (e.tagName() == "tag") {
        setTagName(KstObjectTag::fromString(e.text()));
      } else if (e.tagName() == "orphan") {
        _orphan = true;
      } else if (e.tagName() == "value") {
        setValue(e.text().toDouble());
      } else if (e.tagName() == "editable") {
        _editable = true;
      }
    }
    n = n.nextSibling();
  }

  if (tagName().toDouble(&ok) == value() && ok) {
    _displayable = false;
  }

  scalarList.append(this);
}


Scalar::~Scalar() {
}


KstObject::UpdateType Scalar::update(int updateCounter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  if (KstObject::checkUpdateCounter(updateCounter) && !force) {
    return lastUpdateResult();
  }

  double v = value();
  if (_provider) {
    KstWriteLocker pl(_provider);
    _provider->update(updateCounter);
  } else if (force) {
    return setLastUpdateResult(UPDATE);
  }

  return setLastUpdateResult(v == value() ? NO_CHANGE : UPDATE);
}


void Scalar::save(QXmlStreamWriter &s) {
  if (provider()) {
    return;
  }
  s.writeStartElement("scalar");
  s.writeAttribute("tag", tag().tagString());
  if (_orphan) {
    s.writeAttribute("orphan", "true");
  }
  if (_editable) {
    s.writeAttribute("editable", "true");
  }
  s.writeAttribute("value", QString::number(value()));
  s.writeEndElement();
}


Scalar& Scalar::operator=(double v) {
  setValue(v);
  return *this;
}


void Scalar::setValue(double inV) {
  if (_value != inV) {
    setDirty();
    dirtyScalars = true;
    _value = inV;
    emit trigger();
  }
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


void Scalar::setTagName(const KstObjectTag& newTag) {
  if (newTag == tag()) {
    return;
  }

  KstWriteLocker l(&scalarList.lock());

  scalarList.doRename(this, newTag);
}

}
// vim: et ts=2 sw=2
