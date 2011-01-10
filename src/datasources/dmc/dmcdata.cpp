#if 0
/***************************************************************************
                         dmcdata.cpp  -  Part of KST
                             -------------------
    begin                : Wed July 4 2007
    copyright            : (C) 2007 The University of Toronto
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


#include "dmcdata.h"
#include "stdlib.h"

using namespace DMC;


namespace DMC {
bool haveDMC() {
  return true;
}

bool validDatabase(const QString& db) {
  if (getenv("PGUSER") == NULL || getenv("DBROOT") == NULL) {
    qDebug("DMC is missing environment variables");
    /* DMC will lock up if these environment variables are not set */
    return false;
  }

  PIOGroup *g = PIOOpenVoidGrp(const_cast<char*>(db.latin1()), const_cast<char*>("r"));
  if (g) {
    PIOCloseVoidGrp(&g);
    // the call PIOOpenVoidGrp open the database, 
    // the PIOCloseVoidGrp call do not close it
    // once one db is opened, 
    // any subsequent open call will fail unless it is the same database
    // lets close it
    int rtc;
    rtc = PIOQuitDB();
    if(rtc != 0) { 
      // closing failed, dont care
    }
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

#endif
