/*
 *  Polynomial weighted fitting plugin for KST.
 *  Copyright 2004, The University of British Columbia
 *  Released under the terms of the GPL.
 */

#include "../linear_weighted.h"

double calculate_matrix_entry( double dX, int iPos ) {
  double dY;
  
  dY = pow( dX, (double)iPos );
  
  return dY;
}

extern "C" int parameterName(int iIndex, char** pName);
extern "C" int kstfit_polynomial_weighted(
  const double *const inArrays[], 
  const int inArrayLens[],
  const double inScalars[],
  double *outArrays[], int outArrayLens[],
  double outScalars[]);

int kstfit_polynomial_weighted(
  const double *const inArrays[], 
  const int inArrayLens[],
	const double inScalars[],
	double *outArrays[], int outArrayLens[],
	double outScalars[])
{
  int iRetVal = -1;
  int iNumParams;
  
  iNumParams = 1 + (int)floor( inScalars[0] );
  if( iNumParams > 0 ) {
    iRetVal = kstfit_linear_weighted( inArrays, inArrayLens, 
                                        outArrays, outArrayLens, 
                                        outScalars, iNumParams );
  }
  
  return iRetVal;
}

int parameterName(int iIndex, char** pName) 
{
  char name[20];

  sprintf( name, "x^%d", iIndex );
  *pName = strdup( name );

  return 1;
}
