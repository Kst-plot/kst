/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2005 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "dateparser.h"
#include <qstringlist.h>

#include <assert.h>
#include <math.h>
#include <stdlib.h>

namespace Kst {

QDateTime millisecondsToQDateTime(double ms) {
  QDateTime edt;
  edt.setTime_t(0);
  if (ms > 0.0) {
    double milli = fmod(ms, 1000.0);
    ms = (ms - milli) / 1000.0;
    assert(ms < 60*365*24*60*60); // we can't handle big dates yet
    // this will have to change when we do
    edt.setTime_t(int(ms));
    QTime t = edt.time();
    t.setHMS(t.hour(), t.minute(), t.second(), int(milli));
    edt.setTime(t);
  } if (ms < 0.0) {
    abort(); // unhandled at this point
  }
  return edt;
}


double extDateTimeToMilliseconds(const QDateTime& edt) {
  double rc = 0.0;
  if (edt.isNull()) {
    return rc;
  }

  int year = edt.date().year();
  if (year > 1969 && year < 2030) { // fix later
    rc = 1000.0 * edt.toTime_t() + edt.time().msec();
  } else {
    // Manually construct rc
    abort();
  }
  return rc;
}


QDateTime parsePlanckDate(const QString& dateString) {
  QStringList secondSplit = dateString.split('.');
  if (secondSplit.isEmpty() || secondSplit.count() > 2) {
    return QDateTime();
  }

  int seconds = 0;
  if (secondSplit.count() > 1) {
    seconds = secondSplit[1].toUInt();
  }

  QStringList mainSplit = secondSplit[0].split(':');
  QDateTime edt = QDateTime::currentDateTime();
  int offset = QDateTime::currentDateTime().toUTC().toTime_t() - edt.toTime_t();
  QDate d = edt.date();
  QTime t = edt.time();
  int i = 0;
  switch (mainSplit.count()) {
    default:
      return QDateTime();
    case 5:
      {
        int years = mainSplit[i++].toInt();
        if (years < 100) {
          if (years < 0) {
            years = 1970 - years;
          } else {
            years += 2000;
          }
        }
        d.setDate(years, d.month(), d.day());
      }
    case 4:
      {
        unsigned month = mainSplit[i++].toUInt();
        d.setDate(d.year(), month, d.day());
      }
    case 3:
      {
        unsigned day = mainSplit[i++].toInt();
        d.setDate(d.year(), d.month(), day);
      }
      edt.setDate(d);
    case 2:
      {
        unsigned hour = mainSplit[i++].toInt();
        t.setHMS(hour, t.minute(), t.second());
      }
    case 1:
      {
        unsigned minute = mainSplit[i].toInt();
        t.setHMS(t.hour(), minute, t.second());
      }
    case 0:
      t.setHMS(t.hour(), t.minute(), seconds);
      edt.setTime(t);
      break;
  }
  return edt.addSecs(-offset);
}

}

// vim: ts=2 sw=2 et
