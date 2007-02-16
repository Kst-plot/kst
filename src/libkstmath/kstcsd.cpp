
/***************************************************************************
                          kstcsd.cpp: Cumulative Spectral Decay for KST
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
 *                                                                         *
 ***************************************************************************/

/** A class for handling cumulative spectral decays for kst
 */

#include <assert.h>
#include <math.h>

#include <q3stylesheet.h>

#include <kglobal.h>
#include <klocale.h>

#include "dialoglauncher.h"
#include "kstcsd.h"
#include "kstdatacollection.h"
#include "kstdebug.h"
#include "psdcalculator.h"
#include "kstobjectdefaults.h"

extern "C" void rdft(int n, int isgn, double *a);

static const QString& INVECTOR = KGlobal::staticQString("I");
static const QString& OUTMATRIX = KGlobal::staticQString("M");

#define KSTCSDMAXLEN 27
KstCSD::KstCSD(const QString &in_tag, KstVectorPtr in_V,
               double in_freq, bool in_average, bool in_removeMean, bool in_apodize, 
               ApodizeFunction in_apodizeFxn, int in_windowSize, int in_averageLength, double in_gaussianSigma, 
               PSDType in_outputType, const QString &in_vectorUnits, const QString &in_rateUnits)
: KstDataObject() {
  commonConstructor(in_tag, in_V, in_freq, in_average, in_removeMean,
                    in_apodize, in_apodizeFxn, in_windowSize, in_averageLength, in_gaussianSigma, 
                    in_vectorUnits, in_rateUnits, in_outputType, in_V->tagName());
  setDirty();
}


KstCSD::KstCSD(const QDomElement &e)
: KstDataObject(e) {
  
    QString in_tag;
    QString vecName;
    QString in_vectorUnits, in_rateUnits;
    KstVectorPtr in_V;
    double in_freq = 60.0;
    bool in_average = true;
    int in_averageLength = 8;
    bool in_removeMean = true;
    bool in_apodize = true;
    ApodizeFunction in_apodizeFxn = WindowOriginal;
    int in_windowSize = 5000;
    double in_gaussianSigma = 3.0;
    PSDType in_outputType = PSDAmplitudeSpectralDensity;
    
    QDomNode n = e.firstChild();
    while (!n.isNull()) {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if (!e.isNull()) { // the node was really an element.
        if (e.tagName() == "tag") {
          in_tag = e.text();
        } else if (e.tagName() == "vectag") {
          vecName = e.text();
        } else if (e.tagName() == "sampRate") {
          in_freq = e.text().toDouble();
        } else if (e.tagName() == "average") {
          in_average = (e.text() != "0");
        } else if (e.tagName() == "fftLen") {
          in_averageLength = e.text().toInt();
        } else if (e.tagName() == "apodize") {
          in_apodize = (e.text() != "0");
        } else if (e.tagName() == "apodizefxn") {
          in_apodizeFxn = ApodizeFunction(e.text().toInt());
        } else if (e.tagName() == "gaussiansigma") {
          in_gaussianSigma = e.text().toDouble();
        } else if (e.tagName() == "removeMean") {
          in_removeMean = (e.text() != "0");
        } else if (e.tagName() == "windowsize") {
          in_windowSize = e.text().toInt();
        } else if (e.tagName() == "vectorunits") {
          in_vectorUnits = e.text();
        } else if (e.tagName() == "rateunits") {
          in_rateUnits = e.text();
        } else if (e.tagName() == "output") {
          in_outputType = (PSDType)e.text().toInt();
        }
      }
      n = n.nextSibling();
    }
    
    _inputVectorLoadQueue.append(qMakePair(INVECTOR, vecName));
    
    commonConstructor(in_tag, in_V, in_freq, in_average, in_removeMean,
                      in_apodize, in_apodizeFxn, in_windowSize, in_averageLength, in_gaussianSigma,
                      in_vectorUnits, in_rateUnits, in_outputType, vecName);
}


