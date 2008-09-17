/*
 *  Generic linear fit plugin for KST.
 *  Copyright 2004, The University of British Columbia
 *  Released under the terms of the GPL.
 */

#include <stdlib.h>
#include <math.h>
#include <gsl/gsl_multifit.h>
#include "common.h"

double calculate_matrix_entry( double dX, int iPos );

extern "C" int kstfit_linear_weighted(
  const double *const inArrays[], 
  const int inArrayLens[],
  double *outArrays[], int outArrayLens[],
  double outScalars[], int iNumParams);

int kstfit_linear_weighted(
  const double *const inArrays[], 
  const int inArrayLens[],
	double *outArrays[], int outArrayLens[],
	double outScalars[], int iNumParams)
{
  gsl_matrix*	pMatrixX = NULL;
  gsl_matrix* pMatrixCovariance = NULL;
  gsl_vector*	pVectorY = NULL;
  gsl_vector* pVectorWeights = NULL;
  gsl_vector* pVectorParameters = NULL;
  gsl_multifit_linear_workspace* pWork = NULL;
  double dX;
  double dY;
  double* pResult[4];
  double dChiSq = 0.0;
  int i = 0;
  int j;
  int	iStatus = 0;
  int	iLength;
  int iReturn = -1;
  
  if (inArrayLens[YVALUES] >= 2 &&
      inArrayLens[XVALUES] >= 2 &&
      inArrayLens[WEIGHTS] >= 2) {
    iLength = inArrayLens[YVALUES];
    if( inArrayLens[XVALUES] > iLength ) {
      iLength = inArrayLens[XVALUES];
    }
        
    //
    // first do some sanity checks...
    //
    if( iNumParams > 0 && iLength > iNumParams )
    {
      //
      // first handle the output data arrays...
      //
      for( i=0; i<2; i++ ) {
        if( outArrayLens[i] != iLength ) {
          pResult[i] = (double*)realloc( outArrays[i], iLength * sizeof( double ) );
        } else {
          pResult[i] = outArrays[i];
        }
      }
      
      //
      // now the output parameter array...
      //
      for( ; i<3; i++ ) {
        if( outArrayLens[i] != iNumParams ) {
          pResult[i] = (double*)realloc( outArrays[i], iNumParams * sizeof( double ) );
        } else {
          pResult[i] = outArrays[i];
        }
      }
      
      //
      // now the covariance matrix...
      //
      for( ; i<4; i++ ) {
        if( outArrayLens[i] != iNumParams * iNumParams ) {
          pResult[i] = (double*)realloc( outArrays[i], iNumParams * iNumParams * sizeof( double ) );
        } else {
          pResult[i] = outArrays[i];
        }
      }      
      
      if( pResult[0] != NULL && 
          pResult[1] != NULL && 
          pResult[2] != NULL && 
          pResult[3] != NULL )
     {
        for( i=0; i<2; i++ ) {
          outArrays[i] 		= pResult[i];
          outArrayLens[i] = iLength;
        }
        for( ; i<3; i++ ) {
          outArrays[i] 		= pResult[i];
          outArrayLens[i] = iNumParams;
        }
        for( ; i<4; i++ ) {
          outArrays[i] 		= pResult[i];
          outArrayLens[i] = iNumParams * iNumParams;
        }
        
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
                  pVectorWeights = gsl_vector_alloc( iLength );
                  if( pVectorWeights != NULL ) {
                    
                    //
                    // fill in the matrices and vectors...
                    //
                    for( i=0; i<iLength; i++ ) {
                      gsl_vector_set( pVectorY, i, interpolate(i, iLength, inArrays[YVALUES], inArrayLens[YVALUES]));
                      gsl_vector_set( pVectorWeights, i, interpolate(i, iLength, inArrays[WEIGHTS], inArrayLens[WEIGHTS]));
                      for( j=0; j<iNumParams; j++ ) {
                        dX = calculate_matrix_entry( interpolate(i, iLength, inArrays[XVALUES], inArrayLens[XVALUES]), j );
                        gsl_matrix_set( pMatrixX, i, j, dX );
                      }
                    }
                    
                    
                    iStatus = gsl_multifit_wlinear( pMatrixX, 
                                                    pVectorWeights,
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
                        outArrays[YFIT][i] = dY;
                        outArrays[YRESIDUALS][i] = interpolate(i, iLength, inArrays[YVALUES], inArrayLens[YVALUES]) - dY;
                      }
                      
                      for( i=0; i<iNumParams; i++ ) {
                        outArrays[PARAMETERS][i] = gsl_vector_get( pVectorParameters, i );
                        for( j=0; j<iNumParams; j++ ) {
                          outArrays[COVARIANCE][(i*iNumParams)+j] = gsl_matrix_get( pMatrixCovariance, i, j );
                        }
                      }
                      
                      outScalars[0] = dChiSq / ( (double)iLength - (double)iNumParams );
                      
                      iReturn = 0;
                    }
                    gsl_vector_free( pVectorWeights );
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
  }
  
  return iReturn;
}
