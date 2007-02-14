/*
 *  Generic non-linear fit plugin for KST.
 *  Copyright 2004, The University of British Columbia
 *  Released under the terms of the GPL.
 */

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
  const double* pdWeight;
};

void function_initial_estimate( const double* pdX, const double* pdY, int iLength, double* pdParameterEstimates );
double function_calculate( double dX, double* pdParameters );
void function_derivative( double dX, double* pdParameters, double* pdDerivatives );
int function_f( const gsl_vector* pVectorX, void* pParams, gsl_vector* pVectorF );
int function_df( const gsl_vector* pVectorX, void* pParams, gsl_matrix* pMatrixJ );
int function_fdf( const gsl_vector* pVectorX, void* pParams, gsl_vector* pVectorF, gsl_matrix* pMatrixJ );
int kstfit_nonlinear(
  const double *const inArrays[], 
  const int inArrayLens[],
  const double inScalars[],
  double *outArrays[], 
  int outArrayLens[],
	double outScalars[] );

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
    gsl_vector_set( pVectorF, i, (dY - pData->pdY[i])*pData->pdWeight[i] );
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
      gsl_matrix_set( pMatrixJ, i, j, dDerivatives[j] * pData->pdWeight[i] );
    }
  }
  
  return GSL_SUCCESS;
}

int function_fdf( const gsl_vector* pVectorX, void* pParams, gsl_vector* pVectorF, gsl_matrix* pMatrixJ ) {  
  function_f( pVectorX, pParams, pVectorF );
  function_df( pVectorX, pParams, pMatrixJ );
  
  return GSL_SUCCESS;
}

int kstfit_nonlinear(
  const double *const inArrays[], 
  const int inArrayLens[],
  const double inScalars[],
  double *outArrays[], 
  int outArrayLens[],
	double outScalars[] ) {
  
  KST_UNUSED( inScalars )
    
  const gsl_multifit_fdfsolver_type* pType;
  gsl_multifit_fdfsolver* pSolver;
  gsl_multifit_function_fdf	function;
  gsl_vector_view vectorViewInitial;
  gsl_matrix* pMatrixCovariance;
  struct data d;  
  double dXInitial[NUM_PARAMS];
  double* pDelete[3];
  double* pInput[3];
  double* pResult[4];
  int iIterations = 0;
  int iLengthData;
  int iReturn = -1;
  int iStatus;
  int i;
  int j;
  
  if( inArrayLens[X] >= 2 &&
      inArrayLens[Y] >= 2 &&
      inArrayLens[WEIGHT] >= 2 ) {
    iLengthData = inArrayLens[X];
    if( inArrayLens[Y] > iLengthData ) {
      iLengthData = inArrayLens[Y];
    }

    //
    // do any necessary interpolation...
    //
    for( i=0; i<3; i++ ) {
      if (inArrayLens[i] == iLengthData) {
        pDelete[i] = 0L;
        pInput[i] = (double*)inArrays[i];
      } else {
        pDelete[i] = (double*)malloc(iLengthData * sizeof( double ));
        pInput[i] = pDelete[i];
        for( j=0; j<iLengthData; j++) {
          pInput[i][j] = interpolate( j, iLengthData, inArrays[i], inArrayLens[i] );
        }
      }
    }

    if( iLengthData > NUM_PARAMS ) {
      if( outArrayLens[0] != iLengthData ) {
        pResult[0] = (double*)realloc( outArrays[0], iLengthData * sizeof( double ) );
      } else {
        pResult[0] = outArrays[0];
      }

      if( outArrayLens[1] != iLengthData ) {
        pResult[1] = (double*)realloc( outArrays[1], iLengthData * sizeof( double ) );
      } else {
        pResult[1] = outArrays[1];
      }

      if( outArrayLens[2] != NUM_PARAMS ) {
        pResult[2] = (double*)realloc( outArrays[2], NUM_PARAMS * sizeof( double ) );
      } else {
        pResult[2] = outArrays[2];
      }

      if( outArrayLens[3] != NUM_PARAMS * NUM_PARAMS ) {
        pResult[3] = (double*)realloc( outArrays[3], NUM_PARAMS * NUM_PARAMS * sizeof( double ) );
      } else {
        pResult[3] = outArrays[3];
      }

      if( pResult[0] != NULL &&
          pResult[1] != NULL &&
          pResult[2] != NULL &&
          pResult[3] != NULL ) {
        outArrays[0] = pResult[0];
        outArrayLens[0] = iLengthData;
        outArrays[1] = pResult[1];
        outArrayLens[1] = iLengthData;
        outArrays[2] = pResult[2];
        outArrayLens[2] = NUM_PARAMS;
        outArrays[3] = pResult[3];
        outArrayLens[3] = NUM_PARAMS * NUM_PARAMS;

        pType   = gsl_multifit_fdfsolver_lmsder;
        pSolver = gsl_multifit_fdfsolver_alloc( pType, iLengthData, NUM_PARAMS );
        if( pSolver != NULL ) {
          d.n        = iLengthData;
          d.pdX	     = pInput[X];
          d.pdY      = pInput[Y];
          d.pdWeight = pInput[WEIGHT];

          function.f   	  = function_f;
          function.df  	  = function_df;
          function.fdf 	  = function_fdf;
          function.n   	  = iLengthData;
          function.p   	  = NUM_PARAMS;
          function.params = &d;

          pMatrixCovariance = gsl_matrix_alloc( NUM_PARAMS, NUM_PARAMS );
          if( pMatrixCovariance != NULL ) {
            function_initial_estimate( pInput[X], pInput[Y], iLengthData, dXInitial );
            vectorViewInitial = gsl_vector_view_array( dXInitial, NUM_PARAMS );

            gsl_multifit_fdfsolver_set( pSolver, &function, &vectorViewInitial.vector );

            //
            // iterate to a solution...
            //
            do {
              iStatus = gsl_multifit_fdfsolver_iterate( pSolver );
              if( iStatus == GSL_SUCCESS ) {
                iStatus = gsl_multifit_test_delta( pSolver->dx, pSolver->x, 1.0e-4, 1.0e-4 );
              }
              iIterations++;
            }
            while( iStatus == GSL_CONTINUE && iIterations < MAX_NUM_ITERATIONS );

            gsl_multifit_covar( pSolver->J, 0.0, pMatrixCovariance );

            //
            // determine the fitted values...
            //
            for( i=0; i<NUM_PARAMS; i++ ) {
              dXInitial[i] = gsl_vector_get( pSolver->x, i );
            }

            for( i=0; i<iLengthData; i++ ) {
              outArrays[0][i] = function_calculate( pInput[X][i], dXInitial );
              outArrays[1][i] = pInput[Y][i] - outArrays[0][i];
            }

            //
            // fill in the parameter values and covariance matrix...
            //
            for( i=0; i<NUM_PARAMS; i++ ) {
              outArrays[2][i] = gsl_vector_get( pSolver->x, i );
              for( j=0; j<NUM_PARAMS; j++ ) {
                outArrays[3][(i*NUM_PARAMS)+j] = gsl_matrix_get( pMatrixCovariance, i, j );
              }
            }

            //
            // determine the value of chi^2/nu
            //
            outScalars[0] = gsl_blas_dnrm2( pSolver->f );

            iReturn = 0;

            gsl_matrix_free( pMatrixCovariance );
          }
          gsl_multifit_fdfsolver_free( pSolver );
        }
      }
    }

    for( i=0; i<3; i++ ) {
      if( pDelete[i] ) {
        free( pDelete[i] );
      }
    }
  }
    
  return iReturn;
}
