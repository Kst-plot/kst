/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *   copyright : (C) 2005 by University of British Columbia                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/** A class for handling cumulative spectral decays for kst
 */

#include <assert.h>
#include <math.h>

#include <QXmlStreamWriter>
#include <QLatin1String>

#include "kst_i18n.h"

#include "dialoglauncher.h"
#include "csd.h"
#include "datacollection.h"
#include "debug.h"
#include "psdcalculator.h"
#include "objectstore.h"

extern "C" void rdft(int n, int isgn, double *a);

namespace Kst {

const QString CSD::staticTypeString = I18N_NOOP("Cumulative Spectral Decay");
const QString CSD::staticTypeTag = I18N_NOOP("csd");

static const QLatin1String INVECTOR = QLatin1String("I");
static const QLatin1String& OUTMATRIX = QLatin1String("M");

#define KSTCSDMAXLEN 27
CSD::CSD(ObjectStore *store)
  : DataObject(store) {
  _typeString = staticTypeString;
  _type = "Cumulative Spectral Decay";
}


void CSD::change(VectorPtr in_V, double in_freq, bool in_average,
    bool in_removeMean, bool in_apodize, ApodizeFunction in_apodizeFxn,
    int in_windowSize, int in_length, double in_gaussianSigma,
    PSDType in_outputType, const QString& in_vectorUnits,
    const QString& in_rateUnits) {

  if (_inputVectors[INVECTOR]) {
    disconnect(_inputVectors[INVECTOR], SIGNAL(vectorUpdated(ObjectPtr)));
  }

  _inputVectors[INVECTOR] = in_V;
  QString vecName = in_V ? in_V->Name() : QString::null;
  _frequency = in_freq;
  _average = in_average;
  _apodize = in_apodize;
  _windowSize = in_windowSize;
  _apodizeFxn = in_apodizeFxn;
  _gaussianSigma = in_gaussianSigma;
  _removeMean = in_removeMean;
  _averageLength = in_length;
  _vectorUnits = in_vectorUnits;
  _rateUnits = in_rateUnits;
  _outputType = in_outputType;

  if (_frequency <= 0.0) {
    _frequency = 1.0;
  }

  Q_ASSERT(store());
  MatrixPtr outMatrix = store()->createObject<Matrix>();
  outMatrix->setProvider(this);
  outMatrix->setSlaveName("SG");
  outMatrix->change(1, 1);
  outMatrix->setLabel(i18n("Power [%1/%2^{1/2}]").arg(_vectorUnits).arg(_rateUnits));
  outMatrix->setXLabel(i18n("%1 [%2]").arg(vecName).arg(_vectorUnits));
  outMatrix->setYLabel(i18n("Frequency [%1]").arg(_rateUnits));
  _outMatrix = _outputMatrices.insert(OUTMATRIX, outMatrix).value();

  updateMatrixLabels();
  _outMatrix->setDirty();
  setDirty();
  _shortName = "G"+QString::number(_csdnum);
  if (_csdnum>max_csdnum) 
    max_csdnum = _csdnum;
  _csdnum++;

  connect(_inputVectors[INVECTOR], SIGNAL(vectorUpdated(ObjectPtr)), this, SLOT(inputObjectUpdated(ObjectPtr)));

  setDirty();
}


CSD::~CSD() {
  _outMatrix = 0L;
}

Object::UpdateType CSD::update() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  VectorPtr inVector = _inputVectors[INVECTOR];

  bool force = dirty();
  setDirty(false);

  writeLockInputsAndOutputs();

  bool xUpdated = Object::UPDATE == inVector->update();
  // if vector was not changed, don't update the CSD
  if (!xUpdated && !force) {
    unlockInputsAndOutputs();
    return NO_CHANGE;
  }

  double *tempOutput, *input;
  int tempOutputLen = PSDCalculator::calculateOutputVectorLength(_windowSize, _average, _averageLength);
  _length = tempOutputLen;
  tempOutput = new double[tempOutputLen];

  input = inVector->value();

  int xSize = 0;
  for (int i=0; i < inVector->length(); i+= _windowSize) {
    //ensure there is enough data left.
    if (i + _windowSize >= inVector->length()) {
        break; //If there isn't enough left for a complete window.
    }

    _psdCalculator.calculatePowerSpectrum(input + i, _windowSize, tempOutput, tempOutputLen, _removeMean,  false, _average, _averageLength, _apodize, _apodizeFxn, _gaussianSigma, _outputType, _frequency);

    // resize output matrix
    _outMatrix->resize(xSize+1, tempOutputLen);

    if (_outMatrix->sampleCount() == (xSize+1)*tempOutputLen) { // all is well.
      // copy elements to output matrix
      for (int j=0; j < tempOutputLen; j++) {
        _outMatrix->setValueRaw(xSize, j, tempOutput[j]);
      }
    } else {
      Debug::self()->log(i18n("Could not allocate sufficient memory for CSD."), Debug::Error);
      break;
    }

    xSize++;
  }

  delete[] tempOutput;

  double frequencyStep = .5*_frequency/(double)(tempOutputLen-1);

  _outMatrix->change(xSize, tempOutputLen, 0, 0, _windowSize, frequencyStep);
  _outMatrix->update();

  unlockInputsAndOutputs();

  return UPDATE;
}


