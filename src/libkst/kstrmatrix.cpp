/***************************************************************************
                          kstrmatrix.cpp  -  matrix from file
                             -------------------
    begin                : 2005
    copyright            : (C) 2005 by University of British Columbia
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *   Permission is granted to link with any opensource library             *
 *                                                                         *
 ***************************************************************************/

#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <qtextdocument.h>

#include <klocale.h>

#include "kstdatacollection.h"
#include "kstdebug.h"
#include "kstrmatrix.h"


// xStart, yStart < 0             count from end
// xNumSteps, yNumSteps < 1       read to end

KstRMatrix::KstRMatrix(KstDataSourcePtr file, const QString &field, KstObjectTag tag,
                       int xStart, int yStart,
                       int xNumSteps, int yNumSteps,
                       bool doAve, bool doSkip, int skip)
: KstMatrix(tag, 0L, 1, 1, 0,0,1,1) {
  commonConstructor(file, field, xStart, yStart, xNumSteps, yNumSteps, doAve, doSkip, skip);
}


KstRMatrix::KstRMatrix(const QDomElement &e) : KstMatrix(KstObjectTag::invalidTag, 0L, 1,1,0,0,1,1) {
  KstDataSourcePtr in_file = 0L, in_provider = 0L;
  QString in_field;
  QString in_tag;
  int in_xStart = 0;
  int in_yStart = 0;
  int in_xNumSteps = 0;
  int in_yNumSteps = 1;
  bool in_doAve = false;
  bool in_doSkip = false;
  int in_skip = 1;

  /* parse the DOM tree */
  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement e = n.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "tag") {
        in_tag = e.text();
      } else if (e.tagName() == "file") {
        KST::dataSourceList.lock().readLock();
        in_file = *KST::dataSourceList.findFileName(e.text());
        KST::dataSourceList.lock().unlock();
      } else if (e.tagName() == "provider") {
        KST::dataSourceList.lock().readLock();
        in_provider = *KST::dataSourceList.findTag(e.text());
        KST::dataSourceList.lock().unlock();
      } else if (e.tagName() == "field") {
        in_field = e.text();
      } else if (e.tagName() == "reqxstart") {
        in_xStart = e.text().toInt();
      } else if (e.tagName() == "reqystart") {
        in_yStart = e.text().toInt();
      } else if (e.tagName() == "reqnx") {
        in_xNumSteps = e.text().toInt();
      } else if (e.tagName() == "reqny") {
        in_yNumSteps = e.text().toInt();
      } else if (e.tagName() == "doave") {
        in_doAve = (e.text() != "0");
      } else if (e.tagName() == "doskip") {
        in_doSkip = (e.text() != "0");
      } else if (e.tagName() == "skip") {
        in_skip = e.text().toInt();
      }
    }
    n = n.nextSibling();
  }

  if (in_provider) {
    // provider overrides filename
    in_file = in_provider;
  }

  KstObjectTag tag = KstObjectTag::fromString(in_tag);
  if (in_file) {
    tag.setContext(in_file->tag().fullTag());
  }
  setTagName(tag);

  // call common constructor
  commonConstructor(in_file, in_field, in_xStart, in_yStart, in_xNumSteps, in_yNumSteps, in_doAve, in_doSkip, in_skip);
}


void KstRMatrix::save(QTextStream &ts, const QString& indent) {
  if (_file) {

    QString indent2 = "  ";

    ts << indent << "<rmatrix>" << endl;
    ts << indent << indent2 << "<tag>" << Qt::escape(tag().tagString()) << "</tag>" << endl;
    _file->readLock();
    ts << indent << indent2 << "<provider>" << Qt::escape(_file->tag().tagString()) << "</provider>" << endl;
    ts << indent << indent2 << "<file>" << Qt::escape(_file->fileName()) << "</file>" << endl;
    _file->unlock();
    ts << indent << indent2 << "<field>" << _field << "</field>" << endl;
    ts << indent << indent2 << "<reqxstart>" << _reqXStart << "</reqxstart>" << endl;
    ts << indent << indent2 << "<reqystart>" << _reqYStart << "</reqystart>" << endl;
    ts << indent << indent2 << "<reqnx>" << _reqNX << "</reqnx>" << endl;
    ts << indent << indent2 << "<reqny>" << _reqNY << "</reqny>" << endl;
    ts << indent << indent2 << "<doave>" << _doAve << "</doave>" << endl;
    ts << indent << indent2 << "<doskip>" << _doSkip << "</doskip>" << endl;
    ts << indent << indent2 << "<skip>" << _skip << "</skip>" << endl;
    ts << indent << "</rmatrix>" << endl;
  }
}

