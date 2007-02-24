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
#include "kstrmatrix.h"
#include "stdinsource.h"

#include <kconfiggroup.h>

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
  KstRMatrixList rmatrixList = kstObjectSubList<KstMatrix,KstRMatrix>(KST::matrixList);
  KST::matrixList.lock().unlock();
  int j = rmatrixList.count() - 1;

  // Find a non-stdin source
  while (j >= 0) {
    rmatrixList[j]->readLock();
    KstDataSourcePtr dsp = rmatrixList[j]->dataSource();
    rmatrixList[j]->unlock();
    if (dsp && !kst_cast<KstStdinSource>(dsp)) {
      break;
    }
    --j;
  }

  if (j >= 0) {
    rmatrixList[j]->readLock();
    
    // get the settings
    _dataSource = rmatrixList[j]->filename();
    _xStart = rmatrixList[j]->reqXStart();
    _yStart = rmatrixList[j]->reqYStart();
    _xNumSteps = rmatrixList[j]->reqXNumSteps();
    _yNumSteps = rmatrixList[j]->reqYNumSteps();
    _skip = rmatrixList[j]->skip();
    _doAve = rmatrixList[j]->doAverage();
    _doSkip = rmatrixList[j]->doSkip();
    
    rmatrixList[j]->unlock();
  }
}

void KstMatrixDefaults::writeConfig(KConfigGroup *config) {
  config->writeEntry("defaultMatrixDataSource", KST::matrixDefaults.dataSource());
  config->writeEntry("defaultXStart", KST::matrixDefaults.xStart());
  config->writeEntry("defaultYStart", KST::matrixDefaults.yStart());
  config->writeEntry("defaultXNumSteps", KST::matrixDefaults.xNumSteps());
  config->writeEntry("defaultYNumSteps", KST::matrixDefaults.yNumSteps());
  config->writeEntry("defaultMatrixDoSkip", KST::matrixDefaults.doSkip());
  config->writeEntry("defaultMatrixDoAverage", KST::matrixDefaults.doAverage());
  config->writeEntry("defaultMatrixSkip", KST::matrixDefaults.skip());
}


void KstMatrixDefaults::readConfig(KConfigGroup *config) {
  _dataSource = config->readEntry("defaultMatrixDataSource", ".");
  _xStart = config->readEntry("defaultXStart", 0);
  _yStart = config->readEntry("defaultYStart", 0);
  _xNumSteps = config->readEntry("defaultXNumSteps", -1);
  _yNumSteps = config->readEntry("defaultYNumSteps", -1);
  _doSkip = config->readEntry("defaultMatrixDoSkip", 0);
  _doAve = config->readEntry("defaultMatrixDoAverage", 0);
  _skip = config->readEntry("defaultMatrixSkip", 0);
}


// vim: ts=2 sw=2 et
