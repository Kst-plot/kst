/***************************************************************************
                              js_dataobject.cpp
                             -------------------
    begin                : Apr 23 2005
    copyright            : (C) 2005 The University of Toronto
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

#include "js_dataobject.h"

#include <qstylesheet.h>

#include <kdebug.h>
#include <klocale.h>

#include <kstdebug.h>


JSDataObject::JSDataObject()
: KstDataObject() {
}


JSDataObject::JSDataObject(const QDomElement &e)
: KstDataObject(e) {
  // FIXME
}


JSDataObject::~JSDataObject() {
}


KstObject::UpdateType JSDataObject::update(int update_counter) {
  if (!isValid()) {
    return setLastUpdateResult(NO_CHANGE);
  }

  bool force = dirty();
  setDirty(false);

  if (KstObject::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  bool updated = false;
  // FIXME: happens in wrong thread
#if 0
  KJS::Completion res;
  if (_jsPart->execute(res, _script, KJS::Null())) {
    if (res.isValueCompletion()) {
      updated = res.value().toBoolean(_jsPart->globalExec());
    }
  }
#endif

  return setLastUpdateResult(updated ? UPDATE : NO_CHANGE);
}


void JSDataObject::save(QTextStream &ts, const QString& indent) {
}


bool JSDataObject::isValid() const {
  return true; // FIXME
}


QString JSDataObject::propertyString() const {
  if (!isValid()) {
    return i18n("Invalid script object.");
  }
  return _propertyString;
}


void JSDataObject::showNewDialog() {
  // FIXME
}


void JSDataObject::showEditDialog() {
  // FIXME
}


const KstCurveHintList* JSDataObject::curveHints() const {
  // FIXME
  return KstDataObject::curveHints();
}


const QString& JSDataObject::script() const {
  return _script;
}


void JSDataObject::setScript(const QString& script) {
  _script = script;
}


void JSDataObject::setProperty(const QString& property) {
  _propertyString = property;
}



// vim: ts=2 sw=2 et
