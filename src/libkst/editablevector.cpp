/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *   copyright : (C) 2005  University of British Columbia                        *
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

#include "editablevector.h"
#include "kstdebug.h"
#include <qtextstream.h>

namespace Kst {

EditableVector::EditableVector(const QString &tag, const QByteArray &data)
: Vector(tag, data) {
  _editable = true;
  _saveable = true;
  _saveData = true;
}


EditableVector::EditableVector(int n, ObjectTag tag)
: Vector(tag, n) {
  _editable = true;
  _saveable = true;
  _saveData = true;
}


Object::UpdateType EditableVector::update(int update_counter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();

  if (Object::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  Object::UpdateType baseRC = Vector::update(update_counter);
  if (force) {
    baseRC = UPDATE;
  }

  return baseRC;
}


void EditableVector::setSaveData(bool save) {
  Q_UNUSED(save)
}

}
// vim: ts=2 sw=2 et
