/***************************************************************************
                          psd.cpp: Power Spectra for KST
                             -------------------
    begin                : Fri Feb 10 2002
    copyright            : (C) 2002 by C. Barth Netterfield
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

/** A class for handling power spectra for kst
 *@author C. Barth Netterfield
 */

#include <assert.h>
#include <math.h>

#include <QTextDocument>

#include "kst_i18n.h"
#include <qdebug.h>

#include "dialoglauncher.h"
#include "datacollection.h"
#include "debug.h"
#include "psd.h"
#include "psdcalculator.h"
#include "objectdefaults.h"

extern "C" void rdft(int n, int isgn, double *a);

namespace Kst {

const QLatin1String& INVECTOR = QLatin1String("I");
const QLatin1String& SVECTOR = QLatin1String("S");
const QLatin1String& FVECTOR = QLatin1String("F");

#define KSTPSDMAXLEN 27
PSD::PSD(const QString &in_tag, VectorPtr in_V,
                         double in_freq, bool in_average, int in_averageLen,
                         bool in_apodize, bool in_removeMean,
                         const QString &in_VUnits, const QString &in_RUnits, ApodizeFunction in_apodizeFxn, 
                         double in_gaussianSigma, PSDType in_output)
: DataObject() {
  commonConstructor(in_tag, in_V, in_freq, in_average, in_averageLen,
                    in_apodize, in_removeMean,
                    in_VUnits, in_RUnits, in_apodizeFxn, in_gaussianSigma,
                    in_output, false);
  setDirty();
}


PSD::PSD(const QDomElement &e)
: DataObject(e) {
  QString in_VUnits;
  QString in_RUnits;
  QString in_tag;
  QString vecName;
  VectorPtr in_V;
  double in_freq = 60.0;
  bool in_average = true;
  bool in_removeMean = true;
  bool in_apodize = true;
  ApodizeFunction in_apodizeFxn = WindowOriginal;
  double in_gaussianSigma = 3.0;
  int in_averageLen = 12;
  PSDType in_output = PSDAmplitudeSpectralDensity;
  bool interpolateHoles = false;
  
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
        if (e.text() == "0") {
          in_average = false;
        } else {
          in_average = true;
        }
      } else if (e.tagName() == "fftLen") {
        in_averageLen = e.text().toInt();
      } else if (e.tagName() == "apodize") {
        if (e.text() == "0") {
          in_apodize = false;
        } else {
          in_apodize = true;
        }
      } else if (e.tagName() == "apodizefxn") {
        in_apodizeFxn = ApodizeFunction(e.text().toInt());
      } else if (e.tagName() == "gaussiansigma") {
        in_gaussianSigma = e.text().toDouble();
      } else if (e.tagName() == "removeMean") {
        if (e.text() == "0") {
          in_removeMean = false;
        } else {
          in_removeMean = true;
        }
      } else if (e.tagName() == "VUnits") {
        in_VUnits = e.text();
      } else if (e.tagName() == "RUnits") {
        in_RUnits = e.text();
      } else if (e.tagName() == "output") {
        in_output = (PSDType)e.text().toInt();
      } else if (e.tagName() == "interpolateHoles") {
        interpolateHoles = e.text().toInt() != 0;
      }
    }
    n = n.nextSibling();
  }

  _inputVectorLoadQueue.append(qMakePair(QString(INVECTOR), vecName));
  commonConstructor(in_tag, in_V, in_freq, in_average, in_averageLen,
                    in_apodize, in_removeMean,
                    in_VUnits, in_RUnits, in_apodizeFxn, in_gaussianSigma,
                    in_output, interpolateHoles);
}


void PSD::commonConstructor(const QString& in_tag, VectorPtr in_V,
                               double in_freq, bool in_average, int in_averageLen, bool in_apodize, 
                               bool in_removeMean, const QString& in_VUnits, const QString& in_RUnits, 
                               ApodizeFunction in_apodizeFxn, double in_gaussianSigma, PSDType in_output,
                               bool interpolateHoles) {

  _typeString = i18n("Power Spectrum");
  _type = "PowerSpectrum";
  if (in_V) {
    _inputVectors[INVECTOR] = in_V;
  }
  setTagName(ObjectTag::fromString(in_tag));
  _Freq = in_freq;
  _Average = in_average;
  _Apodize = in_apodize;
  _apodizeFxn = in_apodizeFxn;
  _gaussianSigma = in_gaussianSigma;
  _prevOutput = PSDUndefined;
  _RemoveMean = in_removeMean;
  _vUnits = in_VUnits;
  _rUnits = in_RUnits;
  _Output = in_output;
  _interpolateHoles = interpolateHoles;
  _averageLen = in_averageLen;

  _last_n_subsets = 0;
  _last_n_new = 0;

  _PSDLen = 1;
  VectorPtr ov = new Vector(ObjectTag("freq", tag()), _PSDLen, this);
  _fVector = _outputVectors.insert(FVECTOR, ov);

  ov = new Vector(ObjectTag("sv", tag()), _PSDLen, this);
  _sVector = _outputVectors.insert(SVECTOR, ov);

  updateVectorLabels();
}


