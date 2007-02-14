/***************************************************************************
                               kstdateparser.h
                             -------------------
    begin                : Nov 07, 2005
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

#ifndef KSTDATEPARSER_H
#define KSTDATEPARSER_H

#include <kstextdatetime.h>
#include "kst_export.h"

/* This is a public header */
namespace KST {
  /*
  All times represented as doubles are the number of milliseconds since Jan 01
  1970 00:00:00.  Negative times are before then.
  */
  KST_EXPORT extern ExtDateTime parsePlanckDate(const QString& dateString);
  KST_EXPORT extern double extDateTimeToMilliseconds(const ExtDateTime& edt);
  KST_EXPORT extern ExtDateTime millisecondsToExtDateTime(double ms);
}

#endif
// vim: ts=2 sw=2 et
