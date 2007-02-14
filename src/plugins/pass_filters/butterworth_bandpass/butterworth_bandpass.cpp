/*
 *  Butterworth band pass filter plugin for KST.
 *  Copyright 2004, The University of British Columbia
 *  Released under the terms of the GPL.
 */

#include <stdlib.h>
#include <math.h>
#include "../filters.h"
#include <gsl/gsl_pow_int.h>

extern "C" int filter_bandpass(const double *const inArrays[], const int inArrayLens[],
		const double inScalars[],
		double *outArrays[], int outArrayLens[],
		double outScalars[]);

int filter_bandpass(const double *const inArrays[], const int inArrayLens[],
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

  int order2 = int(2.0 * inScalars[0]);
  double lowpass  = inScalars[1] + 0.5*inScalars[2];
  double highpass = inScalars[1] - 0.5*inScalars[2];

  if( dFreqValue > 0.0 ) {
    dValue = 1.0 / ( 1.0 + gsl_pow_int( dFreqValue / lowpass, order2) );
    dValue *= 1.0 / ( 1.0 + gsl_pow_int( highpass / dFreqValue, order2) );
  } else {
    dValue = 0.0;
  }

  return dValue;
}
