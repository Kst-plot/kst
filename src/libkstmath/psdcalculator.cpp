/***************************************************************************
                     psdcalculator.cpp: Power Spectra Calculator for KST
                             -------------------
    begin                : 2006
    copyright            : (C) 2006 by Kst
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** A utility class for calculating power spectra 
*/

#include <assert.h>
#include <math.h>

#include <kglobal.h>
#include <klocale.h>

#include "kstdebug.h"
#include "psdcalculator.h"
#include "kstvector.h"

extern "C" void rdft(int n, int isgn, double *a);

#define PSDMINLEN 2
#define PSDMAXLEN 27

inline double PSDCalculator::cabs2(double r, double i) {
  return r*r + i*i;
}

PSDCalculator::PSDCalculator()
{
  _a = 0L;
  _w = 0L;

  _awLen = 0;

  _prevApodizeFxn = WindowUndefined;
  _prevGaussianSigma = 1.0;
  _prevOutputLen = 0;
}


PSDCalculator::~PSDCalculator() {
  delete[] _w;
  _w = 0L;
  delete[] _a;
  _a = 0L;
}

void PSDCalculator::updateWindowFxn(ApodizeFunction apodizeFxn, double gaussianSigma) {
  const double a = double(_awLen) / 2.0;
  double x;
  double sW = 0.0;

  switch (apodizeFxn) {
    case WindowOriginal: 
      for (int i = 0; i < _awLen; ++i) {
        _w[i] = 1.0 - cos(2.0 * M_PI * double(i) / double(_awLen));
        sW += _w[i] * _w[i];
      }
      break;

    case WindowBartlett:
      for (int i = 0; i < _awLen; ++i) {
        x = i - a;
        _w[i] = 1.0 - fabs(x) / a;
        sW += _w[i] * _w[i];
      }
      break;
 
    case WindowBlackman:
      for (int i = 0; i < _awLen; ++i) {
        x = i - a;
        _w[i] = 0.42 + 0.5 * cos(M_PI * x / a) + 0.08 * cos(2 * M_PI * x/a);
        sW += _w[i] * _w[i];
      }
      break;

    case WindowConnes: 
      for (int i = 0; i < _awLen; ++i) {
        x = i - a;
        _w[i] = pow(1.0 - (x * x) / (a * a), 2.0);
        sW += _w[i] * _w[i];
      }
      break;

    case WindowCosine:
      for (int i = 0; i < _awLen; ++i) {
        x = i - a;
        _w[i] = cos(M_PI * x / (2.0 * a));
        sW += _w[i] * _w[i];
      }
      break;

    case WindowGaussian:
      for (int i = 0; i < _awLen; ++i) {
        x = i - a;
        _w[i] = exp(-1.0 * x * x/(2.0 * gaussianSigma * gaussianSigma));
      }
      break;

    case WindowHamming:
      for (int i = 0; i < _awLen; ++i) {
        x = i - a;
        _w[i] = 0.53836 + 0.46164 * cos(M_PI * x / a);
        sW += _w[i] * _w[i];
      }
      break;

    case WindowHann:
      for (int i = 0; i < _awLen; ++i) {
        x = i - a;
        _w[i] = pow(cos(M_PI * x/(2.0 * a)), 2.0);
        sW += _w[i] * _w[i];
      }
      break;

    case WindowWelch:
      for (int i = 0; i < _awLen; ++i) {
        x = i - a;
        _w[i] = 1.0 - x * x / (a * a);
        sW += _w[i] * _w[i];
      }
      break;

    case WindowUniform:
    default:
      for (int i = 0; i < _awLen; ++i) {
        _w[i] = 1.0;
      }
      sW = _awLen;
      break;
  }

  double norm = sqrt((double)_awLen/sW); // normalization constant s.t. sum over (w^2) is _awLen

  for (int i = 0; i < _awLen; ++i) {
    _w[i] *= norm;
  }

  _prevApodizeFxn = apodizeFxn;
  _prevGaussianSigma = gaussianSigma;
}


