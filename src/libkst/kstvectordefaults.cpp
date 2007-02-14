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
#include "kstrvector.h"
#include "kstdatacollection.h"
#include "stdinsource.h"

#include <kconfig.h>

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
  KST::vectorList.lock().readLock();
  KstRVectorList vl = kstObjectSubList<KstVector,KstRVector>(KST::vectorList);
  KST::vectorList.lock().unlock();
  int j = vl.count() - 1;

  // Find a non-stdin source
  while (j >= 0) {
    vl[j]->readLock();
    KstDataSourcePtr dsp = vl[j]->dataSource();
    vl[j]->unlock();
    if (dsp && !kst_cast<KstStdinSource>(dsp)) {
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


void KstVectorDefaults::writeConfig(KConfig *config) {
  config->writeEntry("defaultDataSource", KST::vectorDefaults.dataSource());
  config->writeEntry("defaultWizardXVector", KST::vectorDefaults.wizardXVector());
  config->writeEntry("defaultStartFrame", KST::vectorDefaults.f0());
  config->writeEntry("defaultNumFrames", KST::vectorDefaults.n());
  config->writeEntry("defaultDoSkip", KST::vectorDefaults.doSkip());
  config->writeEntry("defaultDoAve", KST::vectorDefaults.doAve());
  config->writeEntry("defaultSkip", KST::vectorDefaults.skip());
}


void KstVectorDefaults::readConfig(KConfig *config) {
  _f0 = config->readDoubleNumEntry("defaultStartFrame", 0);
  _n = config->readDoubleNumEntry("defaultNumFrames", -1);
  _dataSource = config->readEntry("defaultDataSource", ".");
  _wizardX = config->readEntry("defaultWizardXVector", "INDEX");
  _doSkip = config->readNumEntry("defaultDoSkip", 0);
  _doAve = config->readNumEntry("defaultDoAve", 0);
  _skip = config->readNumEntry("defaultSkip", 0);
}


void KstVectorDefaults::setWizardXVector(const QString& vector) {
  _wizardX = vector;
}

// vim: ts=2 sw=2 et