void CSD::save(QXmlStreamWriter &s) {
  s.writeStartElement(staticTypeTag);
  s.writeAttribute("vector", _inputVectors[INVECTOR]->Name());
  s.writeAttribute("samplerate", QString::number(_frequency));
  s.writeAttribute("gaussiansigma", QString::number(_gaussianSigma));
  s.writeAttribute("average", QVariant(_average).toString());
  s.writeAttribute("fftlength", QString::number(int(ceil(log(double(_length*2)) / log(2.0)))));
  s.writeAttribute("removemean", QVariant(_removeMean).toString());
  s.writeAttribute("apodize", QVariant(_apodize).toString());
  s.writeAttribute("apodizefunction", QString::number(_apodizeFxn));
  s.writeAttribute("windowsize", QString::number(_windowSize));
  s.writeAttribute("vectorunits", _vectorUnits);
  s.writeAttribute("rateunits", _rateUnits);
  s.writeAttribute("outputtype", QString::number(_outputType));
  saveNameInfo(s,VNUM|XNUM|MNUM|CSDNUM);

  s.writeEndElement();
}


void CSD::setVector(VectorPtr new_v) {
  VectorPtr v = _inputVectors[INVECTOR];
  if (v) {
    if (v == new_v) {
      return;
    }
    v->unlock();
  }

  _inputVectors.remove(INVECTOR);
  new_v->writeLock();
  _inputVectors[INVECTOR] = new_v;
  setDirty();
}


VectorPtr CSD::vector() const {
  return _inputVectors[INVECTOR];
}


bool CSD::slaveVectorsUsed() const {
  return true;
}


QString CSD::propertyString() const {
  return i18n("CSD: %1").arg(_inputVectors[INVECTOR]->Name());
}


void CSD::showNewDialog() {
  DialogLauncher::self()->showCSDDialog();
}


void CSD::showEditDialog() {
  DialogLauncher::self()->showCSDDialog(this);
}


bool CSD::apodize() const {
  return _apodize;
}


PSDType CSD::output() const {
  return _outputType;
}


void CSD::setOutput(PSDType in_outputType)  {
  setDirty();
  _outputType = in_outputType;

  updateMatrixLabels();
}


void CSD::setApodize(bool in_apodize)  {
  setDirty();
  _apodize = in_apodize;
}


bool CSD::removeMean() const {
  return _removeMean;
}


void CSD::setRemoveMean(bool in_removeMean) {
  setDirty();
  _removeMean = in_removeMean;
}


bool CSD::average() const {
  return _average;
}


void CSD::setAverage(bool in_average) {
  setDirty();
  _average = in_average;
}


double CSD::frequency() const {
  return _frequency;
}


void CSD::setFrequency(double in_frequency) {
  setDirty();
  if (in_frequency > 0.0) {
    _frequency = in_frequency;
  } else {
    _frequency = 1.0;
  }
}

ApodizeFunction CSD::apodizeFxn() const {
  return _apodizeFxn;
}

void CSD::setApodizeFxn(ApodizeFunction in_fxn) {
  setDirty();
  _apodizeFxn = in_fxn;
}

int CSD::length() const {
  return _averageLength;
}

void CSD::setLength(int in_length) {
  _averageLength = in_length;
}


int CSD::windowSize() const {
  return _windowSize;
}


void CSD::setWindowSize(int in_size) {
  setDirty();
  _windowSize = in_size;
}

double CSD::gaussianSigma() const {
  return _gaussianSigma;
}

void CSD::setGaussianSigma(double in_sigma) {
  setDirty();
  _gaussianSigma = in_sigma;
}


MatrixPtr CSD::outputMatrix() const {
  return _outMatrix;
}


const QString& CSD::vectorUnits() const {
  return _vectorUnits;
}


void CSD::setVectorUnits(const QString& units) {
  _vectorUnits = units;
}


const QString& CSD::rateUnits() const {
  return _rateUnits;
}


void CSD::setRateUnits(const QString& units) {
  _rateUnits = units;
}


DataObjectPtr CSD::makeDuplicate() {

  CSDPtr csd = store()->createObject<CSD>();
  csd->change(_inputVectors[INVECTOR],
              _frequency,
              _average,
              _removeMean,
              _apodize,
              _apodizeFxn,
              _windowSize,
              _averageLength,
              _gaussianSigma,
              _outputType,
              _vectorUnits,
              _rateUnits);
  if (descriptiveNameIsManual()) {
    csd->setDescriptiveName(descriptiveName());
  }
  csd->writeLock();
  csd->update();
  csd->unlock();

  return DataObjectPtr(csd);
}

void CSD::updateMatrixLabels(void) {
    switch (_outputType) {
    default:
    case 0: // amplitude spectral density (default) [V/Hz^1/2]
      _outMatrix->setLabel(i18n("ASD [%1/%2^{1/2}]").arg(_vectorUnits).arg(_rateUnits));
      break;
    case 1: // power spectral density [V^2/Hz]
      _outMatrix->setLabel(i18n("PSD [%1^2/%2]").arg(_vectorUnits).arg(_rateUnits));
      break;
    case 2: // amplitude spectrum [V]
      _outMatrix->setLabel(i18n("Amplitude Spectrum [%1]").arg(_vectorUnits));
      break;
    case 3: // power spectrum [V^2]
      _outMatrix->setLabel(i18n("Power Spectrum [%1^2]").arg(_vectorUnits));
      break;
  }
}

QString CSD::_automaticDescriptiveName() const {
  return vector()->descriptiveName();
}

}

// vim: ts=2 sw=2 et
