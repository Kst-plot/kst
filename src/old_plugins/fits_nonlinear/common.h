/***************************************************************************
                          common.h  -  description
                             -------------------
    begin                : Thu Aug 12 2004
    copyright            : (C) 2004 The University of British Columbia
  ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define KST_UNUSED(x) if(x){};

#define X           0
#define Y           1
#define WEIGHT      2

double interpolate(int iIndex, int iLengthDesired, const double* pArray, int iLengthActual) {
  double value;
  double fj;
  double fdj;
  int j;
  
  if (iLengthDesired == iLengthActual) {
    value =  pArray[iIndex];
  } else {
    fj    = (double)(iIndex * (iLengthActual-1)) / (double)(iLengthDesired-1);
    j     = (int)floor(fj);
    fdj   = fj - (double)j;

    //Don't read an invalid index from pArray... found by valgrind
    double A = j+1 < iLengthActual ? pArray[j+1] : 0;
    double B = j < iLengthActual ? pArray[j] : 0;

    value = A * fdj + B * (1.0 - fdj);
  }
  
  return value;
}

