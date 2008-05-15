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

#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include <QTextDocument>
#include <QXmlStreamWriter>

#include "kst_i18n.h"

#include "datacollection.h"
#include "debug.h"
#include "datamatrix.h"
#include "objectstore.h"


// xStart, yStart < 0             count from end
// xNumSteps, yNumSteps < 1       read to end

namespace Kst {

const QString DataMatrix::staticTypeString = I18N_NOOP("Data Matrix");
const QString DataMatrix::staticTypeTag = I18N_NOOP("datamatrix");

DataMatrix::DataMatrix(ObjectStore *store, const ObjectTag& tag)
  : Matrix(store, tag) {
}

DataMatrix::DataMatrix(ObjectStore *store, DataSourcePtr file, const QString& field, const ObjectTag& tag,
                       int xStart, int yStart,
                       int xNumSteps, int yNumSteps,
                       bool doAve, bool doSkip, int skip,
                       double minX, double minY, double stepX, double stepY)
    : Matrix(store, tag, 0L, 1, 1, 0,0,1,1) {
  commonConstructor(file, field, xStart, yStart, xNumSteps, yNumSteps, doAve, doSkip, skip, minX, minY, stepX, stepY);
}


DataMatrix::DataMatrix(ObjectStore *store, const QDomElement &e)
    : Matrix(store, ObjectTag::invalidTag, 0L, 1,1,0,0,1,1) {
  DataSourcePtr in_file = 0L, in_provider = 0L;
  QString in_field;
  QString in_tag;
  int in_xStart = 0;
  int in_yStart = 0;
  int in_xNumSteps = 0;
  int in_yNumSteps = 1;
  bool in_doAve = false;
  bool in_doSkip = false;
  int in_skip = 1;
  double in_xMin = 0;
  double in_yMin = 0;
  double in_xStep = 1;
  double in_yStep = 1;

  DataSourceList dsList;
  if (this->store()) {
    dsList = this->store()->dataSourceList();
  }

  /* parse the DOM tree */
  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement e = n.toElement();
    if (!e.isNull()) {
      if (e.tagName() == "tag") {
        in_tag = e.text();
      } else if (e.tagName() == "file") {
        in_file = dsList.findFileName(e.text());
      } else if (e.tagName() == "provider") {
        in_provider = dsList.findTag(ObjectTag::fromString(e.text()));
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
      } else if (e.tagName() == "xmin") {
        in_xMin = e.text().toDouble();
      } else if (e.tagName() == "ymin") {
        in_yMin = e.text().toDouble();
      } else if (e.tagName() == "xstep") {
        in_xStep = e.text().toDouble();
      } else if (e.tagName() == "ystep") {
        in_yStep = e.text().toDouble();
      }
    }
    n = n.nextSibling();
  }

  if (in_provider) {
    // provider overrides filename
    in_file = in_provider;
  }

  ObjectTag tag = ObjectTag::fromString(in_tag);
  if (in_file) {
    tag.setContext(in_file->tag().fullTag());
  }
  setTagName(tag);

  // call common constructor
  commonConstructor(in_file, in_field, in_xStart, in_yStart, in_xNumSteps, in_yNumSteps, in_doAve, in_doSkip, in_skip, in_xMin, in_yMin, in_xStep, in_yStep);
}


const QString& DataMatrix::typeString() const {
  return staticTypeString;
}


void DataMatrix::save(QXmlStreamWriter &xml) {
  if (_file) {
    xml.writeStartElement(staticTypeTag);
    xml.writeAttribute("tag", tag().tagString());

    _file->readLock();
    xml.writeAttribute("provider", _file->tag().tagString());
    xml.writeAttribute("file", _file->fileName());
    _file->unlock();

    xml.writeAttribute("field", _field);
    xml.writeAttribute("reqxstart", QString::number(_reqXStart));
    xml.writeAttribute("reqystart", QString::number(_reqYStart));
    xml.writeAttribute("reqnx", QString::number(_reqNX));
    xml.writeAttribute("reqny", QString::number(_reqNY));
    xml.writeAttribute("doave", QVariant(_doAve).toString());
    xml.writeAttribute("doskip", QVariant(_doSkip).toString());
    xml.writeAttribute("skip", QString::number(_skip));
    xml.writeAttribute("xmin", QString::number(minX()));
    xml.writeAttribute("ymin", QString::number(minY()));
    xml.writeAttribute("xstep", QString::number(xStepSize()));
    xml.writeAttribute("ystep", QString::number(yStepSize()));
    saveNameInfo(xml, VNUM|MNUM|XNUM);

    xml.writeEndElement();
  }
}

DataMatrix::~DataMatrix() {
  _file = 0;
}


void DataMatrix::change(DataSourcePtr file, const QString &field,
                        int xStart, int yStart,
                        int xNumSteps, int yNumSteps,
                        bool doAve, bool doSkip, int skip, double minX, double minY,
                        double stepX, double stepY) {
  KstWriteLocker l(this);

  commonConstructor(file, field, xStart, yStart, xNumSteps, yNumSteps, doAve, doSkip, skip, minX, minY, stepX, stepY);

  setDirty(true);
}


void DataMatrix::changeFrames(int xStart, int yStart,
                        int xNumSteps, int yNumSteps,
                        bool doAve, bool doSkip, int skip, double minX, double minY,
                        double stepX, double stepY) {
  KstWriteLocker l(this);

  commonConstructor(_file, _field, xStart, yStart, xNumSteps, yNumSteps, doAve, doSkip, skip, minX, minY, stepX, stepY);

  setDirty(true);
}


