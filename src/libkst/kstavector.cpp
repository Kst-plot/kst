/***************************************************************************
                          kstavector.cpp - a vector with editable points.
                             -------------------
    begin                : april, 2005
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
// use KCodecs::base64Encode() in kmdcodecs.h
// Create QDataStream into a QByteArray
// qCompress the bytearray

#include "kstavector.h"
#include "kstdebug.h"
#include <qtextstream.h>

KstAVector::KstAVector(const QDomElement &e)
: KstVector(e) {
  _editable = true;
  _saveable = true;
  _saveData = true;
}


KstAVector::KstAVector(int n, KstObjectTag tag)
: KstVector(tag, n) {
  _editable = true;
  _saveable = true;
  _saveData = true;
}


void KstAVector::save(QTextStream &ts, const QString& indent, bool saveAbsolutePosition) {
  ts << indent << "<avector>" << endl;
  KstVector::save(ts, indent + "  ", saveAbsolutePosition);
  ts << indent << "</avector>" << endl;
}


KstObject::UpdateType KstAVector::update(int update_counter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();

  if (KstObject::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  KstObject::UpdateType baseRC = KstVector::update(update_counter);
  if (force) {
    baseRC = UPDATE;
  }

  return baseRC;
}


void KstAVector::setSaveData(bool save) {
  Q_UNUSED(save)
}

// vim: ts=2 sw=2 et