void KstCSD::commonConstructor(const QString& in_tag, KstVectorPtr in_V,
                               double in_freq, bool in_average, bool in_removeMean, bool in_apodize, 
                               ApodizeFunction in_apodizeFxn, int in_windowSize, int in_averageLength, 
                               double in_gaussianSigma, const QString& in_vectorUnits, 
                               const QString& in_rateUnits, PSDType in_outputType, const QString& vecName) {
  _typeString = i18n("Cumulative Spectral Decay");
  _type = "Cumulative Spectral Decay";
  _inputVectors[INVECTOR] = in_V;
  setTagName(KstObjectTag::fromString(in_tag));
  _frequency = in_freq;
  _average = in_average;
  _apodize = in_apodize;
  _windowSize = in_windowSize;
  _apodizeFxn = in_apodizeFxn;
  _gaussianSigma = in_gaussianSigma;
  _removeMean = in_removeMean;
  _averageLength = in_averageLength;
  _vectorUnits = in_vectorUnits;
  _rateUnits = in_rateUnits;
  _outputType = in_outputType;

  if (_frequency <= 0.0) {
    _frequency = 1.0;
  }
 
  {
    KstWriteLocker blockMatrixUpdates(&KST::matrixList.lock());

    KstMatrixPtr outMatrix = new KstMatrix(KstObjectTag("csd", tag()), this, 1, 1);
    outMatrix->setLabel(i18n("Power [%1/%2^{1/2}]").arg(_vectorUnits).arg(_rateUnits));
    outMatrix->setXLabel(i18n("%1 [%2]").arg(vecName).arg(_vectorUnits));
    outMatrix->setYLabel(i18n("Frequency [%1]").arg(_rateUnits));
    _outMatrix = _outputMatrices.insert(OUTMATRIX, outMatrix);
  }

  updateMatrixLabels();
  (*_outMatrix)->setDirty();
}


KstCSD::~KstCSD() {
  _outMatrix = _outputMatrices.end();
  KST::matrixList.lock().writeLock();
  KST::matrixList.remove(_outputMatrices[OUTMATRIX]);
  KST::matrixList.lock().unlock();
}

KstObject::UpdateType KstCSD::update(int update_counter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  KstVectorPtr inVector = _inputVectors[INVECTOR];

  bool force = dirty();
  setDirty(false);

  if (KstObject::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  writeLockInputsAndOutputs();

  if (update_counter <= 0) {
    assert(update_counter == 0);
    force = true;
  }

  bool xUpdated = KstObject::UPDATE == inVector->update(update_counter);
  // if vector was not changed, don't update the CSD
  if (!xUpdated && !force) {
    unlockInputsAndOutputs();
    return setLastUpdateResult(NO_CHANGE);
  }

  double *tempOutput, *input;
  int tempOutputLen = PSDCalculator::calculateOutputVectorLength(_windowSize, _average, _averageLength);
  _PSDLen = tempOutputLen;
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
    (*_outMatrix)->resize(xSize+1, tempOutputLen);

    if ((*_outMatrix)->sampleCount() == (xSize+1)*tempOutputLen) { // all is well.
      // copy elements to output matrix
      for (int j=0; j < tempOutputLen; j++) {
        (*_outMatrix)->setValueRaw(xSize, j, tempOutput[j]);
      }
    } else {
      KstDebug::self()->log(i18n("Could not allocate sufficient memory for CSD."), KstDebug::Error);
      break;
    }

    xSize++;
  }

  delete[] tempOutput;

  double frequencyStep = .5*_frequency/(double)(tempOutputLen-1);

  (*_outMatrix)->change((*_outMatrix)->tag(), xSize, tempOutputLen, 0, 0, _windowSize, frequencyStep);
  (*_outMatrix)->update(update_counter);

  unlockInputsAndOutputs();

  return setLastUpdateResult(UPDATE);
}

void KstCSD::save(Q3TextStream &ts, const QString& indent) {
  QString l2 = indent + "  ";
  ts << indent << "<csdobject>" << endl;
  ts << l2 << "<tag>" << Q3StyleSheet::escape(tagName()) << "</tag>" << endl;
  ts << l2 << "<vectag>" << Q3StyleSheet::escape(_inputVectors[INVECTOR]->tag().tagString()) << "</vectag>" << endl;
  ts << l2 << "<sampRate>"  << _frequency << "</sampRate>" << endl;
  ts << l2 << "<average>" << _average << "</average>" << endl;
  ts << l2 << "<fftLen>" << int(ceil(log(double(_PSDLen*2)) / log(2.0))) << "</fftLen>" << endl;
  ts << l2 << "<removeMean>" << _removeMean << "</removeMean>" << endl;
  ts << l2 << "<apodize>" << _apodize << "</apodize>" << endl;
  ts << l2 << "<apodizefxn>" << _apodizeFxn << "</apodizefxn>" << endl;
  ts << l2 << "<windowsize>" << _windowSize << "</windowsize>" << endl;
  ts << l2 << "<vectorunits>" << _vectorUnits << "</vectorunits>" << endl;
  ts << l2 << "<rateunits>" << _rateUnits << "</rateunits>" << endl;
  ts << l2 << "<output>" << _outputType << "</output>" << endl;
  ts << indent << "</csdobject>" << endl;
}


QString KstCSD::vTag() const {
  return _inputVectors[INVECTOR]->tag().displayString();
}


