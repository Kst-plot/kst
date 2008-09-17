/*
 *  Sinusoid unweighted fitting plugin for KST.
 *  Copyright 2004, The University of British Columbia
 *  Released under the terms of the GPL.
 */

#include "../linear.h"

double g_dPeriod;

double calculate_matrix_entry( double dX, int iPos ) {
  double dY;
  
  if( iPos == 0 )
  {
    dY = 1.0;
  }
  else if( iPos % 2 == 1 )
  {
    dY =  cos( (double)( ( iPos + 1 ) / 2 ) * 2.0 * M_PI * dX / g_dPeriod );
  }
  else
  {
    dY = -sin( (double)( ( iPos + 0 ) / 2 ) * 2.0 * M_PI * dX / g_dPeriod );
  }
  
  return dY;
}

extern "C" int parameterName(int iIndex, char** pName);
extern "C" int kstfit_sinusoid_unweighted(
  const double *const inArrays[], 
  const int inArrayLens[],
  const double inScalars[],
  double *outArrays[], int outArrayLens[],
  double outScalars[]);

int kstfit_sinusoid_unweighted(
  const double *const inArrays[], 
  const int inArrayLens[],
	const double inScalars[],
	double *outArrays[], int outArrayLens[],
	double outScalars[])
{
  int iRetVal = -1;
  int iNumParams;
  
  iNumParams = 3 + ( 2 * (int)floor( inScalars[0] ) );
  if( iNumParams >= 3 ) {
    g_dPeriod = inScalars[1];
    if( g_dPeriod > 0.0 )
    {
      iRetVal = kstfit_linear_unweighted( inArrays, inArrayLens, 
                                        outArrays, outArrayLens, 
                                        outScalars, iNumParams );
    }
  }
  
  return iRetVal;
}

int parameterName(int iIndex, char** pName) {
  char name[50];

  if( iIndex == 0 ) {
    *pName = strdup( "Mean" );
  } else if (iIndex % 2 == 1 ) {
    sprintf( name, " cos(%d 2PI x/P)", ( iIndex + 1 ) / 2 );
    *pName = strdup( name );
  } else {
    sprintf( name, "-sin(%d 2PI x/P)", ( iIndex + 0 ) / 2 );
    *pName = strdup( name );
  }

  return 1;
}
