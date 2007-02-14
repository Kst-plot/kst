/***************************************************************************
                          common.h  -  description
                             -------------------
    begin                : Mon Apr 26 2004
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

#define XVALUES 0
#define YVALUES 1
#define WEIGHTS 2

#define YFIT        0
#define YRESIDUALS  1
#define PARAMETERS  2
#define COVARIANCE  3
#define Y_LOW_VALS  4
#define Y_HGH_VALS  5
#define MAX_OUT     6

#define CHI2NU      0

#define KST_UNUSED(x) if(x){};

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

void assign( double* outArrays[], int outArrayLens[], double* pResult[], int iIndex, int iLength ) {
  outArrays[iIndex]    = pResult[iIndex];
  outArrayLens[iIndex] = iLength;
}

void alloc( double* outArrays[], int outArrayLens[], double* pResult[], int iIndex, int iLength ) {
  if( outArrayLens[iIndex] != iLength ) {
    pResult[iIndex] = (double*)realloc( outArrays[iIndex], iLength * sizeof( double ) );
  } else {
    pResult[iIndex] = outArrays[iIndex];
  }
}

bool precursor( const double *const inArrays[],
                const int inArrayLens[],
                double* outArrays[],
                int outArrayLens[],
                int*    piLength,
                bool    bWeighted,
                bool    bLowHigh,
                int     iNumParams,
                double* pInputs[]) {
  double* pResult[MAX_OUT];
  bool bRetVal = false;
  int  iNumCovar = ( iNumParams * ( iNumParams + 1 ) ) / 2;
  int  i;

  pInputs[XVALUES] = 0L;
  pInputs[YVALUES] = 0L;
  pInputs[WEIGHTS] = 0L;

  if( inArrayLens[XVALUES] >= 2 &&
      inArrayLens[YVALUES] >= 2 &&
      (!bWeighted || inArrayLens[WEIGHTS] >= 2) ) {
    *piLength = inArrayLens[XVALUES];
    if( inArrayLens[YVALUES] > *piLength ) {
      *piLength = inArrayLens[YVALUES];
    }

    //
    // do any necessary interpolations...
    //
    if (inArrayLens[XVALUES] == *piLength) {
      pInputs[XVALUES] = (double*)inArrays[XVALUES];
    } else {
      pInputs[XVALUES] = (double*)malloc(*piLength * sizeof( double ));
      for( i=0; i<*piLength; i++) {
        pInputs[XVALUES][i] = interpolate( i, *piLength, inArrays[XVALUES], inArrayLens[XVALUES] );
      }
    }

    if (inArrayLens[YVALUES] == *piLength) {
      pInputs[YVALUES] = (double*)inArrays[YVALUES];
    } else {
      pInputs[YVALUES] = (double*)malloc(*piLength * sizeof( double ));
      for( i=0; i<*piLength; i++) {
        pInputs[YVALUES][i] = interpolate( i, *piLength, inArrays[YVALUES], inArrayLens[YVALUES] );
      }
    }

    if (bWeighted) {
      if (inArrayLens[WEIGHTS] == *piLength) {
        pInputs[WEIGHTS] = (double*)inArrays[WEIGHTS];
      } else {
        pInputs[WEIGHTS] = (double*)malloc(*piLength * sizeof( double ));
        for( i=0; i<*piLength; i++) {
          pInputs[WEIGHTS][i] = interpolate( i, *piLength, inArrays[WEIGHTS], inArrayLens[WEIGHTS] );
        }
      }
    }

    if( *piLength > iNumParams + 1 ) {
      alloc( outArrays, outArrayLens, pResult, YFIT, *piLength );
      alloc( outArrays, outArrayLens, pResult, YRESIDUALS, *piLength );
      alloc( outArrays, outArrayLens, pResult, PARAMETERS, iNumParams );
      alloc( outArrays, outArrayLens, pResult, COVARIANCE, iNumCovar );
      if( bLowHigh ) {
        alloc( outArrays, outArrayLens, pResult, Y_LOW_VALS, *piLength );
        alloc( outArrays, outArrayLens, pResult, Y_HGH_VALS, *piLength );
      }

      if( pResult[YFIT] != NULL &&
          pResult[YRESIDUALS] != NULL &&
          pResult[PARAMETERS] != NULL &&
          pResult[COVARIANCE] != NULL &&
          ( !bLowHigh || pResult[Y_LOW_VALS] != NULL ) &&
          ( !bLowHigh || pResult[Y_HGH_VALS] != NULL ) )
      {
        assign( outArrays, outArrayLens, pResult, YFIT, *piLength );
        assign( outArrays, outArrayLens, pResult, YRESIDUALS, *piLength );
        assign( outArrays, outArrayLens, pResult, PARAMETERS, iNumParams );
        assign( outArrays, outArrayLens, pResult, COVARIANCE, iNumCovar );
        if( bLowHigh ) {
          assign( outArrays, outArrayLens, pResult, Y_LOW_VALS, *piLength );
          assign( outArrays, outArrayLens, pResult, Y_HGH_VALS, *piLength );
        }

        bRetVal = true;
      }
    }
  }

  return bRetVal;
}

void postcursor( const double *const inArrays[],
                bool bWeighted,
                double* pInputs[]) {
  if (pInputs[XVALUES] != (double*)inArrays[XVALUES] &&
      pInputs[XVALUES] != 0L ) {
    free( pInputs[XVALUES] );
  }

  if (pInputs[YVALUES] != (double*)inArrays[YVALUES] &&
      pInputs[YVALUES] != 0L ) {
    free( pInputs[YVALUES] );
  }

  if( bWeighted ) {
    if (pInputs[WEIGHTS] != (double*)inArrays[WEIGHTS] &&
        pInputs[WEIGHTS] != 0L ) {
      free( pInputs[WEIGHTS] );
    }
  }
}
