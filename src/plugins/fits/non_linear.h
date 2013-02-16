/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
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
#include <gsl/gsl_rng.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_statistics.h>
#include "common.h"

struct data {
  size_t	n;
  const double*	pdX;
  const double* pdY;
};

void function_initial_estimate( const double* pdX, const double* pdY, int iLength, double* pdParameterEstimates );
double function_calculate( double dX, double* pdParameters );
void function_derivative( double dX, double* pdParameters, double* pdDerivatives );
int function_f( const gsl_vector* pVectorX, void* pParams, gsl_vector* pVectorF );
int function_df( const gsl_vector* pVectorX, void* pParams, gsl_matrix* pMatrixJ );
int function_fdf( const gsl_vector* pVectorX, void* pParams, gsl_vector* pVectorF, gsl_matrix* pMatrixJ );
bool kstfit_nonlinear(
  Kst::VectorPtr xVector, Kst::VectorPtr yVector,
  Kst::VectorPtr vectorOutYFitted, Kst::VectorPtr vectorOutYResiduals,
  Kst::VectorPtr vectorOutYParameters, Kst::VectorPtr vectorOutYCovariance,
  Kst::ScalarPtr scalarOutChi );


int function_f( const gsl_vector* pVectorX, void* pParams, gsl_vector* pVectorF ) {
  double 	dParameters[NUM_PARAMS];
  double	dY;
  data*	 	pData	= (data*)pParams;
  size_t 	i;

  for( i=0; i<NUM_PARAMS; i++ ) {
    dParameters[i] = gsl_vector_get( pVectorX, i );
  }

  for( i=0; i<pData->n; i++ ) {
    dY  = function_calculate( pData->pdX[i], dParameters );
    gsl_vector_set( pVectorF, i, dY - pData->pdY[i] );
  }

  return GSL_SUCCESS;
}


int function_df( const gsl_vector* pVectorX, void* pParams, gsl_matrix* pMatrixJ ) {
  double dParameters[NUM_PARAMS];
  double dDerivatives[NUM_PARAMS];
  data*	 pData	= (data*)pParams;
  size_t i;
  size_t j;

  for( i=0; i<NUM_PARAMS; i++ ) {
    dParameters[i] = gsl_vector_get( pVectorX, i );
  }

  for( i=0; i<pData->n; i++ ) {
    function_derivative( pData->pdX[i], dParameters, dDerivatives );

    for( j=0; j<NUM_PARAMS; j++ ) {
      gsl_matrix_set( pMatrixJ, i, j, dDerivatives[j] );
    }
  }

  return GSL_SUCCESS;
}


int function_fdf( const gsl_vector* pVectorX, void* pParams, gsl_vector* pVectorF, gsl_matrix* pMatrixJ ) {  
  function_f( pVectorX, pParams, pVectorF );
  function_df( pVectorX, pParams, pMatrixJ );

  return GSL_SUCCESS;
}