KstRMatrix::~KstRMatrix() {
  _file = 0;
}


void KstRMatrix::change(KstDataSourcePtr file, const QString &field,
                        KstObjectTag tag,
                        int xStart, int yStart,
                        int xNumSteps, int yNumSteps,
                        bool doAve, bool doSkip, int skip) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  setTagName(tag);
  commonConstructor(file, field, xStart, yStart, xNumSteps, yNumSteps, doAve, doSkip, skip);

  setDirty(true);
}


int KstRMatrix::reqXStart() const {
  return _reqXStart;
}


int KstRMatrix::reqYStart() const {
  return _reqYStart;
}


int KstRMatrix::reqXNumSteps() const {
  return _reqNX;
}


int KstRMatrix::reqYNumSteps() const {
  return _reqNY;
}


QString KstRMatrix::filename() const {
  if (_file) {
    return QString(_file->fileName());
  }
  return QString::null;
}


const QString& KstRMatrix::field() const {
  return _field;
}


bool KstRMatrix::xReadToEnd() const {
  return (_reqNX <= 0);
}


bool KstRMatrix::yReadToEnd() const {
  return (_reqNY <= 0);
}


bool KstRMatrix::xCountFromEnd() const {
  return (_reqXStart < 0);
}


bool KstRMatrix::yCountFromEnd() const {
  return (_reqYStart < 0);
}


QString KstRMatrix::label() const {
  bool ok;
  QString returnLabel;

  _field.toInt(&ok);
  if (ok && _file) {
    _file->readLock();
    if (_file->fileType() == "ASCII") {
      returnLabel = QString("Column %1").arg(_field);
    } else {
      returnLabel = _field;
    }
    _file->unlock();
  } else {
    returnLabel = _field;
  }
  return returnLabel;
}


QString KstRMatrix::fileLabel() const {
  return filename();
}


KstDataSourcePtr KstRMatrix::dataSource() const {
  return _file;
}


bool KstRMatrix::isValid() const {
  if (_file) {
    _file->readLock();
    bool fieldValid = _file->isValidMatrix(_field);
    _file->unlock();
    return fieldValid;
  }
  return false;
}


KstObject::UpdateType KstRMatrix::update(int update_counter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);
  if (KstObject::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  if (_file) {
    _file->writeLock();
  }
  KstObject::UpdateType rc = doUpdate(force);
  if (_file) {
    _file->unlock();
  }

  setDirty(false);
  return setLastUpdateResult(rc);
}


