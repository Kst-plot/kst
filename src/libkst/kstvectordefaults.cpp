/***************************************************************************
                            kstvectordefaults.cpp
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

#include "kstvectordefaults.h"
#include "datavector.h"
#include "datacollection.h"
#include "stdinsource.h"

#include <qsettings.h>

KstVectorDefaults KST::vectorDefaults;

KstVectorDefaults::KstVectorDefaults() {
  _dataSource = ".";
  _f0 = 0;
  _n = -1;
  _doSkip = false;
  _doAve = false;
  _skip = 0;
}


double KstVectorDefaults::f0() const {
  return _f0;
}


double KstVectorDefaults::n() const {
  return _n;
}


bool KstVectorDefaults::readToEOF() const {
  return _n <= 0;
}


bool KstVectorDefaults::countFromEOF() const {
  return _f0 < 0;
}


const QString& KstVectorDefaults::wizardXVector() const {
  return _wizardX;
}


const QString& KstVectorDefaults::dataSource() const {
  return _dataSource;
}


bool KstVectorDefaults::doSkip() const {
  return _doSkip;
}


bool KstVectorDefaults::doAve() const {
  return _doAve;
}


int KstVectorDefaults::skip() const {
  return _skip;
}


void KstVectorDefaults::sync() {
  Kst::vectorList.lock().readLock();
  Kst::DataVectorList vl = kstObjectSubList<Kst::Vector,Kst::DataVector>(Kst::vectorList);
  Kst::vectorList.lock().unlock();
  int j = vl.count() - 1;

  // Find a non-stdin source
  while (j >= 0) {
    vl[j]->readLock();
    KstDataSourcePtr dsp = vl[j]->dataSource();
    vl[j]->unlock();
#ifdef Q_WS_WIN32
    if (dsp) {
#else
    if (dsp && !kst_cast<KstStdinSource>(dsp)) {
#endif
      break;
    }
    --j;
  }

  if (j >= 0) {
    vl[j]->readLock();
    _f0 = vl[j]->reqStartFrame();
    _n = vl[j]->reqNumFrames();
    _dataSource = vl[j]->filename();
    _skip = vl[j]->skip();
    _doAve = vl[j]->doAve();
    _doSkip = vl[j]->doSkip();
    vl[j]->unlock();
  }
}


void KstVectorDefaults::writeConfig(QSettings *config) {
  config->setValue("defaultDataSource", KST::vectorDefaults.dataSource());
  config->setValue("defaultWizardXVector", KST::vectorDefaults.wizardXVector());
  config->setValue("defaultStartFrame", KST::vectorDefaults.f0());
  config->setValue("defaultNumFrames", KST::vectorDefaults.n());
  config->setValue("defaultDoSkip", KST::vectorDefaults.doSkip());
  config->setValue("defaultDoAve", KST::vectorDefaults.doAve());
  config->setValue("defaultSkip", KST::vectorDefaults.skip());
}


void KstVectorDefaults::readConfig(QSettings *config) {
  _f0 = config->value("defaultStartFrame", 0).toInt();
  _n = config->value("defaultNumFrames", -1).toInt();
  _dataSource = config->value("defaultDataSource", ".").toString();
  _wizardX = config->value("defaultWizardXVector", "INDEX").toString();
  _doSkip = config->value("defaultDoSkip", 0).toInt();
  _doAve = config->value("defaultDoAve", 0).toInt();
  _skip = config->value("defaultSkip", 0).toInt();
}


void KstVectorDefaults::setWizardXVector(const QString& vector) {
  _wizardX = vector;
}

// vim: ts=2 sw=2 et
