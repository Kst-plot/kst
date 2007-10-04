/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *   copyright : (C) 2005  University of British Columbia                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "datacollection.h"
#include "matrixdefaults.h"
#include "datamatrix.h"
#include "stdinsource.h"

#include <qsettings.h>

namespace Kst {

MatrixDefaults matrixDefaults;

MatrixDefaults::MatrixDefaults() {
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

const QString& MatrixDefaults::dataSource() const {
  return _dataSource;  
}


int MatrixDefaults::xStart() const {
  return _xStart;  
}


bool MatrixDefaults::xCountFromEnd() const {
  return _xStart < 0;  
}


int MatrixDefaults::yStart() const {
  return _yStart;  
}


bool MatrixDefaults::yCountFromEnd() const {
  return _yStart < 0;  
}


int MatrixDefaults::xNumSteps() const {
  return _xNumSteps;  
}


bool MatrixDefaults::xReadToEnd() const {
  return _xNumSteps < 1;  
}


int MatrixDefaults::yNumSteps() const {
  return _yNumSteps;  
}


bool MatrixDefaults::yReadToEnd() const {
  return _yNumSteps < 1;  
}


bool MatrixDefaults::doSkip() const {
  return _doSkip;  
}


bool MatrixDefaults::doAverage() const {
  return _doAve;  
}


int MatrixDefaults::skip() const {
  return _skip;  
}

void MatrixDefaults::sync() {
  matrixList.lock().readLock();
  DataMatrixList dataMatrixList = ObjectSubList<Matrix,DataMatrix>(matrixList);
  matrixList.lock().unlock();
  int j = dataMatrixList.count() - 1;

  // Find a non-stdin source
  while (j >= 0) {
    dataMatrixList[j]->readLock();
    DataSourcePtr dsp = dataMatrixList[j]->dataSource();
    dataMatrixList[j]->unlock();
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

void MatrixDefaults::writeConfig(QSettings *config) {
  config->setValue("defaultMatrixDataSource", matrixDefaults.dataSource());
  config->setValue("defaultXStart", matrixDefaults.xStart());
  config->setValue("defaultYStart", matrixDefaults.yStart());
  config->setValue("defaultXNumSteps", matrixDefaults.xNumSteps());
  config->setValue("defaultYNumSteps", matrixDefaults.yNumSteps());
  config->setValue("defaultMatrixDoSkip", matrixDefaults.doSkip());
  config->setValue("defaultMatrixDoAverage", matrixDefaults.doAverage());
  config->setValue("defaultMatrixSkip", matrixDefaults.skip());
}


void MatrixDefaults::readConfig(QSettings *config) {
  _dataSource = config->value("defaultMatrixDataSource", ".").toString();
  _xStart = config->value("defaultXStart", 0).toInt();
  _yStart = config->value("defaultYStart", 0).toInt();
  _xNumSteps = config->value("defaultXNumSteps", -1).toInt();
  _yNumSteps = config->value("defaultYNumSteps", -1).toInt();
  _doSkip = config->value("defaultMatrixDoSkip", 0).toInt();
  _doAve = config->value("defaultMatrixDoAverage", 0).toInt();
  _skip = config->value("defaultMatrixSkip", 0).toInt();
}

}
// vim: ts=2 sw=2 et
