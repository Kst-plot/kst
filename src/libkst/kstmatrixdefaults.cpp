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

#include "kstdatacollection.h" 
#include "kstmatrixdefaults.h"
#include "datamatrix.h"
#include "stdinsource.h"

#include <qsettings.h>

KstMatrixDefaults KST::matrixDefaults;

KstMatrixDefaults::KstMatrixDefaults() {
  // some arbitrary defaults for the defaults
  _dataSource = ".";
  _xStart = 0;
  _yStart = 0;
  _xNumSteps = -1;
  _yNumSteps = -1;
  _doSkip = false;
  _doAve = false;
  _skip = 0;
}

const QString& KstMatrixDefaults::dataSource() const {
  return _dataSource;  
}


int KstMatrixDefaults::xStart() const {
  return _xStart;  
}


bool KstMatrixDefaults::xCountFromEnd() const {
  return _xStart < 0;  
}


int KstMatrixDefaults::yStart() const {
  return _yStart;  
}


bool KstMatrixDefaults::yCountFromEnd() const {
  return _yStart < 0;  
}


int KstMatrixDefaults::xNumSteps() const {
  return _xNumSteps;  
}


bool KstMatrixDefaults::xReadToEnd() const {
  return _xNumSteps < 1;  
}


int KstMatrixDefaults::yNumSteps() const {
  return _yNumSteps;  
}


bool KstMatrixDefaults::yReadToEnd() const {
  return _yNumSteps < 1;  
}


bool KstMatrixDefaults::doSkip() const {
  return _doSkip;  
}


bool KstMatrixDefaults::doAverage() const {
  return _doAve;  
}


int KstMatrixDefaults::skip() const {
  return _skip;  
}

void KstMatrixDefaults::sync() {
  KST::matrixList.lock().readLock();
  Kst::DataMatrixList dataMatrixList = kstObjectSubList<KstMatrix,Kst::DataMatrix>(KST::matrixList);
  KST::matrixList.lock().unlock();
  int j = dataMatrixList.count() - 1;

  // Find a non-stdin source
  while (j >= 0) {
    dataMatrixList[j]->readLock();
    KstDataSourcePtr dsp = dataMatrixList[j]->dataSource();
    dataMatrixList[j]->unlock();
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
    dataMatrixList[j]->readLock();
    
    // get the settings
    _dataSource = dataMatrixList[j]->filename();
    _xStart = dataMatrixList[j]->reqXStart();
    _yStart = dataMatrixList[j]->reqYStart();
    _xNumSteps = dataMatrixList[j]->reqXNumSteps();
    _yNumSteps = dataMatrixList[j]->reqYNumSteps();
    _skip = dataMatrixList[j]->skip();
    _doAve = dataMatrixList[j]->doAverage();
    _doSkip = dataMatrixList[j]->doSkip();
    
    dataMatrixList[j]->unlock();
  }
}

void KstMatrixDefaults::writeConfig(QSettings *config) {
  config->setValue("defaultMatrixDataSource", KST::matrixDefaults.dataSource());
  config->setValue("defaultXStart", KST::matrixDefaults.xStart());
  config->setValue("defaultYStart", KST::matrixDefaults.yStart());
  config->setValue("defaultXNumSteps", KST::matrixDefaults.xNumSteps());
  config->setValue("defaultYNumSteps", KST::matrixDefaults.yNumSteps());
  config->setValue("defaultMatrixDoSkip", KST::matrixDefaults.doSkip());
  config->setValue("defaultMatrixDoAverage", KST::matrixDefaults.doAverage());
  config->setValue("defaultMatrixSkip", KST::matrixDefaults.skip());
}


void KstMatrixDefaults::readConfig(QSettings *config) {
  _dataSource = config->value("defaultMatrixDataSource", ".").toString();
  _xStart = config->value("defaultXStart", 0).toInt();
  _yStart = config->value("defaultYStart", 0).toInt();
  _xNumSteps = config->value("defaultXNumSteps", -1).toInt();
  _yNumSteps = config->value("defaultYNumSteps", -1).toInt();
  _doSkip = config->value("defaultMatrixDoSkip", 0).toInt();
  _doAve = config->value("defaultMatrixDoAverage", 0).toInt();
  _skip = config->value("defaultMatrixSkip", 0).toInt();
}


// vim: ts=2 sw=2 et
