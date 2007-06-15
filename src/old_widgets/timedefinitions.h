/***************************************************************************
                             timdefinitions.h
                             -------------------
    begin                : Jan 20, 2005
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

#ifndef TIMEDEFINITIONS_H
#define TIMEDEFINITIONS_H

// Don't include this anywhere except datarangewidget.ui

#include <klocale.h>

struct KstTimeDef {
  const char *name;
  const char *description;
  double factor;
};

namespace KST {
  const int dateTimeEntry = 1;
  const KstTimeDef timeDefinitions[] = {
    { I18N_NOOP("frames"), "frames", 1.0 },
    { I18N_NOOP("date"), "date", 1.0 },
    { I18N_NOOP("ms"), "milliseconds", 1.0 },
    { I18N_NOOP("s"), "seconds", 1000.0 },
    { I18N_NOOP("m"), "minutes", 60000.0 },
    { I18N_NOOP("h"), "hours", 3600000.0 },
    { I18N_NOOP("days"), "days", 86400000.0 },
    { I18N_NOOP("weeks"), "weeks", 604800000.0 },
    { I18N_NOOP("months"), "months", 2628000000.0 }, // 1/12 of a year
    { I18N_NOOP("years"), "years", 31536000000.0 },  // 365 days
    { 0, 0, 0 }
  };
}

#endif
// vim: ts=2 sw=2 et