bool KstRMatrix::doUpdateSkip(int realXStart, int realYStart, bool force) {

  // since we are skipping, we don't need all the pixels
  // also, samples per frame is always 1 with skipping
  _nX = _nX / _skip;
  _nY = _nY / _skip;

  // unless we are forced to, don't update if the range is the same
  if (realXStart == _lastXStart && realYStart == _lastYStart && _nX == _lastNX && _nY == _lastNY &&
      _doAve == _lastDoAve && _doSkip == _lastDoSkip && _skip == _lastSkip && !force) {
    return false;
  }

  // resize the array if necessary
  int requiredSize = _nX * _nY;
  if (requiredSize != _zSize) {
    bool resizeOK = resizeZ(requiredSize);
    if (!resizeOK) {
      abort(); // FIXME: what to do?
    }
  }

  // return data from readMatrix
  KstMatrixData matData;

  if (!_doAve) {
    // try to use the datasource's read with skip function - it will automatically
    // enlarge each pixel to correct for the skipping
    matData.z=_z;
    _NS = _file->readMatrix(&matData, _field, realXStart, realYStart, _nX, _nY, _skip);

    // -9999 means the skipping function is not supported by datasource
    if (_NS != -9999) {
      // set the recommended translate and scaling, and return
      _minX = matData.xMin;
      _minY = matData.yMin;
      _stepX = matData.xStepSize;
      _stepY = matData.yStepSize;
      return true;
    }
  }

  // the skipping function is not supported by datasource; we need to manually skip
  if (_doAve) {
    // boxcar filtering is not supported by datasources currently; need to manually average
    if (_aveReadBufferSize < _samplesPerFrameCache*_skip*_samplesPerFrameCache*_skip) {
      _aveReadBufferSize = _samplesPerFrameCache*_skip*_samplesPerFrameCache*_skip;
      _aveReadBuffer = static_cast<double*>(realloc(_aveReadBuffer, _aveReadBufferSize*sizeof(double)));
    }
    _NS = 0;
    bool first = true;
    matData.z = _aveReadBuffer;
    double* zPos = _z;
    for (int i = 0; i < _nX; i++) {
      for (int j = 0; j < _nY; j++) {
        // read one buffer size in
        _file->readMatrix(&matData, _field, realXStart + _skip*i, realYStart + _skip*j, _skip, _skip);
        // take average of the buffer
        double bufferAverage = 0;
        for (int k = 0; k < _samplesPerFrameCache*_skip*_samplesPerFrameCache*_skip; k++) {
          bufferAverage += _aveReadBuffer[k];
        }
        bufferAverage = bufferAverage / _aveReadBufferSize;
        // insert the average into the matrix
        *zPos = bufferAverage;
        zPos++;
        _NS++;
        if (first) {
          _minX = matData.xMin;
          _minY = matData.yMin;
          _stepX = matData.xStepSize * _skip * _samplesPerFrameCache;
          _stepY = matData.yStepSize * _skip * _samplesPerFrameCache;
          first = false;
        }
      }
    }

  } else {
    _NS = 0;
    bool first = true;
    for (int i = 0; i < _nX; i++) {
      for (int j = 0; j < _nY; j++) {
        // read one sample
        int samples = _file->readMatrix(&matData, _field, realXStart + _skip*i, realYStart + _skip*j, -1, -1);
        matData.z += samples;
        _NS += samples;
        if (first) {
          _minX = matData.xMin;
          _minY = matData.yMin;
          _stepX = matData.xStepSize * _skip * _samplesPerFrameCache;
          _stepY = matData.yStepSize * _skip * _samplesPerFrameCache;
          first = false;
        }
      }
    }
  }

  return true;
}


bool KstRMatrix::doUpdateNoSkip(int realXStart, int realYStart, bool force) {

  // unless we are forced to, don't update if the range is the same
  if (realXStart == _lastXStart && realYStart == _lastYStart && _nX == _lastNX && _nY == _lastNY &&
      _doAve == _lastDoAve && _doSkip == _lastDoSkip && _skip == _lastSkip && !force) {
    return false;
  }

  // resize _z if necessary
  int requiredSize = _nX*_nY*_samplesPerFrameCache*_samplesPerFrameCache;
  if (requiredSize != _zSize) {
    bool resizeOK = resizeZ(requiredSize);
    if (!resizeOK) {
      abort(); // FIXME: what to do?
    }
  }
  // read new data from file
  KstMatrixData matData;
  matData.z=_z;

  _NS = _file->readMatrix(&matData, _field, realXStart, realYStart, _nX, _nY);

  // set the recommended translate and scaling
  _minX = matData.xMin;
  _minY = matData.yMin;
  _stepX = matData.xStepSize;
  _stepY = matData.yStepSize;

  return true;
}


