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

#include <vector.h>

bool interpolate(Kst::VectorPtr xVector,
                 Kst::VectorPtr yVector,
                 Kst::VectorPtr x1Vector,
                 Kst::VectorPtr yOutVector,
                 const gsl_interp_type* pType) {

  gsl_interp_accel *pAccel = NULL;
  gsl_interp *pInterp = NULL;
  gsl_spline *pSpline = NULL;
  int iLengthData;
  int iLengthInterp;
  bool bReturn = false;
  double* pResult[1];

  iLengthData = xVector->length();
  if (yVector->length() < iLengthData) {
    iLengthData = yVector->length();
  }

  iLengthInterp = x1Vector->length();
  if (iLengthInterp > 0) {
    if (yOutVector->length() != iLengthInterp) {
      yOutVector->resize(iLengthInterp, true);
      pResult[0] = (double*)realloc( yOutVector->value(), iLengthInterp * sizeof( double ) );
    } else {
      pResult[0] = yOutVector->value();
    }

    if (pResult[0] != NULL) {

      for (int i = 0; i < iLengthInterp; ++i) {
        yOutVector->value()[i] = pResult[0][i];
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
              if (!gsl_spline_init( pSpline, xVector->value(), yVector->value(), iLengthData )) {
                for( int i=0; i<iLengthInterp; i++) {
                  yOutVector->value()[i] = gsl_spline_eval( pSpline, x1Vector->value()[i], pAccel );
                }

                bReturn = true;
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

  return bReturn;
}
