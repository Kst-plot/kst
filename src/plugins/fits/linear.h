/*
 *  Generic linear fit plugin for KST.
 *  Copyright 2004, The University of British Columbia
 *  Released under the terms of the GPL.
 */

#include <gsl/gsl_multifit.h>
#include "common.h"

double calculate_matrix_entry( double dX, int iPos );

extern "C" bool kstfit_linear_unweighted(
  Kst::VectorPtr xVector, Kst::VectorPtr yVector,
  Kst::VectorPtr vectorOutYFitted, Kst::VectorPtr vectorOutYResiduals,
  Kst::VectorPtr vectorOutYParameters, Kst::VectorPtr vectorOutYCovariance,
  Kst::ScalarPtr scalarOutChi, int iNumParams);

bool kstfit_linear_unweighted(
  Kst::VectorPtr xVector, Kst::VectorPtr yVector,
  Kst::VectorPtr vectorOutYFitted, Kst::VectorPtr vectorOutYResiduals,
  Kst::VectorPtr vectorOutYParameters, Kst::VectorPtr vectorOutYCovariance,
  Kst::ScalarPtr scalarOutChi, int iNumParams)
{
  gsl_matrix* pMatrixX = NULL;
  gsl_matrix* pMatrixCovariance = NULL;
  gsl_vector* pVectorY = NULL;
  gsl_vector* pVectorParameters = NULL;
  gsl_multifit_linear_workspace* pWork = NULL;
  double dXInterpolated;
  double dX;
  double dY;
  double dChiSq = 0.0;
  int i = 0;
  int j;
  int iStatus = 0;
  int iLength;
  bool bReturn = false;

  if (xVector->length() >= 2 &&
      yVector->length() >= 2 ) {
    iLength = yVector->length();
    if( xVector->length() > iLength ) {
      iLength = xVector->length();
    }

    if( iLength > iNumParams + 1 ) {
      vectorOutYFitted->resize(iLength);
      vectorOutYResiduals->resize(iLength);
      vectorOutYParameters->resize(iNumParams);
      vectorOutYCovariance->resize(iNumParams * iNumParams);
      //
      // create the matrices and vectors...
      //
      pMatrixX = gsl_matrix_alloc( iLength, iNumParams );
      if( pMatrixX != NULL ) {
        pVectorY = gsl_vector_alloc( iLength );
        if( pVectorY != NULL ) {
          pVectorParameters = gsl_vector_alloc( iNumParams );
          if( pVectorParameters != NULL ) {
            pMatrixCovariance = gsl_matrix_alloc( iNumParams, iNumParams );
            if( pMatrixCovariance != NULL ) {
              pWork = gsl_multifit_linear_alloc( iLength, iNumParams );
              if( pWork != NULL ) {

                //
                // fill in the matrices and vectors...
                //
                for( i=0; i<iLength; i++ ) {
                  gsl_vector_set( pVectorY, i, interpolate(i, iLength, yVector->value(), yVector->length()));
                  dXInterpolated = interpolate(i, iLength, xVector->value(), xVector->length());
                  for( j=0; j<iNumParams; j++ ) {
                    dX = calculate_matrix_entry( dXInterpolated, j );
                    gsl_matrix_set( pMatrixX, i, j, dX );
                  }
                }


                iStatus = gsl_multifit_linear( pMatrixX,
                                                pVectorY,
                                                pVectorParameters,
                                                pMatrixCovariance,
                                                &dChiSq,
                                                pWork );
                if( iStatus == 0 ) {
                  //
                  // fill in the output arrays and scalars...
                  //
                  for( i=0; i<iLength; i++ ) {
                    dY = 0.0;
                    for( j=0; j<iNumParams; j++ ) {
                      dY += gsl_matrix_get( pMatrixX, i, j ) *
                            gsl_vector_get( pVectorParameters, j );
                    }
                    vectorOutYFitted->value()[i] = dY;
                    vectorOutYResiduals->value()[i] = interpolate(i, iLength, yVector->value(), yVector->length()) - dY;
                  }

                  for( i=0; i<iNumParams; i++ ) {
                    vectorOutYParameters->value()[i] = gsl_vector_get( pVectorParameters, i );
                    for( j=0; j<iNumParams; j++ ) {
                      vectorOutYCovariance->value()[(i*iNumParams)+j] = gsl_matrix_get( pMatrixCovariance, i, j );
                    }
                  }

                  scalarOutChi->setValue(dChiSq / ( (double)iLength - (double)iNumParams ));

                  bReturn = true;
                }

                gsl_multifit_linear_free( pWork );
              }
              gsl_matrix_free( pMatrixCovariance );
            }
            gsl_vector_free( pVectorParameters );
          }
          gsl_vector_free( pVectorY );
        }
        gsl_matrix_free( pMatrixX );
      }
    }
  }

  return bReturn;
}
