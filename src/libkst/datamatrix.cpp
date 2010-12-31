/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *   copyright : (C) 2005  University of British Columbia                  *
 *                   dscott@phas.ubc.ca                                    *
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
#include <QVariant>

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



DataMatrix::DataInfo::DataInfo() :
    samplesPerFrame(-1),
    xSize(-1),
    ySize(-1),
    invertXHint(false),
    invertYHint(false)
{
}




DataMatrix::DataMatrix(ObjectStore *store)
  : Matrix(store) {
  _dp = new DataPrimitive(this);

}


const QString& DataMatrix::typeString() const {
  return staticTypeString;
}


void DataMatrix::save(QXmlStreamWriter &xml) {
  if (_dp->dataSource()) {
    xml.writeStartElement(staticTypeTag);

    _dp->saveFilename(xml);

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
}


void DataMatrix::change(DataSourcePtr file, const QString &field,
                        int xStart, int yStart,
                        int xNumSteps, int yNumSteps,
                        bool doAve, bool doSkip, int skip, double minX, double minY,
                        double stepX, double stepY) {
  KstWriteLocker l(this);

  commonConstructor(file, field, xStart, yStart, xNumSteps, yNumSteps, doAve, doSkip, skip, minX, minY, stepX, stepY);
}


void DataMatrix::changeFrames(int xStart, int yStart,
                        int xNumSteps, int yNumSteps,
                        bool doAve, bool doSkip, int skip, double minX, double minY,
                        double stepX, double stepY) {
  KstWriteLocker l(this);

  commonConstructor(_dp->dataSource(), _field, xStart, yStart, xNumSteps, yNumSteps, doAve, doSkip, skip, minX, minY, stepX, stepY);
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


/*
QString DataMatrix::filename() const {
  if (_dp->dataSource()) {
    return QString(_dp->dataSource()->fileName());
  }
  return QString::null;
}
*/

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
  if (ok && _dp->dataSource()) {
    _dp->dataSource()->readLock();
    if (_dp->dataSource()->fileType() == "ASCII") {
      returnLabel = i18n("Column %1").arg(_field);
    } else {
      returnLabel = _field;
    }
    _dp->dataSource()->unlock();
  } else {
    returnLabel = _field;
  }
  return returnLabel;
}


bool DataMatrix::isValid() const {
  if (_dp->dataSource()) {
    _dp->dataSource()->readLock();
    bool fieldValid = _dp->dataSource()->matrix().isValid(_field);
    _dp->dataSource()->unlock();
    return fieldValid;
  }
  return false;
}


bool DataMatrix::_checkValidity(const DataSourcePtr ds) const {
  if (ds) {
    ds->readLock();
    bool rc = ds->matrix().isValid(_dp->_field);
    ds->unlock();
    return rc;
  }
  return false;
}


void DataMatrix::doUpdateSkip(int realXStart, int realYStart) {

  // since we are skipping, we don't need all the pixels
  // also, samples per frame is always 1 with skipping
  _nX = _nX / _skip;
  _nY = _nY / _skip;

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
    _NS = readMatrix(&matData, _field, realXStart, realYStart, _nX, _nY, _skip);

    // -9999 means the skipping function is not supported by datasource
    if (_NS != -9999) {
      // set the recommended translate and scaling, and return
      _minX = matData.xMin;
      _minY = matData.yMin;
      _stepX = matData.xStepSize;
      _stepY = matData.yStepSize;
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
        readMatrix(&matData, _field, realXStart + _skip*i, realYStart + _skip*j, _skip, _skip, -1);
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
        int samples = readMatrix(&matData, _field, realXStart + _skip*i, realYStart + _skip*j, -1, -1, -1);
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
}


void DataMatrix::doUpdateNoSkip(int realXStart, int realYStart) {

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

  _NS = readMatrix(&matData, _field, realXStart, realYStart, _nX, _nY, -1);

  // set the recommended translate and scaling
  _minX = matData.xMin;
  _minY = matData.yMin;
  _stepX = matData.xStepSize;
  _stepY = matData.yStepSize;
}

qint64 DataMatrix::minInputSerial() const {
  if (_dp->dataSource()) {
    return (_dp->dataSource()->serial());
  }
  return LLONG_MAX;
}

qint64 DataMatrix::minInputSerialOfLastChange() const {
  if (_dp->dataSource()) {
    return (_dp->dataSource()->serialOfLastChange());
  }
  return LLONG_MAX;
}

void DataMatrix::_resetFieldMetadata() {
  _resetFieldScalars();
  _resetFieldStrings();
}

void DataMatrix::_resetFieldScalars() {

}

void DataMatrix::_resetFieldStrings() {
  const QMap<QString, QString> meta_strings = _dp->dataSource()->matrix().metaStrings(_field);

  QStringList fieldStringKeys = _fieldStrings.keys();
  // remove field strings that no longer need to exist
  readLock();
  for (int i=0; i<fieldStringKeys.count(); i++) {
    QString key = fieldStringKeys.at(i);
    if (!meta_strings.contains(key)) {
      StringPtr sp = _fieldStrings[key];
      _fieldStrings.remove(key);
      sp = 0L;
    }
  }
  // find or insert strings, to set their value
  QMapIterator<QString, QString> it(meta_strings);
  while (it.hasNext()) {
    it.next();
    QString key = it.key();
    StringPtr sp;
    if (!_fieldStrings.contains(key)) { // insert a new one
      _fieldStrings.insert(key, sp = store()->createObject<String>());
      sp->setProvider(this);
      sp->setSlaveName(key);
#ifndef KST_USE_QSHAREDPOINTER
      sp->_KShared_ref();
#endif
    } else {  // find it
      sp = _fieldStrings[key];
    }
    sp->setValue(it.value());
  }
  unlock();
}

QString DataMatrix::xLabel() const {
  QString label;

  if (_fieldStrings.contains("x_quantity")) {
    label = _fieldStrings.value("x_quantity")->value();
  }

  if (!label.isEmpty()) {
    if (_fieldStrings.contains("x_units")) {
      QString units = _fieldStrings.value("x_units")->value();
      if (!units.isEmpty()) {
        label += " \\[" + units + "\\]";
      }
    }
  }

  return label;

}

QString DataMatrix::yLabel() const {
  QString label;

  if (_fieldStrings.contains("y_quantity")) {
    label = _fieldStrings.value("y_quantity")->value();
  }

  if (!label.isEmpty()) {
    if (_fieldStrings.contains("y_units")) {
      QString units = _fieldStrings.value("y_units")->value();
      if (!units.isEmpty()) {
        label += " \\[" + units + "\\]";
      }
    }
  }

  return label;
}

void DataMatrix::internalUpdate() {
  if (_dp->dataSource()) {
    _dp->dataSource()->writeLock();
  } else {
    return;
  }

  // see if we can turn off skipping (only check if skipping enabled)
  if (_doSkip && _samplesPerFrameCache == 1 && _skip < 2) {
    _doSkip = false;
  }

  // first get the real start and end range
  int realXStart;
  int realYStart;

  const DataInfo info = _dp->dataSource()->matrix().dataInfo(_field);
  int xSize = info.xSize;
  int ySize = info.ySize;

  _invertXHint = info.invertXHint;
  _invertYHint = info.invertYHint;

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
    doUpdateSkip(realXStart, realYStart);
  } else {
    doUpdateNoSkip(realXStart, realYStart);
  }

  // remember these as the last updated range
  _lastXStart = realXStart;
  _lastYStart = realYStart;
  _lastNX = _nX;
  _lastNY = _nY;
  _lastDoAve = _doAve;
  _lastDoSkip = _doSkip;
  _lastSkip = _skip;

  _dp->dataSource()->unlock();

  Matrix::internalUpdate();
}


void DataMatrix::reload() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (_dp->dataSource()) {
    _dp->dataSource()->writeLock();
    _dp->dataSource()->reset();
    _dp->dataSource()->unlock();
    reset();
  }
}


