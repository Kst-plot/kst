/*
 *  Generic non-linear fit plugin for KST.
 *  Copyright 2004, The University of British Columbia
 *  Released under the terms of the GPL.
 */

#define X           0
#define Y           1
#define X_INTERP    2

#include <kstvector.h>

bool interpolate(KstVectorPtr x_array,
                 KstVectorPtr y_array,
                 KstVectorPtr x1_array,
                 KstVectorPtr y_interpolated,
                 const gsl_interp_type* pType) {

  gsl_interp_accel *pAccel = NULL;
  gsl_interp *pInterp = NULL;
  gsl_spline *pSpline = NULL;
  int iLengthData;
  int iLengthInterp;
  bool iReturn = false;
  double* pResult[1];

  iLengthData = x_array->length();
  if (y_array->length() < iLengthData) {
    iLengthData = y_array->length();
  }

  iLengthInterp = x1_array->length();
  if (iLengthInterp > 0) {
    if (y_interpolated->length() != iLengthInterp) {
      y_interpolated->resize(iLengthInterp, true);
      pResult[0] = (double*)realloc( y_interpolated->value(), iLengthInterp * sizeof( double ) );
    } else {
      pResult[0] = y_interpolated->value();
    }

    if (pResult[0] != NULL) {

      for (int i = 0; i < iLengthInterp; ++i) {
        y_interpolated->value()[i] = pResult[0][i];
      }

      pInterp = gsl_interp_alloc( pType, iLengthData );
      if (pInterp != NULL) {
        //
        // check that we have enough data points...
        //
        if ((unsigned int)iLengthData > gsl_interp_min_size( pInterp )) {
          pAccel  = gsl_interp_accel_alloc( );
          if (pAccel != NULL) {
            pSpline = gsl_spline_alloc( pType, iLengthData );
            if (pSpline != NULL) {
              if (!gsl_spline_init( pSpline, x_array->value(), y_array->value(), iLengthData )) {
                for( int i=0; i<iLengthInterp; i++) {
                  y_interpolated->value()[i] = gsl_spline_eval( pSpline, x1_array->value()[i], pAccel );
                }

                iReturn = true;
              }
              gsl_spline_free( pSpline );
            }
            gsl_interp_accel_free( pAccel );
          }
        }
        gsl_interp_free( pInterp );
      }
    }
  }

  return iReturn;
}