int PSDCalculator::calculatePowerSpectrum(
  double *input, int inputLen, 
  double *output, int outputLen, 
  bool removeMean, bool interpolateHoles,
  bool average, int averageLen, 
  bool apodize, ApodizeFunction apodizeFxn, double gaussianSigma,
  PSDType outputType, double inputSamplingFreq) {

  if (outputLen != calculateOutputVectorLength(inputLen, average, averageLen)) {
    KstDebug::self()->log(i18n("in PSDCalculator::calculatePowerSpectrum: received output array with wrong length."), KstDebug::Error);
    return -1;
  }

  if (outputLen != _prevOutputLen) {
    delete[] _a;
    delete[] _w;

    _awLen = outputLen*2;
    _prevOutputLen = outputLen;

    _a = new double[_awLen];
    _w = new double[_awLen];

    updateWindowFxn(apodizeFxn, gaussianSigma);
  }

  if ( (_prevApodizeFxn != apodizeFxn) || (_prevGaussianSigma != gaussianSigma) ) {
    updateWindowFxn(apodizeFxn, gaussianSigma);
  }

  int currentCopyLen, nsamples = 0;
  int i_samp, i_subset, ioffset;

  memset(output, 0, sizeof(double)*outputLen); // initialize output.

  bool done = false;
  for (i_subset = 0; !done; i_subset++) {
    ioffset = i_subset*outputLen; //overlapping average => i_subset*outputLen

    // only zero pad if we really have to.  It is better to adjust the last chunk's
    // overlap.
    if (ioffset + _awLen*5/4 < inputLen) {
      currentCopyLen = _awLen; //will copy a complete window.
    } else if (_awLen<inputLen) {  // count the last one from the end.
      ioffset = inputLen-_awLen - 1;
      currentCopyLen = _awLen; //will copy a complete window.
      done = true;
    } else {
      currentCopyLen = inputLen - ioffset; //will copy a partial window.
      memset(&_a[currentCopyLen], 0, sizeof(double)*(_awLen - currentCopyLen)); //zero the leftovers.
      done = true;
    }

    double mean = 0.0;

    if (removeMean) {
      for (i_samp = 0; i_samp < currentCopyLen; i_samp++) {
        mean += input[i_samp + ioffset];
      }
      mean /= (double)currentCopyLen;
    }

    // apply the PSD options (removeMean, apodize, etc.)
    // separate cases for speed- although this shouldn't really matter- the rdft should be the most time consuming step by far for any large data set.
    if (removeMean && apodize && interpolateHoles) {
      for (i_samp = 0; i_samp < currentCopyLen; i_samp++) {
        _a[i_samp] = (kstInterpolateNoHoles(input, inputLen, i_samp + ioffset, inputLen) - mean)*_w[i_samp];
      }
    } else if (removeMean && apodize) {
      for (i_samp = 0; i_samp < currentCopyLen; i_samp++) {
        _a[i_samp] = (input[i_samp + ioffset] - mean)*_w[i_samp];
      }
    } else if (removeMean && interpolateHoles) {
      for (i_samp = 0; i_samp < currentCopyLen; i_samp++) {
        _a[i_samp] = kstInterpolateNoHoles(input, inputLen, i_samp + ioffset, inputLen) - mean;
      }
    } else if (apodize && interpolateHoles) {
      for (i_samp = 0; i_samp < currentCopyLen; i_samp++) {
        _a[i_samp] = kstInterpolateNoHoles(input, inputLen, i_samp + ioffset, inputLen)*_w[i_samp];
      }
    } else if (removeMean) {
      for (i_samp = 0; i_samp < currentCopyLen; i_samp++) {
        _a[i_samp] = input[i_samp + ioffset] - mean;
      }
    } else if (apodize) {
      for (i_samp = 0; i_samp < currentCopyLen; i_samp++) {
        _a[i_samp] = input[i_samp + ioffset]*_w[i_samp];
      }
    } else if (interpolateHoles) {
      for (i_samp = 0; i_samp < currentCopyLen; i_samp++) {
        _a[i_samp] = kstInterpolateNoHoles(input, inputLen, i_samp + ioffset, inputLen);
      }
    } else {
      for (i_samp = 0; i_samp < currentCopyLen; i_samp++) {
        _a[i_samp] = input[i_samp + ioffset];
      }
    }

    nsamples += currentCopyLen;

    rdft(_awLen, 1, _a); //real discrete fourier transorm on _a.

    output[0] += _a[0] * _a[0];
    output[outputLen-1] += _a[1] * _a[1];
    for (i_samp = 1; i_samp < outputLen - 1; i_samp++) {
      output[i_samp] += cabs2(_a[i_samp * 2], _a[i_samp * 2 + 1]);
    }
  }

  // FIXME: NORMALIZATION. 
  /* This normalization doesn't give the same results as the original KstPSD.

  double frequencyStep = .5*(double)inputSamplingFreq/(double)(outputLen-1);

  //normalization factors which were left out earlier for speed. 
  //    - 2.0 for the negative frequencies which were neglected by the rdft //FIXME: double check.
  //    - /(_awLen*_awLen) for the constant Wss from numerical recipes in C. (ensure that the window function agrees with this.)
  //    - /i_subset to average the powers in all the subsets.
  double norm = 2.0/(double)_awLen/(double)_awLen/(double)i_subset;
  */

  // original normalization
  double frequencyStep = 2.0*(double)inputSamplingFreq/(double)nsamples; //OLD value for frequencyStep.
  double norm = 2.0/(double)nsamples*2.0/(double)nsamples; //OLD value for norm.

  switch (outputType) {
  default:
    case PSDAmplitudeSpectralDensity: // amplitude spectral density (default) [V/Hz^1/2]
      norm /= frequencyStep;
      for (i_samp = 0; i_samp < outputLen; i_samp++) {
        output[i_samp] = sqrt(output[i_samp]*norm);
      }
    break;

    case PSDPowerSpectralDensity: // power spectral density [V^2/Hz]
      norm /= frequencyStep;
      for (i_samp = 0; i_samp < outputLen; i_samp++) {
        output[i_samp] *= norm;
      }
    break;

    case PSDAmplitudeSpectrum: // amplitude spectrum [V]
      for (i_samp = 0; i_samp < outputLen; i_samp++) {
        output[i_samp] = sqrt(output[i_samp]*norm);
      }
    break;

    case PSDPowerSpectrum: // power spectrum [V^2]
      for (i_samp = 0; i_samp < outputLen; i_samp++) {
        output[i_samp] *= norm;
      }
    break;
  }

  return 0;
}


int PSDCalculator::calculateOutputVectorLength(int inputLen, bool average, int averageLen) {
  int psdloglen;

  if (average && pow(2.0, averageLen) < inputLen) {
    psdloglen = averageLen;
  } else {
    psdloglen = int(ceil(log(double(inputLen)) / log(2.0)));
  }

  if (psdloglen < PSDMINLEN) {
    psdloglen = PSDMINLEN;
  }

  if (psdloglen > PSDMAXLEN) {
    psdloglen = PSDMAXLEN;
  }

  return int(pow(2, psdloglen - 1));
}

// vim: ts=2 sw=2 et
