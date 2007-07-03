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

#include <qsettings.h>

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


void KstObjectDefaults::writeConfig(QSettings *config) {
  config->setValue("defaultFFTLen", KST::objectDefaults.fftLen());
  config->setValue("defaultPSDFreq", KST::objectDefaults.psdFreq());
  config->setValue("defaultVUnits", KST::objectDefaults.vUnits());
  config->setValue("defaultRUnits", KST::objectDefaults.rUnits());
  config->setValue("defaultApodize", KST::objectDefaults.apodize());
  config->setValue("defaultRemoveMean", KST::objectDefaults.removeMean());
  config->setValue("defaultPSDAverage", KST::objectDefaults.psdAverage());
  config->setValue("defaultApodizeFxn", KST::objectDefaults.apodizeFxn());
  config->setValue("defaultOutput", KST::objectDefaults.output());
  config->setValue("defaultInterpolateHoles", KST::objectDefaults.interpolateHoles());
}


void KstObjectDefaults::readConfig(QSettings *config) {
  _fft_len = config->value("defaultFFTLen", 10).toInt();
  _psd_freq = config->value("defaultPSDFreq", 100.0).toDouble();
  _vUnits = config->value("defaultVUnits", "V").toString();
  _rUnits = config->value("defaultRUnits", "Hz").toString();
  _apodize = config->value("defaultApodize", 1).toInt();
  _removeMean = config->value("defaultRemoveMean", 1).toInt();
  _psd_average = config->value("defaultPSDAverage", 1).toInt();
  _apodizeFxn = config->value("defaultApodizeFxn", 0).toInt();
  _output = config->value("defaultOutput", 0).toInt();
  _interpolateHoles = config->value("defaultInterpolateHoles", false).toBool();
}


// vim: ts=2 sw=2 et
