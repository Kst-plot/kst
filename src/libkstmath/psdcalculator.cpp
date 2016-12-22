/***************************************************************************
                     psdcalculator.cpp: Power Spectra Calculator for KST
                             -------------------
    begin                : 2006
    copyright            : (C) 2006 by Kst
    email                : netterfield@astro.utoronto.ca
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

#include "psdcalculator.h"

#include <assert.h>



#include "debug.h"
#include "vector.h"

#include <qnamespace.h>
#include <math_kst.h>
#include "measuretime.h"

extern "C" void rdft(int n, int isgn, double *a);

#define PSDMINLEN 2
#define PSDMAXLEN 27

inline double PSDCalculator::cabs2(double r, double i) {
  return r*r + i*i;
}

PSDCalculator::PSDCalculator()
{
  _a = 0L;
  _b = 0L;
  _w = 0L;

  _fft_len = 0;

  _prev_apodize_function = WindowUndefined;
  _prev_gaussian_sigma = 1.0;
  _prev_output_len = 0;
  _prev_cross_spec = false;
}


PSDCalculator::~PSDCalculator() {
  delete[] _w;
  _w = 0L;
  delete[] _a;
  _a = 0L;
  delete[] _b;
  _b = 0L;
}

void PSDCalculator::updateWindowFxn(ApodizeFunction apodizeFxn, double gaussianSigma) {
  const double a = double(_fft_len) / 2.0;
  double x;
  double sW = 0.0;

  switch (apodizeFxn) {
    case WindowOriginal: 
      for (int i = 0; i < _fft_len; ++i) {
        _w[i] = 1.0 - cos(2.0 * M_PI * double(i) / double(_fft_len));
        sW += _w[i] * _w[i];
      }
      break;

    case WindowBartlett:
      for (int i = 0; i < _fft_len; ++i) {
        x = i - a;
        _w[i] = 1.0 - fabs(x) / a;
        sW += _w[i] * _w[i];
      }
      break;
 
    case WindowBlackman:
      for (int i = 0; i < _fft_len; ++i) {
        x = i - a;
        _w[i] = 0.42 + 0.5 * cos(M_PI * x / a) + 0.08 * cos(2 * M_PI * x/a);
        sW += _w[i] * _w[i];
      }
      break;

    case WindowConnes: 
      for (int i = 0; i < _fft_len; ++i) {
        x = i - a;
        _w[i] = pow(static_cast<double>(1.0 - (x * x) / (a * a)), 2);
        sW += _w[i] * _w[i];
      }
      break;

    case WindowCosine:
      for (int i = 0; i < _fft_len; ++i) {
        x = i - a;
        _w[i] = cos(M_PI * x / (2.0 * a));
        sW += _w[i] * _w[i];
      }
      break;

    case WindowGaussian:
      for (int i = 0; i < _fft_len; ++i) {
        x = i - a;
        _w[i] = exp(-1.0 * x * x/(2.0 * gaussianSigma * gaussianSigma));
      }
      break;

    case WindowHamming:
      for (int i = 0; i < _fft_len; ++i) {
        x = i - a;
        _w[i] = 0.53836 + 0.46164 * cos(M_PI * x / a);
        sW += _w[i] * _w[i];
      }
      break;

    case WindowHann:
      for (int i = 0; i < _fft_len; ++i) {
        x = i - a;
        _w[i] = pow(static_cast<double>(cos(M_PI * x/(2.0 * a))), 2);
        sW += _w[i] * _w[i];
      }
      break;

    case WindowWelch:
      for (int i = 0; i < _fft_len; ++i) {
        x = i - a;
        _w[i] = 1.0 - x * x / (a * a);
        sW += _w[i] * _w[i];
      }
      break;

    case WindowUniform:
    default:
      for (int i = 0; i < _fft_len; ++i) {
        _w[i] = 1.0;
      }
      sW = _fft_len;
      break;
  }

  double norm = sqrt((double)_fft_len/sW); // normalization constant s.t. sum over (w^2) is _awLen

  for (int i = 0; i < _fft_len; ++i) {
    _w[i] *= norm;
  }

  _prev_apodize_function = apodizeFxn;
  _prev_gaussian_sigma = gaussianSigma;
}


int PSDCalculator::calculatePowerSpectrum(
  double const *input, int input_len,
  double *output, int output_len,
  bool removeMean,
  bool average, int average_len,
  bool apodize, ApodizeFunction apodize_function, double gaussian_sigma,
  PSDType output_type, double sampling_freq,
  double const *input2, int input2_len, double *output2) {

  bool cross_spectra = false;

  if ((input2_len == input_len) && input2) {
    cross_spectra = true;
  }

  if ((input2) && (input2_len == input_len)) {
    cross_spectra = true;
  }

  if (output_len != calculateOutputVectorLength(input_len, average, average_len)) {
    Kst::Debug::self()->log(Kst::Debug::tr("in PSDCalculator::calculatePowerSpectrum: received output array with wrong length."), Kst::Debug::Error);
    return -1;
  }

  if (output_len != _prev_output_len) {
    delete[] _a;
    delete[] _w;
    delete[] _b;

    _fft_len = output_len*2;
    _prev_output_len = output_len;

    _a = new double[_fft_len];
    _b = new double[_fft_len];
    _w = new double[_fft_len];

    updateWindowFxn(apodize_function, gaussian_sigma);
  }

  if ( (_prev_apodize_function != apodize_function) || (_prev_gaussian_sigma != gaussian_sigma) ) {
    updateWindowFxn(apodize_function, gaussian_sigma);
  }

  int currentCopyLen;
  int nsamples = 0;
  int i_samp;
  int ioffset;

  memset(output, 0, sizeof(double)*output_len); // initialize output.
  if (cross_spectra) {
    memset(output2, 0, sizeof(double)*output_len); // initialize complex output for xspectra.
  }

  // Mingw build could be 10 times slower (Gaussian apod, mostly 0 then?)
  //MeasureTime time_in_rfdt("rdft()");

  bool done = false;
  for (int i_subset = 0; !done; i_subset++) {
    ioffset = i_subset*output_len; //overlapping average => i_subset*outputLen

    // only zero pad if we really have to.  It is better to adjust the last chunk's
    // overlap.
    if (ioffset + _fft_len*5/4 < input_len) {
      currentCopyLen = _fft_len; //will copy a complete window.
    } else if (_fft_len<input_len) {  // count the last one from the end.
      ioffset = input_len-_fft_len - 1;
      currentCopyLen = _fft_len; //will copy a complete window.
      done = true;
    } else {
      currentCopyLen = input_len - ioffset; //will copy a partial window.
      memset(&_a[currentCopyLen], 0, sizeof(double)*(_fft_len - currentCopyLen)); //zero the leftovers.
      if (cross_spectra) {
        memset(&_b[currentCopyLen], 0, sizeof(double)*(_fft_len - currentCopyLen)); //zero the leftovers.
      }
      done = true;
    }

    double mean = 0.0;
    double mean2 = 0.0;

    if (removeMean) {
      for (i_samp = 0; i_samp < currentCopyLen; ++i_samp) {
        mean += input[i_samp + ioffset];
      }
      mean /= (double)currentCopyLen;
      if (cross_spectra) {
        for (i_samp = 0; i_samp < currentCopyLen; ++i_samp) {
          mean2 += input2[i_samp + ioffset];
        }
        mean2 /= (double)currentCopyLen;
      }
    }

    // apply the PSD options (removeMean, apodize, etc.)
    // separate cases for speed- although this shouldn't really matter- the rdft should be the most time consuming step by far for any large data set.
    if (removeMean && apodize) {
      for (i_samp = 0; i_samp < currentCopyLen; ++i_samp) {
        _a[i_samp] = (input[i_samp + ioffset] - mean)*_w[i_samp];
      }
    } else if (removeMean) {
      for (i_samp = 0; i_samp < currentCopyLen; ++i_samp) {
        _a[i_samp] = input[i_samp + ioffset] - mean;
      }
    } else if (apodize) {
      for (i_samp = 0; i_samp < currentCopyLen; ++i_samp) {
        _a[i_samp] = input[i_samp + ioffset]*_w[i_samp];
      }
    } else {
      for (i_samp = 0; i_samp < currentCopyLen; ++i_samp) {
        _a[i_samp] = input[i_samp + ioffset];
      }
    }

    if (cross_spectra) {
      if (removeMean && apodize) {
        for (i_samp = 0; i_samp < currentCopyLen; ++i_samp) {
          _b[i_samp] = (input2[i_samp + ioffset] - mean2)*_w[i_samp];
        }
      } else if (removeMean) {
        for (i_samp = 0; i_samp < currentCopyLen; ++i_samp) {
          _b[i_samp] = input2[i_samp + ioffset] - mean2;
        }
      } else if (apodize) {
        for (i_samp = 0; i_samp < currentCopyLen; ++i_samp) {
          _b[i_samp] = input2[i_samp + ioffset]*_w[i_samp];
        }
      } else {
        for (i_samp = 0; i_samp < currentCopyLen; ++i_samp) {
          _b[i_samp] = input2[i_samp + ioffset];
        }
      }
    }

    nsamples += currentCopyLen;

#if !defined(__QNX__)
    rdft(_fft_len, 1, _a); //real discrete fourier transorm on _a.
    if (cross_spectra) {
      rdft(_fft_len, 1, _b); //real discrete fourier transorm on _b.
    }
#else
    Q_ASSERT(0); // there is a linking problem when not compling with pch. . .
#endif

    if (cross_spectra) {
      output[0] += _a[0] * _b[0];
      output2[0] = 0;
      output[output_len-1] += _a[1] * _b[1];
      output2[output_len-1] = 0;
      for (i_samp = 1; i_samp < output_len - 1; ++i_samp) {
        output[i_samp] += _a[i_samp*2] * _b[i_samp*2] +
            _a[i_samp*2+1] * _b[i_samp*2+1];
        output2[i_samp] = -_a[i_samp*2] * _b[i_samp*2+1] +
            _a[i_samp*2+1] * _b[i_samp*2];
      }
    } else {
      output[0] += _a[0] * _a[0];
      output[output_len-1] += _a[1] * _a[1];
      for (i_samp = 1; i_samp < output_len - 1; ++i_samp) {
        output[i_samp] += cabs2(_a[i_samp * 2], _a[i_samp * 2 + 1]);
      }
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
  double frequencyStep = 2.0*(double)sampling_freq/(double)nsamples; //OLD value for frequencyStep.
  double norm = 2.0/(double)nsamples*2.0/(double)nsamples; //OLD value for norm.

  switch (output_type) {
  default:
    case PSDAmplitudeSpectralDensity: // amplitude spectral density (default) [V/Hz^1/2]
      norm /= frequencyStep;
      for (i_samp = 0; i_samp < output_len; ++i_samp) {
        output[i_samp] = sqrt(output[i_samp]*norm);
      }
    break;

    case PSDPowerSpectralDensity: // power spectral density [V^2/Hz]
      norm /= frequencyStep;
      for (i_samp = 0; i_samp < output_len; ++i_samp) {
        output[i_samp] *= norm;
      }
      if (cross_spectra) {
        for (i_samp = 0; i_samp < output_len; ++i_samp) {
          output2[i_samp] *= norm;
        }
      }
    break;

    case PSDAmplitudeSpectrum: // amplitude spectrum [V]
      for (i_samp = 0; i_samp < output_len; ++i_samp) {
        output[i_samp] = sqrt(output[i_samp]*norm);
      }
    break;

    case PSDPowerSpectrum: // power spectrum [V^2]
      for (i_samp = 0; i_samp < output_len; ++i_samp) {
        output[i_samp] *= norm;
      }
      if (cross_spectra) {
        for (i_samp = 0; i_samp < output_len; ++i_samp) {
          output2[i_samp] *= norm;
        }
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

  return int(pow(2.0, psdloglen - 1));
}

// vim: ts=2 sw=2 et
