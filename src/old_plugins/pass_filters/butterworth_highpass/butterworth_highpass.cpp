/*
 *  Butterworth high pass filter plugin for KST.
 *  Copyright 2004, The University of British Columbia
 *  Released under the terms of the GPL.
 */

#include <stdlib.h>
#include <math.h>
#include "../filters.h"

extern "C" int filter_highpass(const double *const inArrays[], const int inArrayLens[],
		const double inScalars[],
		double *outArrays[], int outArrayLens[],
		double outScalars[]);

int filter_highpass(const double *const inArrays[], const int inArrayLens[],
		const double inScalars[],
		double *outArrays[], int outArrayLens[],
		double outScalars[])
{
  int iReturn;
  
  iReturn = kst_pass_filter( inArrays, 
                             inArrayLens, 
                             inScalars, 
                             outArrays, 
                             outArrayLens,
                             outScalars );
  
  return iReturn;
}

double filter_calculate( double dFreqValue, const double inScalars[] ) {
  double dValue;
  
  if( dFreqValue > 0.0 ) {
    dValue = 1.0 / ( 1.0 + pow( inScalars[1] / dFreqValue, 2.0 * (double)inScalars[0] ) );
  } else {
    dValue = 0.0;
  }
  
  return dValue;
}
