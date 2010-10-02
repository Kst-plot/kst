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

    int calculatePowerSpectrum(double *input, int inputLen, double *output, int outputLen, bool removeMean,  bool interpolateHoles, bool average, int averageLen, bool apodize, ApodizeFunction apodizeFxn, double gaussianSigma, PSDType outputType, double inputSamplingFreq);

    static int calculateOutputVectorLength(int inputLen, bool average, int averageLen);

  private:
    void updateWindowFxn(ApodizeFunction apodizeFxn, double gaussianSigma);
    void adjustInternalLengths();
    double cabs2(double r, double i);

    double *_a;
    double *_w;

    int _awLen; //length of a and w.

    // keep track of prevs to avoid redundant regenerations
    ApodizeFunction _prevApodizeFxn;
    double _prevGaussianSigma;

    int _prevOutputLen;
};

#endif
// vim: ts=2 sw=2 et
