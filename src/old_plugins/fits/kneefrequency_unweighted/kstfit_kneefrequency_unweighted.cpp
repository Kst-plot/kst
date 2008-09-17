/***************************************************************************
    kstfit_kneefrequency_unweighted.cpp  -  knee frequency fit plugin for kst
                             -------------------
    begin                : Jun 6, 2006
    copyright            : (C) 2006 by Duncan Hanson
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h> //FIXME: remove w/ the printf statements.
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define KNEEFREQ_XVALUES      0
#define KNEEFREQ_YVALUES      1
#define KNEEFREQ_WEIGHTS      2

#define	KNEEFREQ_YFIT         0
#define KNEEFREQ_YRESIDUALS   1
#define KNEEFREQ_PARAMETERS   2

#define KNEEFREQ_NUMPARAMETERS 5

extern "C" int parameterName(int iIndex, char** pName);
extern "C" int kstfit_kneefrequency_unweighted(const double *const inArrays[], const int inArrayLens[],
		const double inScalars[],
		double *outArrays[], int outArrayLens[],
		double outScalars[]);

int kstfit_kneefrequency_unweighted(const double *const inArrays[], const int inArrayLens[],
		const double inScalars[],
		double *outArrays[], int outArrayLens[],
		double outScalars[])
{
  if ((inArrayLens[KNEEFREQ_XVALUES] != inArrayLens[KNEEFREQ_YVALUES] ) || (inArrayLens[KNEEFREQ_XVALUES] < 1))  {
    ////FIXME: give a more descriptive error.
    //return -2; // input error. array lengths different?
    printf("KSTFIT_KNEE_FREQUENCY_UNWEIGHTED: ERROR. INPUT ARRAYS HAVE IMPROPER LENGTHS.\n");
    return 0; //FIXME: remove. errors for fit plugins don't seem to be handled safely right now.
  }

  int inArraysLength = inArrayLens[KNEEFREQ_XVALUES];

  outArrays[KNEEFREQ_YFIT]=(double*)realloc(outArrays[KNEEFREQ_YFIT], inArraysLength*sizeof(double));
  outArrays[KNEEFREQ_YRESIDUALS]=(double*)realloc(outArrays[KNEEFREQ_YRESIDUALS], inArraysLength*sizeof(double));
  outArrays[KNEEFREQ_PARAMETERS]=(double*)realloc(outArrays[KNEEFREQ_PARAMETERS], KNEEFREQ_NUMPARAMETERS*sizeof(double));

  if ((outArrays[KNEEFREQ_YFIT] == NULL) || (outArrays[KNEEFREQ_YRESIDUALS] == NULL) || (outArrays[KNEEFREQ_PARAMETERS] == NULL)) {
    ////FIXME: give a more descriptive error.
    //return -3; // memory error.
    printf("KSTFIT_KNEE_FREQUENCY_UNWEIGHTED: ERROR. NOT ENOUGH MEMORY.\n");
    return 0; //FIXME: remove. errors for fit plugins don't seem to be handled safely right now.
  }

  outArrayLens[KNEEFREQ_YFIT] = inArraysLength;
  outArrayLens[KNEEFREQ_YRESIDUALS] = inArraysLength;
  outArrayLens[KNEEFREQ_PARAMETERS] = KNEEFREQ_NUMPARAMETERS;

  double xi, yi;
  int i;
  double maxOneOverFFreq, minWhiteNoiseFreq, whiteNoiseC;

  //FIXME: slow hack to make it obvious to the user that something is wrong if the plugin fails to complete. we need a better way to handle errors from plugins.
    for (i = 0; i < inArraysLength; i++) {
      outArrays[KNEEFREQ_YFIT][i] = 1;
      outArrays[KNEEFREQ_YRESIDUALS][i] = 1;
    }

    outArrays[KNEEFREQ_PARAMETERS][0] = 0;
    outArrays[KNEEFREQ_PARAMETERS][1] = 0;
    outArrays[KNEEFREQ_PARAMETERS][2] = 0;
    outArrays[KNEEFREQ_PARAMETERS][3] = 0;
    outArrays[KNEEFREQ_PARAMETERS][4] = 0;
  //end hack.

  maxOneOverFFreq = inScalars[0];
  minWhiteNoiseFreq = inScalars[1];
  whiteNoiseC = inScalars[2];

  int maxOneOverFIndex, minWhiteNoiseIndex;

  //fast calculation of index for maxOneOverFFreq
  int i_bot = 0;
  int i_top = inArraysLength - 1;

  while (i_bot + 1 < i_top) {
    int i0 = (i_top + i_bot)/2;
    if (maxOneOverFFreq < inArrays[KNEEFREQ_XVALUES][i0]) {
      i_top = i0;
    } else {
      i_bot = i0;
    }
  }
  maxOneOverFIndex = i_top; //top because we use i_bot+1.

  //fast calculation of index for minWhiteNoiseFreq
  i_bot = 0;
  i_top = inArraysLength - 1;

  while (i_bot + 1 < i_top) {
    int i0 = (i_top + i_bot)/2;
    if (minWhiteNoiseFreq < inArrays[KNEEFREQ_XVALUES][i0]) {
      i_top = i0;
    } else {
      i_bot = i0;
    }
  }
  minWhiteNoiseIndex = i_top;

  //verify calculated indices.
  if ( !(maxOneOverFIndex>0) || !(minWhiteNoiseIndex>=maxOneOverFIndex) || !(minWhiteNoiseIndex<(inArraysLength-1)) ) {
    ////FIXME: give a more descriptive error.
    //return -2; // input error. frequenc(y/ies) are invalid.
    printf("KSTFIT_KNEE_FREQUENCY_UNWEIGHTED: ERROR. INPUT FREQUENCIES INVALID?.\n");
    return 0; //FIXME: remove. errors for fit plugins don't seem to be handled safely right now.
  }

  // calculate white noise limit
  double sumY, sumY2;
  sumY = sumY2 = 0;

  for (i = minWhiteNoiseIndex; i < inArraysLength; i++) {
    yi = inArrays[KNEEFREQ_YVALUES][i];
    sumY    +=  yi;
    sumY2   +=  pow(yi,2);
  }

  double ybar, ysigma;
  ybar = sumY/(inArraysLength - minWhiteNoiseIndex);
  ysigma = sqrt((sumY2 - 2*ybar*sumY + pow(ybar,2)*(inArraysLength - minWhiteNoiseIndex))/(inArraysLength - minWhiteNoiseIndex));
  // end calculate white noise limit

  // fit 1/f noise
  double sumLnXLnY, sumLnX, sumLnY, sumLnX2;
  sumLnXLnY = sumLnX = sumLnY = sumLnX2 = 0;

  for (i = 0; i < maxOneOverFIndex; i++) {
    xi = inArrays[KNEEFREQ_XVALUES][i];
    yi = inArrays[KNEEFREQ_YVALUES][i];

    if (!(xi>0) || !((yi-ybar)>0)) {
      ////FIXME: give a more descriptive error.
      //return -2; // input error: maxOneOverFFreq too large?
      printf("KSTFIT_KNEE_FREQUENCY_UNWEIGHTED: ERROR. INVALID DATA?\n");
      return 0; //FIXME: remove. errors for fit plugins don't seem to be handled safely right now.
    }

    sumLnXLnY += log(xi)*log(yi-ybar); //-ybar to isolate 1/f noise.
    sumLnX    += log(xi);
    sumLnY    += log(yi-ybar);
    sumLnX2   += pow(log(xi),2);
  }

  double a, b;
  a = (maxOneOverFIndex*sumLnXLnY - sumLnX*sumLnY)/(maxOneOverFIndex*sumLnX2 - pow(sumLnX,2));
  b = exp((sumLnY - a*sumLnX)/maxOneOverFIndex);
  // end fit 1/f noise

  double knee_freq = pow(ybar*whiteNoiseC/b,1.0/a); // calculate knee frequency.

  // output fit data
  for (i = 0; i < maxOneOverFIndex; i++) {
      outArrays[KNEEFREQ_YFIT][i] = b * pow(inArrays[KNEEFREQ_XVALUES][i],a) + ybar;
      outArrays[KNEEFREQ_YRESIDUALS][i] = inArrays[KNEEFREQ_YVALUES][i] - outArrays[KNEEFREQ_YFIT][i];
  }

  for (i = maxOneOverFIndex; i < minWhiteNoiseIndex; i++) { // zeros for unfitted region.
      outArrays[KNEEFREQ_YFIT][i] = 0;
      outArrays[KNEEFREQ_YRESIDUALS][i] = 0;
  }

  for (i = minWhiteNoiseIndex; i < inArraysLength; i++) {
      outArrays[KNEEFREQ_YFIT][i] = ybar;
      outArrays[KNEEFREQ_YRESIDUALS][i] = inArrays[KNEEFREQ_YVALUES][i] - ybar;
  }

  outArrays[KNEEFREQ_PARAMETERS][0] = ybar;
  outArrays[KNEEFREQ_PARAMETERS][1] = ysigma;
  outArrays[KNEEFREQ_PARAMETERS][2] = b;
  outArrays[KNEEFREQ_PARAMETERS][3] = -a;
  outArrays[KNEEFREQ_PARAMETERS][4] = knee_freq;

  return 0;
}

int parameterName(int iIndex, char** pName) {
  int iRetVal = 0;

  switch (iIndex) {
    case 0:
      *pName = strdup("White Noise Limit");
      iRetVal = 1;
      break;
    case 1:
      *pName = strdup("White Noise Sigma");
      iRetVal = 1;
      break;
    case 2:
      *pName = strdup("1/f^a Amplitude");
      iRetVal = 1;
      break;
    case 3:
      *pName = strdup("1/f^a Power Law a");
      iRetVal = 1;
      break;
    case 4:
      *pName = strdup("Knee Frequency");
      iRetVal = 1;
      break;
  }

  return iRetVal;
}
