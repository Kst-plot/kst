/***************************************************************************
                         planck.cpp  -  Part of KST
                             -------------------
    begin                : Mon Oct 06 2003
    copyright            : (C) 2003 The University of Toronto
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


#include "planckdata.h"

using namespace Planck;


namespace Planck {
bool havePlanck() {
  return true;
}

bool validDatabase(const QString& db) {
  PIOGroup *g = PIOOpenVoidGrp(const_cast<char*>(db.latin1()), const_cast<char*>("r"));
  if (g) {
    PIOCloseVoidGrp(&g);
    return true;
  }
  return false;
}
}

Source::Source() : KstShared() {
  _isValid = false;
}


Source::~Source() {
}


bool Source::setGroup(const QString& group) {
  Q_UNUSED(group)
  return false;
}


void Source::reset() {
  _isValid = false;
}


bool Source::isValid() const {
  return _isValid;
}


QStringList Source::fields() const {
  return QStringList();
}


int Source::readObject(const QString& object, double *buf, long start, long end) {
  Q_UNUSED(object)
  Q_UNUSED(buf)
  Q_UNUSED(start)
  Q_UNUSED(end)
  return 0;
}


bool Source::updated() const {
  return false;
}


QSize Source::range(const QString& object) const {
  Q_UNUSED(object)
  return QSize();
}

// vim: ts=2 sw=2 et