void KstCSD::setVector(KstVectorPtr new_v) {
  KstVectorPtr v = _inputVectors[INVECTOR];
  if (v) {
    if (v == new_v) {
      return;
    }
    v->unlock();
  }

  _inputVectors.erase(INVECTOR);
  new_v->writeLock();
  _inputVectors[INVECTOR] = new_v;
  setDirty();
}


bool KstCSD::slaveVectorsUsed() const {
  return true;
}


QString KstCSD::propertyString() const {
  return i18n("CSD: %1").arg(_inputVectors[INVECTOR]->tag().displayString());
}


void KstCSD::showNewDialog() {
  KstDialogs::self()->showCSDDialog();
}


void KstCSD::showEditDialog() {
  KstDialogs::self()->showCSDDialog(tagName(), true);
}


bool KstCSD::apodize() const {
  return _apodize;
}


PSDType KstCSD::output() const {
  return _outputType;
}


void KstCSD::setOutput(PSDType in_outputType)  {
  setDirty();
  _outputType = in_outputType;

  updateMatrixLabels();
}


void KstCSD::setApodize(bool in_apodize)  {
  setDirty();
  _apodize = in_apodize;
}


bool KstCSD::removeMean() const {
  return _removeMean;
}


void KstCSD::setRemoveMean(bool in_removeMean) {
  setDirty();
  _removeMean = in_removeMean;
}


bool KstCSD::average() const {
  return _average;
}


void KstCSD::setAverage(bool in_average) {
  setDirty();
  _average = in_average;
}


double KstCSD::freq() const {
  return _frequency;
}


void KstCSD::setFreq(double in_freq) {
  setDirty();
  if (in_freq > 0.0) {
    _frequency = in_freq;
  } else {
    _frequency = KST::objectDefaults.psdFreq();
  }
}

ApodizeFunction KstCSD::apodizeFxn() const {
  return _apodizeFxn;
} 

void KstCSD::setApodizeFxn(ApodizeFunction in_fxn) {
  setDirty();
  _apodizeFxn = in_fxn;
}

int KstCSD::length() const { 
  return _averageLength;
}

void KstCSD::setLength(int in_length) {
  _averageLength = in_length;
}


int KstCSD::windowSize() const {
  return _windowSize;
}


void KstCSD::setWindowSize(int in_size) {
  setDirty();
  _windowSize = in_size;  
}

double KstCSD::gaussianSigma() const {
  return _gaussianSigma;
}

void KstCSD::setGaussianSigma(double in_sigma) {
  setDirty();
  _gaussianSigma = in_sigma;
}


KstMatrixPtr KstCSD::outputMatrix() const {
  return *_outMatrix;  
}


const QString& KstCSD::vectorUnits() const {
  return _vectorUnits;  
}


void KstCSD::setVectorUnits(const QString& units) {
  _vectorUnits = units;
}


const QString& KstCSD::rateUnits() const {
  return _rateUnits;
}


void KstCSD::setRateUnits(const QString& units) {
  _rateUnits = units;
}

 
KstDataObjectPtr KstCSD::makeDuplicate(KstDataObjectDataObjectMap& duplicatedMap) {
  QString name(tagName() + '\'');
  while (KstData::self()->dataTagNameNotUnique(name, false)) {
    name += '\'';
  }
  KstCSDPtr csd = new KstCSD(name, _inputVectors[INVECTOR], _frequency, _average, _removeMean,
                             _apodize, _apodizeFxn, _windowSize, _averageLength, _gaussianSigma, 
                             _outputType, _vectorUnits, _rateUnits);
  duplicatedMap.insert(this, KstDataObjectPtr(csd));
  return KstDataObjectPtr(csd);
}

void KstCSD::updateMatrixLabels(void) {
    switch (_outputType) {
    default:
    case 0: // amplitude spectral density (default) [V/Hz^1/2]
      (*_outMatrix)->setLabel(i18n("ASD [%1/%2^{1/2}]").arg(_vectorUnits).arg(_rateUnits));
      break;
    case 1: // power spectral density [V^2/Hz]
      (*_outMatrix)->setLabel(i18n("PSD [%1^2/%2]").arg(_vectorUnits).arg(_rateUnits));    
      break;
    case 2: // amplitude spectrum [V]
      (*_outMatrix)->setLabel(i18n("Amplitude Spectrum [%1]").arg(_vectorUnits));
      break;
    case 3: // power spectrum [V^2]
      (*_outMatrix)->setLabel(i18n("Power Spectrum [%1^2]").arg(_vectorUnits));    
      break;
  }
}

// vim: ts=2 sw=2 et