KstObject::UpdateType KstRMatrix::doUpdate(bool force) {

  if (!_file) {
    return NO_CHANGE;
  }

  // see if we can turn off skipping (only check if skipping enabled)
  if (_doSkip && _samplesPerFrameCache == 1 && _skip < 2) {
    _doSkip = false;
  }

  // first get the real start and end range
  int xSize, ySize, realXStart, realYStart;

  _file->matrixDimensions(_field, &xSize, &ySize);

  if (_reqXStart < 0) {
    // counting from end
    realXStart = xSize - _reqNX;
  } else {
    realXStart = _reqXStart;
  }
  if (_reqYStart < 0) {
    // counting from end
    realYStart = ySize - _reqNY;
  } else {
    realYStart = _reqYStart;
  }
  if (_reqNX < 1) {
    // read to end
    _nX = xSize - _reqXStart;
  } else {
    _nX = _reqNX;
  }
  if (_reqNY < 1) {
    // read to end
    _nY = ySize - _reqYStart;
  } else {
    _nY = _reqNY;
  }

  // now do some sanity checks
  if (realXStart > xSize - 1) {
    realXStart = xSize - 1;
  }
  if (realXStart < 0) {
    realXStart = 0;
  }
  if (realYStart > ySize - 1) {
    realYStart = ySize - 1;
  }
  if (realYStart < 0) {
    realYStart = 0;
  }
  if (_nX < 1) {
    _nX = 1;
  }
  if (realXStart + _nX > xSize) {
    _nX = xSize - realXStart;
  }
  if (_nY < 1) {
    _nY = 1;
  }
  if (realYStart + _nY > ySize) {
    _nY = ySize - realYStart;
  }

  // do the reading; skip or non-skip version
  if (_doSkip) {
    if (!doUpdateSkip(realXStart, realYStart, force)) {
      return NO_CHANGE;
    }
  } else {
    if (!doUpdateNoSkip(realXStart, realYStart, force)) {
      return NO_CHANGE;
    }
  }

  // remember these as the last updated range
  _lastXStart = realXStart;
  _lastYStart = realYStart;
  _lastNX = _nX;
  _lastNY = _nY;
  _lastDoAve = _doAve;
  _lastDoSkip = _doSkip;
  _lastSkip = _skip;

  return KstMatrix::internalUpdate(UPDATE);
}


void KstRMatrix::reload() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (_file) {
    _file->writeLock();
    if (_file->reset()) { // try the efficient way first
      reset();
    } else { // the inefficient way
      KstDataSourcePtr newsrc = KstDataSource::loadSource(_file->fileName(), _file->fileType());
      assert(newsrc != _file);
      if (newsrc) {
        _file->unlock();
        KST::dataSourceList.lock().writeLock();
        KST::dataSourceList.removeAll(_file);
        _file = newsrc;
        _file->writeLock();
        KST::dataSourceList.append(_file);
        KST::dataSourceList.lock().unlock();
        reset();
      }
    }
    _file->unlock();
  }
}


KstRMatrixPtr KstRMatrix::makeDuplicate() const {
  QString newTag = tag().tag() + "'";
  return new KstRMatrix(_file, _field, KstObjectTag(newTag, tag().context()),
                        _reqXStart, _reqYStart, _reqNX, _reqNY,
                        _doAve, _doSkip, _skip);
}


void KstRMatrix::commonConstructor(KstDataSourcePtr file, const QString &field,
                                   int reqXStart, int reqYStart, int reqNX, int reqNY,
                                   bool doAve, bool doSkip, int skip) {
//  qDebug() << "constructing KstRMatrix " << tag().displayString() << " from file " << file->tag().displayString() << " (" << (void*)(&(*file)) << ")" << endl;
  _reqXStart = reqXStart;
  _reqYStart = reqYStart;
  _reqNX = reqNX;
  _reqNY = reqNY;
  _file = file;
  _field = field;
  _doAve = doAve;
  _doSkip = doSkip;
  _skip = skip;

  _saveable = true;
  _editable = true;

  if (!_file) {
    KstDebug::self()->log(QString("Data file for matrix %1 was not opened.").arg(tagName()), KstDebug::Warning);
  } else {
    _samplesPerFrameCache = _file->samplesPerFrame(_field);
  }

  _aveReadBuffer = 0L;
  _aveReadBufferSize = 0;
  _lastXStart = 0;
  _lastYStart = 0;
  _lastNX = 1;
  _lastNY = 1;
  _lastDoAve = false;
  _lastDoSkip = false;;
  _lastSkip = 1;
}


void KstRMatrix::reset() { // must be called with a lock
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);
  
  if (_file) {
    _samplesPerFrameCache = _file->samplesPerFrame(_field);
  }
  resizeZ(0);
  _NS = 0;
  _nX = 1;
  _nY = 0;
  setDirty();
}


bool KstRMatrix::doSkip() const {
  return _doSkip;
}


bool KstRMatrix::doAverage() const {
  return _doAve;
}


int KstRMatrix::skip() const {
  return _skip;
}


void KstRMatrix::changeFile(KstDataSourcePtr file) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (!file) {
    KstDebug::self()->log(QString("Data file for vector %1 was not opened.").arg(tagName()), KstDebug::Warning);
  }
  _file = file;
  if (_file) {
    _file->writeLock();
  }
  setTagName(KstObjectTag(tag().tag(), _file->tag(), false));
  reset();
  if (_file) {
    _file->unlock();
  }
}

// vim: ts=2 sw=2 et