bool kstfit_nonlinear(
  Kst::VectorPtr xVector, Kst::VectorPtr yVector,
  Kst::VectorPtr vectorOutYFitted, Kst::VectorPtr vectorOutYResiduals,
  Kst::VectorPtr vectorOutYParameters, Kst::VectorPtr vectorOutYCovariance,
  Kst::ScalarPtr scalarOutChi ) {

  const gsl_multifit_fdfsolver_type* pType;
  gsl_multifit_fdfsolver* pSolver;
  gsl_multifit_function_fdf	function;
  gsl_vector_view vectorViewInitial;
  gsl_matrix* pMatrixCovariance;
  struct data d;  
  double dXInitial[NUM_PARAMS];
  double* pInputX;
  double* pInputY;
  int iIterations = 0;
  int iLength;
  bool bReturn = false;
  int iStatus;
  int i;
  int j;

  if (xVector->length() >= 2 &&
      yVector->length() >= 2 ) {
    iLength = yVector->length();
    if( xVector->length() > iLength ) {
      iLength = xVector->length();
    }

    pInputX = (double*)malloc(iLength * sizeof( double ));
    if (xVector->length() == iLength) {
      for( i=0; i<iLength; i++) {
        pInputX[i] = xVector->value()[i];
      }
    } else {
      for( i=0; i<iLength; i++) {
        pInputX[i] = interpolate( i, iLength, xVector->value(), xVector->length() );
      }
    }

    pInputY = (double*)malloc(iLength * sizeof( double ));
    if (yVector->length() == iLength) {
      for( i=0; i<iLength; i++) {
        pInputY[i] = yVector->value()[i];
      }
    } else {
      for( i=0; i<iLength; i++) {
        pInputY[i] = interpolate( i, iLength, yVector->value(), yVector->length() );
      }
    }

    if( iLength > NUM_PARAMS ) {
      vectorOutYFitted->resize(iLength);
      vectorOutYResiduals->resize(iLength);
      vectorOutYParameters->resize(NUM_PARAMS);
      vectorOutYCovariance->resize(NUM_PARAMS * NUM_PARAMS);

      pType   = gsl_multifit_fdfsolver_lmsder;
      pSolver = gsl_multifit_fdfsolver_alloc( pType, iLength, NUM_PARAMS );
      if( pSolver != NULL ) {
        d.n      = iLength;
        d.pdX	   = pInputX;
        d.pdY    = pInputY;

        function.f   	  = function_f;
        function.df  	  = function_df;
        function.fdf 	  = function_fdf;
        function.n   	  = iLength;
        function.p   	  = NUM_PARAMS;
        function.params = &d;

        pMatrixCovariance = gsl_matrix_alloc( NUM_PARAMS, NUM_PARAMS );
        if( pMatrixCovariance != NULL ) {
          function_initial_estimate( pInputX, pInputY, iLength, dXInitial );
          vectorViewInitial = gsl_vector_view_array( dXInitial, NUM_PARAMS );

          gsl_multifit_fdfsolver_set( pSolver, &function, &vectorViewInitial.vector );

          //
          // iterate to a solution...
          //
          do {
            iStatus = gsl_multifit_fdfsolver_iterate( pSolver );
            if( iStatus == GSL_SUCCESS ) {
              iStatus = gsl_multifit_test_delta( pSolver->dx, pSolver->x, 1.0e-6, 1.0e-6 );
            }
            iIterations++;
          } while( iStatus == GSL_CONTINUE && iIterations < MAX_NUM_ITERATIONS );
          gsl_multifit_covar( pSolver->J, 0.0, pMatrixCovariance );

          //
          // determine the fitted values...
          //
          for( i=0; i<NUM_PARAMS; i++ ) {
            dXInitial[i] = gsl_vector_get( pSolver->x, i );
          }

          for( i=0; i<iLength; i++ ) {
            vectorOutYFitted->value()[i] = function_calculate( pInputX[i], dXInitial );
            vectorOutYResiduals->value()[i] = pInputY[i] - vectorOutYFitted->value()[i];
          }

          //
          // fill in the parameter values and covariance matrix...
          //
          for( i=0; i<NUM_PARAMS; i++ ) {
            vectorOutYParameters->value()[i] = gsl_vector_get( pSolver->x, i );
            for( j=0; j<NUM_PARAMS; j++ ) {
              vectorOutYCovariance->value()[(i*NUM_PARAMS)+j] = gsl_matrix_get( pMatrixCovariance, i, j );
            }
          }

          //
          // determine the value of chi^2/nu
          //
          scalarOutChi->setValue(gsl_blas_dnrm2( pSolver->f ));

          bReturn = true;

          gsl_matrix_free( pMatrixCovariance );
        }
        gsl_multifit_fdfsolver_free( pSolver );
      }
    }
    free( pInputX );
    free( pInputY );
  }

  return bReturn;
}