PSD::~PSD() {
  _sVector = _outputVectors.end();
  _fVector = _outputVectors.end();
  vectorList.lock().writeLock();
  vectorList.remove(_outputVectors[SVECTOR]);
  vectorList.remove(_outputVectors[FVECTOR]);
  vectorList.lock().unlock();
}


const CurveHintList *PSD::curveHints() const {
  _curveHints->clear();
  _curveHints->append(new CurveHint(i18n("PSD Curve"), (*_fVector)->tagName(), (*_sVector)->tagName()));
  return _curveHints;
}


Object::UpdateType PSD::update(int update_counter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  if (Object::checkUpdateCounter(update_counter) && !force) {
    return lastUpdateResult();
  }

  writeLockInputsAndOutputs();

  VectorPtr iv = _inputVectors[INVECTOR];

  if (update_counter <= 0) {
    assert(update_counter == 0);
    force = true;
  }

  bool xUpdated = Object::UPDATE == iv->update(update_counter);

  const int v_len = iv->length();

  // Don't touch _last_n_new if !xUpdated since it will certainly be wrong.
  if (!xUpdated && !force) {
    unlockInputsAndOutputs();
    return setLastUpdateResult(NO_CHANGE);
  }

  _last_n_new += iv->numNew();
  assert(_last_n_new >= 0);

  int n_subsets = v_len/_PSDLen;

  // determine if the PSD needs to be updated. if not using averaging, then we need at least _PSDLen/16 new data points. if averaging, then we want enough new data for a complete subset.
  if ( ((_last_n_new < _PSDLen/16) || (_Average && (n_subsets - _last_n_subsets < 1))) &&  iv->length() != iv->numNew() && !force) {
    unlockInputsAndOutputs();
    return setLastUpdateResult(NO_CHANGE);
  }

  _adjustLengths();

  double *psd = (*_sVector)->value();
  double *f = (*_fVector)->value();

  int i_samp;
  for (i_samp = 0; i_samp < _PSDLen; ++i_samp) {
    f[i_samp] = i_samp * 0.5 * _Freq / (_PSDLen - 1);
  }

  _psdCalculator.calculatePowerSpectrum(iv->value(), v_len, psd, _PSDLen, _RemoveMean,  _interpolateHoles, _Average, _averageLen, _Apodize, _apodizeFxn, _gaussianSigma, _Output, _Freq);

  _last_n_subsets = n_subsets;
  _last_n_new = 0;

  updateVectorLabels();
  (*_sVector)->setDirty();
  (*_sVector)->update(update_counter);
  (*_fVector)->setDirty();
  (*_fVector)->update(update_counter);

  unlockInputsAndOutputs();

  return setLastUpdateResult(UPDATE);
}


void PSD::_adjustLengths() {
  int nPSDLen = PSDCalculator::calculateOutputVectorLength(_inputVectors[INVECTOR]->length(), _Average, _averageLen);

  if (_PSDLen != nPSDLen) {
    (*_sVector)->resize(nPSDLen);
    (*_fVector)->resize(nPSDLen);

    if ( ((*_sVector)->length() == nPSDLen) && ((*_fVector)->length() == nPSDLen) ) {
      _PSDLen = nPSDLen;
    } else {
      Debug::self()->log(i18n("Attempted to create a PSD that used all memory."), Debug::Error);
    }

    _last_n_subsets = 0;
  }
}

void PSD::save(QTextStream &ts, const QString& indent) {
  QString l2 = indent + "  ";
  ts << indent << "<psdobject>" << endl;
  ts << l2 << "<tag>" << Qt::escape(tagName()) << "</tag>" << endl;
  ts << l2 << "<vectag>" << Qt::escape(_inputVectors[INVECTOR]->tag().tagString()) << "</vectag>" << endl;
  ts << l2 << "<sampRate>"  << _Freq << "</sampRate>" << endl;
  ts << l2 << "<average>" << _Average << "</average>" << endl;
  ts << l2 << "<fftLen>" << int(ceil(log(double(_PSDLen*2)) / log(2.0))) << "</fftLen>" << endl;
  ts << l2 << "<removeMean>" << _RemoveMean << "</removeMean>" << endl;
  ts << l2 << "<interpolateHoles>" << _interpolateHoles << "</interpolateHoles>" << endl;
  ts << l2 << "<apodize>" << _Apodize << "</apodize>" << endl;
  ts << l2 << "<apodizefxn>" << _apodizeFxn << "</apodizefxn>" << endl;
  ts << l2 << "<gaussiansigma>" << _gaussianSigma << "</gaussiansigma>" << endl;
  ts << l2 << "<VUnits>" << _vUnits << "</VUnits>" << endl;
  ts << l2 << "<RUnits>" << _rUnits << "</RUnits>" << endl;
  ts << l2 << "<output>" << _Output << "</output>" << endl;
  ts << indent << "</psdobject>" << endl;
}


