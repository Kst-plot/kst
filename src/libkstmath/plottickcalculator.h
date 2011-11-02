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

#ifndef PLOTTICKCALCULATOR_H
#define PLOTTICKCALCULATOR_H

#include "kstmath_export.h"

namespace Kst {

enum MajorTickMode {
  TicksNone = 0,
  TicksCoarse = 2,
  TicksNormal = 5,
  TicksFine = 10,
  TicksVeryFine = 15
};

enum timeUnits {Hour, Minute, Second};

KSTMATH_EXPORT void computeMajorTickSpacing(double *major_spacing, int *minor_count, MajorTickMode majorTickCount, double range);
KSTMATH_EXPORT void computeMajorTickSpacing(double *major_spacing, int *minor_count, MajorTickMode majorTickCount, double range, timeUnits time_units);

}

#endif // PLOTTICKCALCULATOR_H
