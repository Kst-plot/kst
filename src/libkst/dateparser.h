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

#ifndef DATEPARSER_H
#define DATEPARSER_H

#include <QTime>
#include <QDate>
#include <QDateTime>
#include "kst_export.h"

/* This is a public header */
namespace Kst {
  /*
  All times represented as doubles are the number of milliseconds since Jan 01
  1970 00:00:00.  Negative times are before then.
  */
  KSTCORE_EXPORT extern QDateTime parsePlanckDate(const QString& dateString);
  KSTCORE_EXPORT extern double extDateTimeToMilliseconds(const QDateTime& edt);
  KSTCORE_EXPORT extern QDateTime millisecondsToQDateTime(double ms);
}

#endif
// vim: ts=2 sw=2 et
