/***************************************************************************
                          kstscalar.cpp  -  the base scalar type
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

#include <qtextdocument.h>

#include <klocale.h>

#include "kstscalar.h"
#include "kstdatacollection.h"
#include "defaultprimitivenames.h"
#include <qdebug.h>


static int iAnonymousScalarCounter = 0;

static bool dirtyScalars = false;

bool KstScalar::scalarsDirty() {
  // Should use a mutex, but let's play with fire to be fast
  return dirtyScalars;
}


void KstScalar::clearScalarsDirty() {
  // Should use a mutex, but let's play with fire to be fast
  dirtyScalars = false;
}

/** Create the base scalar */
KstScalar::KstScalar(KstObjectTag in_tag, KstObject *provider, double val, bool orphan, bool displayable, bool editable)
: KstPrimitive(provider), _value(val), _orphan(orphan), _displayable(displayable), _editable(editable) {
  QString _tag = in_tag.tag();
  if (_tag.isEmpty()) {
    do {
      _tag = i18n("Anonymous Scalar %1", iAnonymousScalarCounter++);
    } while (KstData::self()->vectorTagNameNotUniqueInternal(_tag));  // FIXME: why vector?
    KstObject::setTagName(KstObjectTag(_tag, in_tag.context()));
  } else {
    KstObject::setTagName(KST::suggestUniqueScalarTag(in_tag));
  }


  KST::scalarList.lock().writeLock();
  KST::scalarList.append(this);
  KST::scalarList.lock().unlock();
}


KstScalar::KstScalar(const QDomElement& e)
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

  KST::scalarList.append(this);
}


KstScalar::~KstScalar() {
}


KstObject::UpdateType KstScalar::update(int updateCounter) {
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


void KstScalar::save(QTextStream &ts, const QString& indent) {
  ts << indent << "<tag>" << Qt::escape(tag().tagString()) << "</tag>" << endl;
  if (_orphan) {
    ts << indent << "<orphan/>" << endl;
  }
  if (_editable) {
    ts << indent << "<editable/>" << endl;
  }
  ts << indent << "<value>" << value() << "</value>" << endl;
}


KstScalar& KstScalar::operator=(double v) {
  setValue(v);
  return *this;
}


void KstScalar::setValue(double inV) {
  if (_value != inV) {
    setDirty();
    dirtyScalars = true;
    _value = inV;
    emit trigger();
  }
}


QString KstScalar::label() const {
  return QString::number(_value);
}


double KstScalar::value() const {
  return _value;
}


bool KstScalar::orphan() const {
  return _orphan;
}


void KstScalar::setOrphan(bool orphan) {
  _orphan = orphan;
}


bool KstScalar::displayable() const {
  return _displayable;
}


void KstScalar::setDisplayable(bool displayable) {
  _displayable = displayable;
}


bool KstScalar::editable() const {
  return _editable;
}


void KstScalar::setEditable(bool editable) {
  _editable = editable;
}


void KstScalar::setTagName(const KstObjectTag& newTag) {
  if (newTag == tag()) {
    return;
  }

  KstWriteLocker l(&KST::scalarList.lock());

  KST::scalarList.doRename(this, newTag);
}

#include "kstscalar.moc"
// vim: et ts=2 sw=2