int DataMatrix::reqXStart() const {
  return _reqXStart;
}


int DataMatrix::reqYStart() const {
  return _reqYStart;
}


int DataMatrix::reqXNumSteps() const {
  return _reqNX;
}


int DataMatrix::reqYNumSteps() const {
  return _reqNY;
}


QString DataMatrix::filename() const {
  if (_file) {
    return QString(_file->fileName());
  }
  return QString::null;
}


const QString& DataMatrix::field() const {
  return _field;
}


bool DataMatrix::xReadToEnd() const {
  return (_reqNX <= 0);
}


bool DataMatrix::yReadToEnd() const {
  return (_reqNY <= 0);
}


bool DataMatrix::xCountFromEnd() const {
  return (_reqXStart < 0);
}


bool DataMatrix::yCountFromEnd() const {
  return (_reqYStart < 0);
}


QString DataMatrix::label() const {
  bool ok;
  QString returnLabel;

  _field.toInt(&ok);
  if (ok && _file) {
    _file->readLock();
    if (_file->fileType() == "ASCII") {
      returnLabel = i18n("Column %1").arg(_field);
    } else {
      returnLabel = _field;
    }
    _file->unlock();
  } else {
    returnLabel = _field;
  }
  return returnLabel;
}


QString DataMatrix::fileLabel() const {
  return filename();
}


DataSourcePtr DataMatrix::dataSource() const {
  return _file;
}


bool DataMatrix::isValid() const {
  if (_file) {
    _file->readLock();
    bool fieldValid = _file->isValidMatrix(_field);
    _file->unlock();
    return fieldValid;
  }
  return false;
}


Object::UpdateType DataMatrix::update() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  if (_file) {
    _file->writeLock();
  }
  Object::UpdateType rc = doUpdate(force);
  if (_file) {
    _file->unlock();
  }

  setDirty(false);
  return rc;
}


bool DataMatrix::doUpdateSkip(int realXStart, int realYStart, bool force) {

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
  MatrixData matData;

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


bool DataMatrix::doUpdateNoSkip(int realXStart, int realYStart, bool force) {

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
  MatrixData matData;
  matData.z=_z;

  _NS = _file->readMatrix(&matData, _field, realXStart, realYStart, _nX, _nY);

  // set the recommended translate and scaling
  _minX = matData.xMin;
  _minY = matData.yMin;
  _stepX = matData.xStepSize;
  _stepY = matData.yStepSize;

  return true;
}


Object::UpdateType DataMatrix::doUpdate(bool force) {

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

  return Matrix::internalUpdate(UPDATE);
}


void DataMatrix::reload() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (_file) {
    _file->writeLock();
    if (_file->reset()) { // try the efficient way first
      reset();
    } else { // the inefficient way
      DataSourcePtr newsrc = DataSource::loadSource(store(), _file->fileName(), _file->fileType());
      assert(newsrc != _file);
      if (newsrc) {
        _file->unlock();
        // FIXME: need to writelock store?
        if (store()) {
          store()->removeObject(_file);
        }
        _file = newsrc;
        _file->writeLock();
        reset();
      }
    }
    _file->unlock();
  }
}


DataMatrixPtr DataMatrix::makeDuplicate() const {
  Q_ASSERT(store());
  QString newTag = tag().name() + "'";
  DataMatrixPtr matrix = store()->createObject<DataMatrix>(ObjectTag::fromString(newTag));

  matrix->writeLock();
  matrix->change(_file, _field, _reqXStart, _reqYStart, _reqNX, _reqNY, _doAve, _doSkip, _skip, _minX, _minY, _stepX, _stepY);
  matrix->update();
  matrix->unlock();

  return matrix;
}


void DataMatrix::commonConstructor(DataSourcePtr file, const QString &field,
                                   int reqXStart, int reqYStart, int reqNX, int reqNY,
                                   bool doAve, bool doSkip, int skip, double minX, double minY,
                                   double stepX, double stepY) {
  _reqXStart = reqXStart;
  _reqYStart = reqYStart;
  _reqNX = reqNX;
  _reqNY = reqNY;
  _file = file;
  _field = field;
  _doAve = doAve;
  _doSkip = doSkip;
  _skip = skip;
  _minX = minX;
  _minY = minY;
  _stepX = stepX;
  _stepY = stepY;

  _saveable = true;
  _editable = true;

  if (!_file) {
    Debug::self()->log(i18n("Data file for matrix %1 was not opened.", Name()), Debug::Warning);
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


void DataMatrix::reset() { // must be called with a lock
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


bool DataMatrix::doSkip() const {
  return _doSkip;
}


bool DataMatrix::doAverage() const {
  return _doAve;
}


int DataMatrix::skip() const {
  return _skip;
}


void DataMatrix::changeFile(DataSourcePtr file) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (!file) {
    Debug::self()->log(i18n("Data file for vector %1 was not opened.", Name()), Debug::Warning);
  }
  _file = file;
  if (_file) {
    _file->writeLock();
  }
  setTagName(ObjectTag(tag().name(), _file->tag(), false));
  reset();
  if (_file) {
    _file->unlock();
  }
}

QString DataMatrix::_automaticDescriptiveName() const{
  return field();
}

}
// vim: ts=2 sw=2 et
