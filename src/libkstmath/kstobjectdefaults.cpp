/***************************************************************************
                            kstobjectdefaults.cpp
                             -------------------
    begin                : May 28, 2004
    copyright            : (C) 2004 The University of Toronto
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

#include "kstobjectdefaults.h"
#include "kstdataobjectcollection.h"
#include "kstpsd.h"

#include <kconfiggroup.h>

KstObjectDefaults KST::objectDefaults;

KstObjectDefaults::KstObjectDefaults() {
  _psd_freq = 1.0;
  _fft_len = 10;
  _vUnits = "V";
  _rUnits = "Hz";
  _apodize = true;
  _removeMean = true;
  _psd_average = true;
  _apodizeFxn = 0;
  _output = 0;
  _interpolateHoles = false;
}


void KstObjectDefaults::sync() {
  KstPSDList pl = kstObjectSubList<KstDataObject, KstPSD>(KST::dataObjectList);

  const int j = pl.count() - 1;
  if (j >= 0) {
    pl[j]->readLock();
    _psd_freq = pl[j]->freq();
    _fft_len = pl[j]->len();

    _vUnits = pl[j]->vUnits();
    _rUnits = pl[j]->rUnits();
    _apodize = pl[j]->apodize();
    _removeMean = pl[j]->removeMean();
    _psd_average = pl[j]->average();
    _apodizeFxn = pl[j]->apodizeFxn();
    _output = pl[j]->output();
    _interpolateHoles = pl[j]->interpolateHoles();

    pl[j]->unlock();
  }
}


double KstObjectDefaults::psdFreq() const { 
  return _psd_freq; 
}


int KstObjectDefaults::fftLen() const { 
  return _fft_len; 
}


void KstObjectDefaults::writeConfig(KConfigGroup *config) {
  config->writeEntry("defaultFFTLen", KST::objectDefaults.fftLen());
  config->writeEntry("defaultPSDFreq", KST::objectDefaults.psdFreq());
  config->writeEntry("defaultVUnits", KST::objectDefaults.vUnits());
  config->writeEntry("defaultRUnits", KST::objectDefaults.rUnits());
  config->writeEntry("defaultApodize", KST::objectDefaults.apodize());
  config->writeEntry("defaultRemoveMean", KST::objectDefaults.removeMean());
  config->writeEntry("defaultPSDAverage", KST::objectDefaults.psdAverage());
  config->writeEntry("defaultApodizeFxn", KST::objectDefaults.apodizeFxn());
  config->writeEntry("defaultOutput", KST::objectDefaults.output());
  config->writeEntry("defaultInterpolateHoles", KST::objectDefaults.interpolateHoles());
}


void KstObjectDefaults::readConfig(KConfigGroup *config) {
  _fft_len = config->readEntry("defaultFFTLen", 10);
  _psd_freq = config->readEntry("defaultPSDFreq", 100.0);
  _vUnits = config->readEntry("defaultVUnits", "V");
  _rUnits = config->readEntry("defaultRUnits", "Hz");
  _apodize = config->readEntry("defaultApodize", 1);
  _removeMean = config->readEntry("defaultRemoveMean", 1);
  _psd_average = config->readEntry("defaultPSDAverage", 1);
  _apodizeFxn = config->readEntry("defaultApodizeFxn", 0);
  _output = config->readEntry("defaultOutput", 0);
  _interpolateHoles = config->readEntry("defaultInterpolateHoles", false);
}


// vim: ts=2 sw=2 et
