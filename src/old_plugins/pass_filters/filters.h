/*
 *  Generic pass filter plugin for KST.
 *  Copyright 2004, The University of British Columbia
 *  Released under the terms of the GPL.
 */

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_real.h>
#include <gsl/gsl_fft_halfcomplex.h>

#define KST_UNUSED(x) if(x){};

double filter_calculate( double dFreqValue, const double inScalars[] );

int kst_pass_filter(
  const double *const inArrays[], 
  const int inArrayLens[],
  const double inScalars[],
  double *outArrays[], 
  int outArrayLens[],
	double outScalars[] );

int kst_pass_filter(
  const double *const inArrays[], 
  const int inArrayLens[],
  const double inScalars[],
  double *outArrays[], 
  int outArrayLens[],
	double outScalars[] ) {
  
  KST_UNUSED( outScalars )
  
  gsl_fft_real_wavetable* real;
  gsl_fft_real_workspace* work;
  gsl_fft_halfcomplex_wavetable* hc;
  double* pResult[1];
  double* pPadded;
  double dFreqValue;
  int iLengthData;
  int iLengthDataPadded;
  int iReturn = -1;
  int iStatus;
  int i;
  
  if( inScalars[1] > 0.0 ) {
    iLengthData = inArrayLens[0];
    
    if( iLengthData > 0 ) {
      //
      // round up to the nearest power of 2...
      //
      iLengthDataPadded = (int)pow( 2.0, ceil( log10( (double)iLengthData ) / log10( 2.0 ) ) );
      pPadded = (double*)malloc( iLengthDataPadded * sizeof( double ) );
      if( pPadded != 0L ) {      
        if( outArrayLens[0] != iLengthData ) {
          pResult[0] = (double*)realloc( outArrays[0], iLengthData * sizeof( double ) );
        } else {
          pResult[0] = outArrays[0];
        }

        if( pResult[0] != NULL ) {
          outArrays[0] = pResult[0];
          outArrayLens[0] = iLengthData;

          real = gsl_fft_real_wavetable_alloc( iLengthDataPadded );
          if( real != NULL ) {
            work = gsl_fft_real_workspace_alloc( iLengthDataPadded );
            if( work != NULL ) {
              memcpy( pPadded, inArrays[0], iLengthData * sizeof( double ) );

              //
              // linear extrapolation on the padded values...
              //
              for( i=iLengthData; i<iLengthDataPadded; i++ ) {
                pPadded[i] = inArrays[0][iLengthData-1] - (double)( i - iLengthData + 1 ) * ( inArrays[0][iLengthData-1] - inArrays[0][0] ) / (double)( iLengthDataPadded - iLengthData );
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
                  pPadded[i] *= filter_calculate( dFreqValue, inScalars );
                }

                hc = gsl_fft_halfcomplex_wavetable_alloc( iLengthDataPadded );
                if( hc != NULL ) {
                  //
                  // calculate the inverse FFT...
                  //
                  iStatus = gsl_fft_halfcomplex_inverse( pPadded, 1, iLengthDataPadded, hc, work );
                  if( !iStatus ) {
                    memcpy( outArrays[0], pPadded, iLengthData * sizeof( double ) );
                    iReturn = 0;
                  }
                  gsl_fft_halfcomplex_wavetable_free( hc );
                }
              }

              gsl_fft_real_workspace_free( work );
            }
            gsl_fft_real_wavetable_free( real );
          }
        }
        free( pPadded );
      }
    }
  }
  
  return iReturn;
}
