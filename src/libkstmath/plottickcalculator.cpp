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

#include "plottickcalculator.h"

#include <QtGlobal>
#include <math.h>
#include <cmath>

namespace Kst {

/*
 * Major ticks are always spaced by D = A*10^B where B is an integer,
 * and A is 1, 2 or 5. So: 1, 0.02, 50, 2000 are all possible major tick
 * spacings, but 30 is not.
 *
 * A and B are chosen so that there are as close as possible to M major ticks
 * on the axis (but at least 2). The value of M is set by the requested
 * MajorTickMode.
 */
void computeMajorTickSpacing(double *major_spacing, int *minor_count, MajorTickMode majorTickCount, double R) {
  qreal M = majorTickCount;
  qreal B = floor(log10(R/M));

  qreal d1 = 1 * pow(10, B); // tick spacing
  qreal d2 = 2 * pow(10, B);
  qreal d5 = 5 * pow(10, B);

  qreal r1 = d1 * M; // tick range
  qreal r2 = d2 * M;
  qreal r5 = d5 * M;

  qreal s1 = qAbs(r1 - R);
  qreal s2 = qAbs(r2 - R);
  qreal s5 = qAbs(r5 - R);

  *minor_count = 5;
  if (s1 <= s2 && s1 <= s5) {
    *major_spacing = d1;
  } else if (s2 <= s5) {
    if ((M == 2) && (r2 > R)) {
      *major_spacing = d1; // Minimum ticks not met using d2 using d1 instead
    } else {
      *minor_count = 4;
      *major_spacing = d2;
    }
  } else {
    if ((M == 2) && (r5 > R)) {
      *minor_count = 4;
      *major_spacing = d2; // Minimum ticks not met using d5 using d2 instead
    } else {
      *major_spacing = d5;
    }
  }
}

// compute the major ticks for hours (base = 24) or min/sec (base = 60)
void computeMajorTickSpacing(double *major_spacing, int *minor_count, MajorTickMode majorTickCount, double R, timeUnits time_units) {
  double base60list[] =     {1.0,2.0,5.0,10.0,20.0,30.0,60.0};
  int minorlist_minutes[] = {  6,  4,  5,   5,   4,   6,   4};
  int minorlist_seconds[] = {  5,  4,  5,   5,   4,   6,   6};
  int n60 = 7;
  double base24list[] =   {1.0,2.0,4.0,6.0,12.0,24.0};
  int minorlist_hours[] = {  4,  4,  4,  6,   6,   6};
  int n24 = 6;
  double *baselist;
  int *minorlist;
  double n;

  if (R <= majorTickCount) {
    computeMajorTickSpacing(major_spacing, minor_count, majorTickCount, R);
    return;
  }

  if (time_units == Hour) {
    baselist = base24list;
    minorlist = minorlist_hours;
    n = n24;
  } else if (time_units == Minute) {
    baselist = base60list;
    minorlist = minorlist_minutes;
    n = n60;
  } else if (time_units == Second) {
    baselist = base60list;
    minorlist = minorlist_seconds;
    n = n60;
  } else { // unknown base - use base 10;
    computeMajorTickSpacing(major_spacing, minor_count, majorTickCount, R);
    return;
  }

  int best_i=0;
  double best_err = 1E88;
  double err;
  for (int i=0; i<n; i++) {
    err = fabs(double(majorTickCount) - R/baselist[i]);
    if (err < best_err) {
      best_err = err;
      best_i = i;
    }
  }

  *minor_count = minorlist[best_i];
  *major_spacing = baselist[best_i];
}

}
