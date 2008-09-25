/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
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
#include <vector.h>

#define XVALUES 0
#define YVALUES 1
#define WEIGHTS 2

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


void assign( Kst::VectorPtr targetVector, double* pResult[], int iIndex, int iLength ) {
  for (int i = 0; i < iLength; i++) {
    targetVector->value()[i] = pResult[iIndex][i];
  }
}


bool precursor( Kst::VectorPtr xVector, Kst::VectorPtr yVector, Kst::VectorPtr weightsVector,
                int *piLength, bool bWeighted, bool bLowHigh, int iNumParams,
                double* pInputs[], Kst::VectorPtr vectorOutYFitted, Kst::VectorPtr vectorOutYResiduals,
                Kst::VectorPtr vectorOutYParameters, Kst::VectorPtr vectorOutYCovariance, Kst::VectorPtr vectorOutYLo,
                Kst::VectorPtr vectorOutYHi) {

  bool bRetVal = false;
  int  iNumCovar = ( iNumParams * ( iNumParams + 1 ) ) / 2;
  int  i;

  pInputs[XVALUES] = 0L;
  pInputs[YVALUES] = 0L;
  if (bWeighted) {
    pInputs[WEIGHTS] = 0L;
  }

  if( xVector->length() >= 2 &&
      yVector->length() >= 2 &&
      (!bWeighted || weightsVector->length() >= 2) ) {
        *piLength = xVector->length();
        if(  yVector->length() > *piLength ) {
          *piLength =  yVector->length();
        }

    //
    // do any necessary interpolations...
    //
        pInputs[XVALUES] = (double*)malloc(*piLength * sizeof( double ));
        if (xVector->length() == *piLength) {
          for( i=0; i<*piLength; i++) {
            pInputs[XVALUES][i] = xVector->value()[i];
          }
        } else {
          for( i=0; i<*piLength; i++) {
            pInputs[XVALUES][i] = interpolate( i, *piLength, xVector->value(), xVector->length() );
          }
        }

        pInputs[YVALUES] = (double*)malloc(*piLength * sizeof( double ));
        if (yVector->length() == *piLength) {
          for( i=0; i<*piLength; i++) {
            pInputs[YVALUES][i] = yVector->value()[i];
          }
        } else {
          for( i=0; i<*piLength; i++) {
            pInputs[YVALUES][i] = interpolate( i, *piLength, yVector->value(), yVector->length() );
          }
        }

        if (bWeighted) {
          pInputs[WEIGHTS] = (double*)malloc(*piLength * sizeof( double ));
          if (weightsVector->length() == *piLength) {
            for( i=0; i<*piLength; i++) {
              pInputs[WEIGHTS][i] = weightsVector->value()[i];
            }
          } else {
            for( i=0; i<*piLength; i++) {
              pInputs[WEIGHTS][i] = interpolate( i, *piLength, weightsVector->value(), weightsVector->length() );
            }
          }
        }

    if( *piLength > iNumParams + 1 ) {
      vectorOutYFitted->resize(*piLength);
      vectorOutYResiduals->resize(*piLength);
      vectorOutYParameters->resize(iNumParams);
      vectorOutYCovariance->resize(iNumCovar);
      if( bLowHigh ) {
        vectorOutYLo->resize(*piLength);
        vectorOutYHi->resize(*piLength);
      }

      bRetVal = true;
    }
  }
  return bRetVal;
}


void postcursor( bool bWeighted, double* pInputs[]) {
  if (pInputs[XVALUES] != 0L ) {
    free( pInputs[XVALUES] );
  }

  if (pInputs[YVALUES] != 0L ) {
    free( pInputs[YVALUES] );
  }

  if( bWeighted ) {
    if (pInputs[WEIGHTS] != 0L ) {
      free( pInputs[WEIGHTS] );
    }
  }
}
