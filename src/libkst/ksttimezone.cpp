/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2011 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ksttimezone.h"


namespace Kst {

static QStringList _tzlist;

KstTimeZone::KstTimeZone(QString name): _tzName(name)
{
  setTZ(name);
}

bool KstTimeZone::setTZ(QString name) {
  bool ok = true;
  if (!recognised(name)) {
    ok = false;
    name = "GMT";
  }
  _supportsDST = true; // force recalculation
  _tzName = name;
  _gmtOffset = gmtOffset(0);
  return ok;
}

void KstTimeZone::initTZList() {
  _tzlist.append("GMT");
  for (double offset = 0.5; offset <= 12; offset+=0.5) {
    _tzlist.append(QString("GMT+%1").arg(offset));
  }
  for (double offset = -0.5; offset >=- 12; offset-=0.5) {
    _tzlist.append(QString("GMT%1").arg(offset));
  }
}

QStringList KstTimeZone::tzList() {
  if (_tzlist.length() == 0) {
    initTZList();
  }
  return _tzlist;
}

bool KstTimeZone::recognised(QString name) {
  if (_tzlist.length() == 0) {
    initTZList();
  }
  return (_tzlist.contains(name));
}

int KstTimeZone::gmtOffset(time_t t) {
  if (!_supportsDST) {
    return _gmtOffset;
  }
  _supportsDST = false;
  if (_tzName == "GMT") {
    _gmtOffset = 0;
  } else if (_tzName.startsWith("GMT")) {
    QString tmp = _tzName;
    bool ok;
    double hours = 0;
    hours = tmp.remove("GMT").toDouble(&ok);
    _gmtOffset = hours * 3600.0;
  } else {
    _gmtOffset = 0;
  }
  return _gmtOffset;
}

}
