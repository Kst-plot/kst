/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *   copyright : (C) 2005  University of British Columbia                  *
 *                   dscott@phas.ubc.ca                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*
 *  Generic pass filter plugin for KST.
 *  Released under the terms of the GPL 
 */

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>

#include "vector.h"
#include "scalar.h"

double filter_calculate( double dFreqValue, Kst::ScalarList scalars );

bool kst_pass_filter(
  Kst::VectorPtr vector,
  Kst::ScalarList scalars,
  Kst::VectorPtr outVector) {

  gsl_fft_real_wavetable* real;
  gsl_fft_real_workspace* work;
  gsl_fft_halfcomplex_wavetable* hc;
  double* pPadded;
  double dFreqValue;
  int iLengthData;
  int iLengthDataPadded;
  bool bReturn = false;
  int iStatus;
  int i;
  
  if( scalars.at(1)->value() > 0.0 ) {
    iLengthData = vector->length();
    
    if( iLengthData > 0 ) {
      //
      // round up to the nearest power of 2...
      //
      iLengthDataPadded = (int)pow( 2.0, ceil( log10( (double)iLengthData ) / log10( 2.0 ) ) );
      pPadded = (double*)malloc( iLengthDataPadded * sizeof( double ) );
      if( pPadded != 0L ) {
        outVector->resize(iLengthData);

        real = gsl_fft_real_wavetable_alloc( iLengthDataPadded );
        if( real != NULL ) {
          work = gsl_fft_real_workspace_alloc( iLengthDataPadded );
          if( work != NULL ) {
            memcpy( pPadded, vector->value(), iLengthData * sizeof( double ) );

            //
            // linear extrapolation on the padded values...
            //
            for( i=iLengthData; i<iLengthDataPadded; i++ ) {
              pPadded[i] = vector->value()[iLengthData-1] - (double)( i - iLengthData + 1 ) * ( vector->value()[iLengthData-1] - vector->value()[0] ) / (double)( iLengthDataPadded - iLengthData );
            }
            
            //
            // calculate the FFT...
            //
            iStatus = gsl_fft_real_transform( pPadded, 1, iLengthDataPadded, real, work );

            if( !iStatus ) {
              //
              // apply the filter...
              //
              for( i=0; i<iLengthDataPadded; i++ ) {
                dFreqValue = 0.5 * (double)i / (double)iLengthDataPadded;
                pPadded[i] *= filter_calculate( dFreqValue, scalars );
              }

              hc = gsl_fft_halfcomplex_wavetable_alloc( iLengthDataPadded );
              if( hc != NULL ) {
                //
                // calculate the inverse FFT...
                //
                iStatus = gsl_fft_halfcomplex_inverse( pPadded, 1, iLengthDataPadded, hc, work );
                if( !iStatus ) {
                  memcpy( outVector->value(), pPadded, iLengthData * sizeof( double ) );
                  bReturn = true;
                }
                gsl_fft_halfcomplex_wavetable_free( hc );
              }
            }
            gsl_fft_real_workspace_free( work );
          }
          gsl_fft_real_wavetable_free( real );
        }
        free( pPadded );
      }
    }
  }
  return bReturn;
}
