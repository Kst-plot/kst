/*
 *  Linear unweighted fitting plugin for KST.
 *  Copyright 2004, The University of British Columbia
 *  Released under the terms of the GPL.
 */

#include <gsl/gsl_fit.h>
#include "../common.h"

extern "C" int parameterName(int iIndex, char** pName);
extern "C" int kstfit_linear_unweighted(const double *const inArrays[], const int inArrayLens[],
		const double inScalars[],
		double *outArrays[], int outArrayLens[],
		double outScalars[]);

int kstfit_linear_unweighted(const double *const inArrays[], const int inArrayLens[],
		const double inScalars[],
		double *outArrays[], int outArrayLens[],
		double outScalars[])
{
  KST_UNUSED(inScalars)

  int i = 0;
  int	iLength;
  int iReturn = -1;
  double* pInputs[3];
  double c0 = 0.0;
  double c1 = 0.0;
  double cov00 = 0.0;
  double cov01 = 0.0;
  double cov11 = 0.0;
  double dSumSq = 0.0;
  double y;
  double yErr;

  if( precursor( inArrays, inArrayLens, outArrays, outArrayLens, &iLength, false, true, 2, pInputs ) ) {
    if( !gsl_fit_linear( pInputs[XVALUES], 1, pInputs[YVALUES], 1, iLength, &c0, &c1, &cov00, &cov01, &cov11, &dSumSq ) ) {
        
      for( i=0; i<iLength; i++ ) {
        gsl_fit_linear_est( pInputs[XVALUES][i], c0, c1, cov00, cov01, cov11, &y, &yErr );
        outArrays[YFIT][i] = y;
        outArrays[YRESIDUALS][i] = pInputs[YVALUES][i] - y;
        outArrays[Y_LOW_VALS][i] = y - yErr;
        outArrays[Y_HGH_VALS][i] = y + yErr;
      }
        
      outArrays[PARAMETERS][0] = c0;
      outArrays[PARAMETERS][1] = c1;
      outArrays[COVARIANCE][0] = cov00;
      outArrays[COVARIANCE][1] = cov01;
      outArrays[COVARIANCE][2] = cov11;

      outScalars[CHI2NU]  = dSumSq / ( (double)iLength - 2.0 );
        
      iReturn = 0;
    } 
  }

  postcursor( inArrays, false, pInputs );
  
  return iReturn;
}

int parameterName(int iIndex, char** pName) {
  int iRetVal = 0;

  switch (iIndex) {
    case 0:
      *pName = strdup("Intercept");
      iRetVal = 1;
      break;
    case 1:
      *pName = strdup("Gradient");
      iRetVal = 1;
      break;
  }

  return iRetVal;
}
