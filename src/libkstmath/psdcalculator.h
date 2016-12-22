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

#ifndef PSDCALCULATOR_H
#define PSDCALCULATOR_H

// the following should reflect the PSD type order in fftoptionswidget.ui
enum PSDType {
  PSDUndefined = -1,
  PSDAmplitudeSpectralDensity = 0,
  PSDPowerSpectralDensity = 1,
  PSDAmplitudeSpectrum = 2,
  PSDPowerSpectrum = 3
};

// the following should reflect the window type order in fftoptionswidget.ui
enum ApodizeFunction {
  WindowUndefined = -1,
  WindowOriginal = 0,
  WindowBartlett = 1,
  WindowBlackman = 2,
  WindowConnes = 3,
  WindowCosine = 4,
  WindowGaussian = 5,
  WindowHamming = 6,
  WindowHann = 7,
  WindowWelch = 8,
  WindowUniform = 9
};


class PSDCalculator {
  public:
    PSDCalculator();
    ~PSDCalculator();

    int calculatePowerSpectrum(double const *input, int input_len, double *output, int output_len,
                               bool remove_mean, bool average, int average_len,
                               bool apodize, ApodizeFunction apodize_function, double gaussian_sigma,
                               PSDType output_type, double sampling_freq,
                               double const *input2 = 0L, int input2_len = 0, double *output2 = 0L);

    static int calculateOutputVectorLength(int input_len, bool average, int average_len);

  private:
    void updateWindowFxn(ApodizeFunction apodizeFxn, double gaussianSigma);
    void adjustInternalLengths();
    double cabs2(double r, double i);

    double *_a;
    double *_b;
    double *_w;

    int _fft_len; //length of a and w.

    // keep track of prevs to avoid redundant regenerations
    ApodizeFunction _prev_apodize_function;
    double _prev_gaussian_sigma;

    int _prev_output_len;
    bool _prev_cross_spec;
};

#endif
// vim: ts=2 sw=2 et
