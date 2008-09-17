/*
 *  Exponential weighted fit plugin for KST.
 *  Copyright 2004, The University of British Columbia
 *  Released under the terms of the GPL.
 */

#define NUM_PARAMS 3
#define MAX_NUM_ITERATIONS 500

#include "../non_linear_weighted.h"

void function_initial_estimate( const double* pdX, const double* pdY, int iLength, double* pdParameterEstimates ) {  
  KST_UNUSED( pdX )
  KST_UNUSED( pdY )
  KST_UNUSED( iLength )
  
  pdParameterEstimates[0] =  1.0;
  pdParameterEstimates[1] =  0.0;
  pdParameterEstimates[2] =  0.0;
}

double function_calculate( double dX, double* pdParameters ) {
  double dScale	 = pdParameters[0];
  double dLambda = pdParameters[1];
  double dOffset = pdParameters[2];
  double dY;
  
  dY  = ( dScale * exp( -dLambda * dX ) ) + dOffset;
 
  return dY;
}

void function_derivative( double dX, double* pdParameters, double* pdDerivatives ) {
  double dScale	 = pdParameters[0];
  double dLambda = pdParameters[1];
  double dExp;  
  double ddScale;
  double ddLambda;
  double ddOffset;
  
  dExp     = exp( -dLambda * dX );
  ddScale  = dExp;
  ddLambda = -dX * dScale * dExp;
  ddOffset = 1.0;

  pdDerivatives[0] = ddScale;
  pdDerivatives[1] = ddLambda;
  pdDerivatives[2] = ddOffset;
}

extern "C" int parameterName(int iIndex, char** pName);
extern "C" int kstfit_exponential_weighted(const double *const inArrays[], const int inArrayLens[],
		const double inScalars[],
		double *outArrays[], int outArrayLens[],
		double outScalars[]);

int kstfit_exponential_weighted(const double *const inArrays[], const int inArrayLens[],
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
      *pName = strdup("Scale");
      iRetVal = 1;
      break;
    case 1:
      *pName = strdup("Lambda");
      iRetVal = 1;
      break;
    case 2:
      *pName = strdup("Offset");
      iRetVal = 1;
      break;
  }

  return iRetVal;
}

