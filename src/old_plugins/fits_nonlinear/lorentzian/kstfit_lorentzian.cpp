/*
 *  Lorentzian fit plugin for KST.
 *  Copyright 2004, The University of British Columbia
 *  Released under the terms of the GPL.
 */

#define NUM_PARAMS 3
#define MAX_NUM_ITERATIONS 500

#include "../non_linear.h"

void function_initial_estimate( const double* pdX, const double* pdY, int iLength, double* pdParameterEstimates ) {
  double dMin;
  double dMax;
  
  gsl_stats_minmax( &dMin, &dMax, pdX, 1, iLength );
  
  pdParameterEstimates[0] = gsl_stats_mean( pdX, 1, iLength );
  pdParameterEstimates[1] = ( dMax - dMin ) / 2.0;
  pdParameterEstimates[2] = gsl_stats_max( pdY, 1, iLength );
}

double function_calculate( double dX, double* pdParameters ) {
  double dMean  = pdParameters[0];
  double dHW		= pdParameters[1];
  double dScale = pdParameters[2];
  double dY;
  
  dY  = ( dScale / M_PI ) * ( dHW / 2.0 );
  dY /= ( ( dX - dMean ) * ( dX - dMean ) )+( ( dHW / 2.0 ) * ( dHW / 2.0 ) );
  
  return dY;
}

void function_derivative( double dX, double* pdParameters, double* pdDerivatives ) {
  double dMean	= pdParameters[0];
  double dHW	  = pdParameters[1];
  double dScale = pdParameters[2];
  double dDenom;  
  double ddMean;
  double ddHW;
  double ddScale;
  
  dDenom  = ( ( dX - dMean ) * ( dX - dMean ) ) + ( ( dHW / 2.0 ) * ( dHW / 2.0 ) );
  ddMean  = ( dScale / M_PI ) * dHW * ( dMean - dX ) / ( dDenom * dDenom );
  ddHW    = ( dScale / ( 2.0 * M_PI ) ) / ( dDenom * dDenom );
  ddHW   *= dDenom - ( dHW * dHW / 2.0 );
  ddScale = ( 1.0 / M_PI ) * ( dHW / 2.0 ) / dDenom;
                                                                                   
  pdDerivatives[0] = ddMean;
  pdDerivatives[1] = ddHW;
  pdDerivatives[2] = ddScale;
}

extern "C" int parameterName(int iIndex, char** pName);
extern "C" int kstfit_lorentzian(const double *const inArrays[], const int inArrayLens[],
		const double inScalars[],
		double *outArrays[], int outArrayLens[],
		double outScalars[]);

int kstfit_lorentzian(const double *const inArrays[], const int inArrayLens[],
		const double inScalars[],
		double *outArrays[], int outArrayLens[],
		double outScalars[])
{
  return kstfit_nonlinear( inArrays, inArrayLens, inScalars, outArrays, outArrayLens, outScalars );
}

int parameterName(int iIndex, char** pName) {
  int iRetVal = 0;

  switch (iIndex) {
    case 0:
      *pName = strdup("Mean");
      iRetVal = 1;
      break;
    case 1:
      *pName = strdup("Half-width");
      iRetVal = 1;
      break;
    case 2:
      *pName = strdup("Scale");
      iRetVal = 1;
      break;
  }

  return iRetVal;
}

