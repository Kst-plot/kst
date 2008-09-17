/***************************************************************************
                          De-spiking filter for kst
                             -------------------
    begin                : Jan 2005
    copyright            : (C) 2005 by cbn
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                        2                                                 *
 ***************************************************************************/


#include <stdlib.h>
#include <math.h>

extern "C" int despike(const double *const inArrays[], const int inArrayLens[],
		       const double inScalars[],
		       double *outArrays[], int outArrayLens[],
		       double outScalars[]);

int despike(const double *const inArrays[], const int inArrayLens[],
	    const double inScalars[],
	    double *outArrays[], int outArrayLens[],
	    double outScalars[]) {
  
  int N = inArrayLens[0];
  const double *X = inArrays[0];
  double *Y;
  double last_good;
  double cut = inScalars[1];
  double mdev = 0;
  int dx = int(inScalars[0]);
  int i;
  int spike_start = -1;
  int border = dx*2;

  if (N<1 || cut<=0 || dx<1 || dx>N/2) {
    return -1;
  }

  /* get mean deviation of 3 pt difference */
  for (i=dx; i<N-dx; i++) {
    mdev += fabs(X[i]-(X[i-dx]+X[i+dx])*0.5);
  }
  mdev /= double(N);

  cut *= mdev;

  // resize the output array
  outArrayLens[0] = N;
  Y = outArrays[0] = (double*)realloc(outArrays[0], N*sizeof(double));

  // for first dx points, do a 2 point difference
  last_good = X[0];
  for (i=0; i<dx; i++) {
    if (fabs(X[i] - X[i+dx])>cut) {
      if (spike_start<0) { 
	spike_start = i-border;
	if (spike_start<0) spike_start = 0;
      }
    } else {
      if (spike_start>=0) {
	i += 4*border-1; 
	if (i>=N) i=N-1;
	for (int j=spike_start; j<=i; j++) {
	  Y[j] = last_good;
	}
	spike_start = -1;
      }
      last_good = Y[i] = X[i];
    }
  }
  // do a 3 point difference where it is possible
  for (i=dx; i<N-dx; i++) {
    if (fabs(X[i] - (X[i-dx]+X[i+dx])*0.5)>cut) {
      if (spike_start<0) { 
	spike_start = i-border;
	if (spike_start<0) spike_start = 0;
      }
    } else {
      if (spike_start>=0) {
	i += 4*border-1; 
	if (i>=N) i=N-1;
	for (int j=spike_start; j<=i; j++) {
	  Y[j] = last_good;
	}
	spike_start = -1;
      } else {
	last_good = Y[i] = X[i];
      }
    }
  }
  // do a 2 point difference for lat dx points
  for (i=N-dx; i<N; i++) {
    if (fabs(X[i-dx] - X[i])>cut) {
      if (spike_start<0) { 
	spike_start = i-border;
	if (spike_start<0) spike_start = 0;
      }
    } else {
      if (spike_start>=0) {
	i += 4*border-1; 
	if (i>=N) i=N-1;
	for (int j=spike_start; j<=i; j++) {
	  Y[j] = last_good;
	}
	spike_start = -1;
      } else {
	last_good = Y[i] = X[i];
      }
    }
  }

  return 0;
}
