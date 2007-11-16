/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2004 The University of Toronto                        *
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QSettings>

#include "objectdefaults.h"
#include "dataobjectcollection.h"
#include "psd.h"
#include "objectstore.h"

namespace Kst {

ObjectDefaults objectDefaults;

ObjectDefaults::ObjectDefaults() {
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


void ObjectDefaults::sync() {
  Q_ASSERT(_store);

  PSDList pl = _store->getObjects<PSD>();

  const int j = pl.count() - 1;
  if (j >= 0) {
    pl[j]->readLock();
    _psd_freq = pl[j]->frequency();
    _fft_len = pl[j]->length();

    _vUnits = pl[j]->vectorUnits();
    _rUnits = pl[j]->rateUnits();
    _apodize = pl[j]->apodize();
    _removeMean = pl[j]->removeMean();
    _psd_average = pl[j]->average();
    _apodizeFxn = pl[j]->apodizeFxn();
    _output = pl[j]->output();
    _interpolateHoles = pl[j]->interpolateHoles();

    pl[j]->unlock();
  }
}


double ObjectDefaults::psdFreq() const {
  return _psd_freq;
}


int ObjectDefaults::fftLen() const {
  return _fft_len; 
}


void ObjectDefaults::writeConfig(QSettings *config) {
  config->setValue("defaultFFTLen", objectDefaults.fftLen());
  config->setValue("defaultPSDFreq", objectDefaults.psdFreq());
  config->setValue("defaultVUnits", objectDefaults.vUnits());
  config->setValue("defaultRUnits", objectDefaults.rUnits());
  config->setValue("defaultApodize", objectDefaults.apodize());
  config->setValue("defaultRemoveMean", objectDefaults.removeMean());
  config->setValue("defaultPSDAverage", objectDefaults.psdAverage());
  config->setValue("defaultApodizeFxn", objectDefaults.apodizeFxn());
  config->setValue("defaultOutput", objectDefaults.output());
  config->setValue("defaultInterpolateHoles", objectDefaults.interpolateHoles());
}


void ObjectDefaults::readConfig(QSettings *config) {
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

}
// vim: ts=2 sw=2 et
