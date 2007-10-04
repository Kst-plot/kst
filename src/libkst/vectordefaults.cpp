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

#include "vectordefaults.h"
#include "datavector.h"
#include "datacollection.h"
#include "stdinsource.h"

#include <qsettings.h>

namespace Kst {

VectorDefaults vectorDefaults;

VectorDefaults::VectorDefaults() {
  _dataSource = ".";
  _f0 = 0;
  _n = -1;
  _doSkip = false;
  _doAve = false;
  _skip = 0;
}


double VectorDefaults::f0() const {
  return _f0;
}


double VectorDefaults::n() const {
  return _n;
}


bool VectorDefaults::readToEOF() const {
  return _n <= 0;
}


bool VectorDefaults::countFromEOF() const {
  return _f0 < 0;
}


const QString& VectorDefaults::wizardXVector() const {
  return _wizardX;
}


const QString& VectorDefaults::dataSource() const {
  return _dataSource;
}


bool VectorDefaults::doSkip() const {
  return _doSkip;
}


bool VectorDefaults::doAve() const {
  return _doAve;
}


int VectorDefaults::skip() const {
  return _skip;
}


void VectorDefaults::sync() {
  vectorList.lock().readLock();
  DataVectorList vl = ObjectSubList<Vector,DataVector>(vectorList);
  vectorList.lock().unlock();
  int j = vl.count() - 1;

  // Find a non-stdin source
  while (j >= 0) {
    vl[j]->readLock();
    DataSourcePtr dsp = vl[j]->dataSource();
    vl[j]->unlock();
#ifdef Q_WS_WIN32
    if (dsp) {
#else
    if (dsp && !kst_cast<StdinSource>(dsp)) {
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


void VectorDefaults::writeConfig(QSettings *config) {
  config->setValue("defaultDataSource", vectorDefaults.dataSource());
  config->setValue("defaultWizardXVector", vectorDefaults.wizardXVector());
  config->setValue("defaultStartFrame", vectorDefaults.f0());
  config->setValue("defaultNumFrames", vectorDefaults.n());
  config->setValue("defaultDoSkip", vectorDefaults.doSkip());
  config->setValue("defaultDoAve", vectorDefaults.doAve());
  config->setValue("defaultSkip", vectorDefaults.skip());
}


void VectorDefaults::readConfig(QSettings *config) {
  _f0 = config->value("defaultStartFrame", 0).toInt();
  _n = config->value("defaultNumFrames", -1).toInt();
  _dataSource = config->value("defaultDataSource", ".").toString();
  _wizardX = config->value("defaultWizardXVector", "INDEX").toString();
  _doSkip = config->value("defaultDoSkip", 0).toInt();
  _doAve = config->value("defaultDoAve", 0).toInt();
  _skip = config->value("defaultSkip", 0).toInt();
}


void VectorDefaults::setWizardXVector(const QString& vector) {
  _wizardX = vector;
}

}
// vim: ts=2 sw=2 et
