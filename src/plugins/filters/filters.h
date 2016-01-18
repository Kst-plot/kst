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
int min_pad(Kst::ScalarList scalars);

void fit_mb(double *y, int n, double &m, double &b) {
  if (n<5) {
    m = 0.0;
    b = y[0];
    return;
  }

  double Sy=0;
  double x_ = 0; // mean of x
  double y_ = 0; // mean of y
  int i;
  double x;
  double Sdxdy=0; // sum(x - x_)
  double Sdx2=0; // sum((x - x_)^2)

  for (i = 0; i<n; i++) {
    Sy += y[i];
  }
  x_ = double(n)*0.5;
  y_ = Sy/n;

  for (i = 0; i<n; i++) {
    x = i;
    Sdxdy += (x - x_) * (y[i] - y_);
    Sdx2 +=  (x - x_) * (x - x_);
  }
  Sdx2 = qMax(Sdx2,1.0);
  m = Sdxdy/Sdx2;
  b = y_;
}

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
      // make sure the padding is long enough - this depends on the type of filter.
      if (iLengthDataPadded - iLengthData < min_pad(scalars)) {
        //qDebug() << "doubled length" << min_pad(scalars) << iLengthDataPadded - iLengthData << iLengthDataPadded;
        iLengthDataPadded *= 2.0;
      }
      pPadded = (double*)malloc( iLengthDataPadded * sizeof( double ) );
      if( pPadded != 0L ) {
        outVector->resize(iLengthData);
        //outVector->resize(iLengthDataPadded);  // DEBUG ************

        real = gsl_fft_real_wavetable_alloc( iLengthDataPadded );
        if( real != NULL ) {
          work = gsl_fft_real_workspace_alloc( iLengthDataPadded );
          if( work != NULL ) {
            memcpy( pPadded, vector->noNanValue(), iLengthData * sizeof( double ) );

            // We are going to do a cubic spline extrapolation on the data
            // to improve behavior for high pass filters.
            double m1, b1;
            double m2, b2;

            int nf = min_pad(scalars)/10.0;
            if (nf > iLengthData/5) nf = iLengthData/5;

            fit_mb(pPadded, nf, m2, b2);
            fit_mb(pPadded + (iLengthData-nf-1), nf, m1, b1);

            double a = b1;
            double b = m1;
            double X = double(nf + iLengthDataPadded - iLengthData);
            double d = (-2.0*b2 +m2*X + 2.0*b1 + m1*X)/(X*X*X);
            double c = (b2 - b1 - m1*X - d*X*X*X)/(X*X);

            //
            // polynomial extrapolation on the padded values...
            //
            for( i=iLengthData; i<iLengthDataPadded; i++ ) {
              X = double(i-iLengthData)+nf*0.5;
              //pPadded[i] = a + b*X + c*X*X + d*X*X*X;
              pPadded[i] = a + X*(b + X*(c + X*d));
              //pPadded[i] = vector->value()[iLengthData-1] - (double)( i - iLengthData + 1 ) * ( vector->value()[iLengthData-1] - vector->value()[0] ) / (double)( iLengthDataPadded - iLengthData );
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
                  //memcpy( outVector->value(), pPadded, iLengthDataPadded * sizeof( double ) ); // DEBUG **********************
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