bool PSD::apodize() const {
  return _Apodize;
}


void PSD::setApodize(bool in_apodize)  {
  setDirty();
  _Apodize = in_apodize;
}


bool PSD::removeMean() const {
  return _RemoveMean;
}


void PSD::setRemoveMean(bool in_removeMean) {
  setDirty();
  _RemoveMean = in_removeMean;
}


bool PSD::average() const {
  return _Average;
}


void PSD::setAverage(bool in_average) {
  setDirty();
  _Average = in_average;
}


double PSD::freq() const {
  return _Freq;
}


void PSD::setFreq(double in_freq) {
  setDirty();
  if (in_freq > 0.0) {
    _Freq = in_freq;
  } else {
    _Freq = objectDefaults.psdFreq();
  }
}


int PSD::len() const {
  return _averageLen;
}


void PSD::setLen(int in_len) {
  if (in_len != _averageLen) {
    _averageLen = in_len;
    setDirty();
  }
}


PSDType PSD::output() const {
  return _Output;
}


void PSD::setOutput(PSDType in_output)  {
  if (in_output != _Output) {
    setDirty();
    _Output = in_output;
  }
}


QString PSD::vTag() const {
  return _inputVectors[INVECTOR]->tag().displayString();
}


void PSD::setVector(VectorPtr new_v) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

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


bool PSD::slaveVectorsUsed() const {
  return true;
}


QString PSD::propertyString() const {
  return i18n("PSD: %1", vTag());
}


void PSD::showNewDialog() {
  DialogLauncher::self()->showPSDDialog();
}


void PSD::showEditDialog() {
  DialogLauncher::self()->showPSDDialog(this);
}


const QString& PSD::vUnits() const {
  return _vUnits;
}


void PSD::setVUnits(const QString& units) {
  _vUnits = units;
}


const QString& PSD::rUnits() const {
  return _rUnits;
}


void PSD::setRUnits(const QString& units) {
  _rUnits = units;
}


ApodizeFunction PSD::apodizeFxn() const {
  return _apodizeFxn;
}


void PSD::setApodizeFxn(ApodizeFunction in_apodizeFxn) {
  if (_apodizeFxn != in_apodizeFxn) {
    setDirty();
    _apodizeFxn = in_apodizeFxn;
  }
}


double PSD::gaussianSigma() const {
  return _gaussianSigma;
}


void PSD::setGaussianSigma(double in_gaussianSigma) {
  if (_gaussianSigma != in_gaussianSigma) {
    setDirty();
    _gaussianSigma = in_gaussianSigma;
  }
}

 
DataObjectPtr PSD::makeDuplicate(DataObjectDataObjectMap& duplicatedMap) {
  QString name(tagName() + '\'');
  while (Data::self()->dataTagNameNotUnique(name, false)) {
    name += '\'';
  }
  PSDPtr psd = new PSD(name, _inputVectors[INVECTOR], _Freq,
                             _Average, _averageLen, _Apodize, _RemoveMean, _vUnits, _rUnits, 
                             _apodizeFxn, _gaussianSigma, _Output);
  duplicatedMap.insert(this, DataObjectPtr(psd));
  return DataObjectPtr(psd);
}


bool PSD::interpolateHoles() const {
  return _interpolateHoles;
}


void PSD::setInterpolateHoles(bool interpolate) {
  if (interpolate != _interpolateHoles) {
    _interpolateHoles = interpolate;
    setDirty();
  }
}

void PSD::updateVectorLabels() {
  switch (_Output) {
    default:
    case 0: // amplitude spectral density (default) [V/Hz^1/2]
      (*_sVector)->setLabel(i18n("ASD \\[%1/%2^{1/2} \\]", _vUnits, _rUnits));
      break;
    case 1: // power spectral density [V^2/Hz]
      (*_sVector)->setLabel(i18n("PSD \\[%1^2/%2\\]", _vUnits, _rUnits));
      break;
    case 2: // amplitude spectrum [V]
      (*_sVector)->setLabel(i18n("Amplitude Spectrum\\[%1\\]", _vUnits));
      break;
    case 3: // power spectrum [V^2]
      (*_sVector)->setLabel(i18n("Power Spectrum \\[%1^2\\]", _vUnits));
      break;
  }
  (*_fVector)->setLabel(i18n("Frequency \\[%1\\]", _rUnits));
}

}
// vim: ts=2 sw=2 et