PrimitivePtr DataMatrix::_makeDuplicate() const {
  Q_ASSERT(store());
  DataMatrixPtr matrix = store()->createObject<DataMatrix>();

  matrix->writeLock();
  matrix->change(_dp->dataSource(), _field, _reqXStart, _reqYStart, _reqNX, _reqNY, _doAve, _doSkip, _skip, _minX, _minY, _stepX, _stepY);
  if (descriptiveNameIsManual()) {
    matrix->setDescriptiveName(descriptiveName());
  }
  matrix->registerChange();
  matrix->unlock();

  return kst_cast<Primitive>(matrix);
}


void DataMatrix::commonConstructor(DataSourcePtr in_file, const QString &field,
                                   int reqXStart, int reqYStart, int reqNX, int reqNY,
                                   bool doAve, bool doSkip, int skip, double minX, double minY,
                                   double stepX, double stepY) {
  _reqXStart = reqXStart;
  _reqYStart = reqYStart;
  _reqNX = reqNX;
  _reqNY = reqNY;
  _dp->setDataSource(in_file);
  _field = field;
  _doAve = doAve;
  _doSkip = doSkip;
  _skip = skip;
  _minX = minX;
  _minY = minY;
  _stepX = stepX;
  _stepY = stepY;
  _invertXHint = false;
  _invertYHint = false;

  _saveable = true;
  _editable = true;

  if (!_dp->dataSource()) {
    Debug::self()->log(i18n("Data file for matrix %1 was not opened.", Name()), Debug::Warning);
  } else {
    const DataInfo info = _dp->dataSource()->matrix().dataInfo(_field);
    _samplesPerFrameCache = info.samplesPerFrame;
    _invertXHint = info.invertXHint;
    _invertYHint = info.invertYHint;
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

  _resetFieldMetadata();

}


void DataMatrix::reset() { // must be called with a lock
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (_dp->dataSource()) {
    const DataInfo info = _dp->dataSource()->matrix().dataInfo(_field);
    _samplesPerFrameCache = info.samplesPerFrame;
    _invertXHint = info.invertXHint;
    _invertYHint = info.invertYHint;
  }
  resizeZ(0);
  _NS = 0;
  _nX = 1;
  _nY = 0;
  _resetFieldMetadata();
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


void DataMatrix::changeFile(DataSourcePtr in_file) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  if (!in_file) {
    Debug::self()->log(i18n("Data file for vector %1 was not opened.", Name()), Debug::Warning);
  }
  _dp->setDataSource(in_file);
  if (_dp->dataSource()) {
    _dp->dataSource()->writeLock();
  }
  reset();
  if (_dp->dataSource()) {
    _dp->dataSource()->unlock();
  }
}

QString DataMatrix::_automaticDescriptiveName() const{
  QString name = field();
  // un-escape escaped special characters so they aren't escaped 2x.
  name.replace("\\_", "_").replace("\\^","^").replace("\\[", "[").replace("\\]", "]");
  // now escape the special characters.
  name.replace('_', "\\_").replace('^', "\\^").replace('[', "\\[").replace(']', "\\]");

  return name;
}

QString DataMatrix::descriptionTip() const {
  return i18n(
      "Data Matrix: %1\n"
      "  %2\n"
      "  Field: %3\n"
      "  %4 x %5"
      ).arg(Name()).arg(_dp->dataSource()->fileName()).arg(field()).arg(_nX).arg(_nY);
}

QString DataMatrix::propertyString() const {
  return i18n("%1 of %2").arg(field()).arg(_dp->dataSource()->fileName());
}


int DataMatrix::readMatrix(MatrixData* data, const QString& matrix, int xStart, int yStart, int xNumSteps, int yNumSteps, int skip)
{
  ReadInfo p = { data, xStart, yStart, xNumSteps, yNumSteps, skip};
  return _dp->dataSource()->matrix().read(matrix, p);
}


}
// vim: ts=2 sw=2 et
