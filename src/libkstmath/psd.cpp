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

#include <QXmlStreamWriter>

#include "kst_i18n.h"
#include <qdebug.h>

#include "dialoglauncher.h"
#include "datacollection.h"
#include "debug.h"
#include "psd.h"
#include "psdcalculator.h"
#include "objectstore.h"

extern "C" void rdft(int n, int isgn, double *a);

namespace Kst {

const QString PSD::staticTypeString = I18N_NOOP("Power Spectrum");
const QString PSD::staticTypeTag = I18N_NOOP("powerspectrum");

const QLatin1String& INVECTOR = QLatin1String("I");
const QLatin1String& SVECTOR = QLatin1String("S");
const QLatin1String& FVECTOR = QLatin1String("F");

#define KSTPSDMAXLEN 27

PSD::PSD(ObjectStore *store)
: DataObject(store) {
  commonConstructor(store, 0, 0, false, 0, false, false, QString::null, QString::null, WindowUndefined, 0, PSDUndefined, false);
}


PSD::PSD(ObjectStore *store, VectorPtr in_V,
                         double in_freq, bool in_average, int in_averageLen,
                         bool in_apodize, bool in_removeMean,
                         const QString &in_VUnits, const QString &in_RUnits, ApodizeFunction in_apodizeFxn,
                         double in_gaussianSigma, PSDType in_output)
: DataObject(store) {
  commonConstructor(store, in_V, in_freq, in_average, in_averageLen,
                    in_apodize, in_removeMean,
                    in_VUnits, in_RUnits, in_apodizeFxn, in_gaussianSigma,
                    in_output, false);
  setDirty();
}


// PSD::PSD(ObjectStore *store, const QDomElement &e)
// : DataObject(store, e) {
//   QString in_VUnits;
//   QString in_RUnits;
//   QString in_tag;
//   QString vecName;
//   VectorPtr in_V;
//   double in_freq = 60.0;
//   bool in_average = true;
//   bool in_removeMean = true;
//   bool in_apodize = true;
//   ApodizeFunction in_apodizeFxn = WindowOriginal;
//   double in_gaussianSigma = 3.0;
//   int in_averageLen = 12;
//   PSDType in_output = PSDAmplitudeSpectralDensity;
//   bool interpolateHoles = false;
// 
//   QDomNode n = e.firstChild();
//   while (!n.isNull()) {
//     QDomElement e = n.toElement(); // try to convert the node to an element.
//     if (!e.isNull()) { // the node was really an element.
//       if (e.tagName() == "tag") {
//         in_tag = e.text();
//       } else if (e.tagName() == "vector") {
//         vecName = e.text();
//       } else if (e.tagName() == "samplerate") {
//         in_freq = e.text().toDouble();
//       } else if (e.tagName() == "average") {
//         if (e.text() == "0") {
//           in_average = false;
//         } else {
//           in_average = true;
//         }
//       } else if (e.tagName() == "fftlength") {
//         in_averageLen = e.text().toInt();
//       } else if (e.tagName() == "apodize") {
//         if (e.text() == "0") {
//           in_apodize = false;
//         } else {
//           in_apodize = true;
//         }
//       } else if (e.tagName() == "apodizefunction") {
//         in_apodizeFxn = ApodizeFunction(e.text().toInt());
//       } else if (e.tagName() == "gaussiansigma") {
//         in_gaussianSigma = e.text().toDouble();
//       } else if (e.tagName() == "removeMean") {
//         if (e.text() == "0") {
//           in_removeMean = false;
//         } else {
//           in_removeMean = true;
//         }
//       } else if (e.tagName() == "vectorunits") {
//         in_VUnits = e.text();
//       } else if (e.tagName() == "rateunits") {
//         in_RUnits = e.text();
//       } else if (e.tagName() == "outputtype") {
//         in_output = (PSDType)e.text().toInt();
//       } else if (e.tagName() == "interpolateHoles") {
//         interpolateHoles = e.text().toInt() != 0;
//       }
//     }
//     n = n.nextSibling();
//   }
// 
//   _inputVectorLoadQueue.append(qMakePair(QString(INVECTOR), vecName));
// 
//   setTagName(ObjectTag::fromString(in_tag));
// 
//   commonConstructor(store, in_V, in_freq, in_average, in_averageLen,
//                     in_apodize, in_removeMean,
//                     in_VUnits, in_RUnits, in_apodizeFxn, in_gaussianSigma,
//                     in_output, interpolateHoles);
// }


void PSD::commonConstructor(ObjectStore *store, VectorPtr in_V,
                               double in_freq, bool in_average, int in_averageLen, bool in_apodize,
                               bool in_removeMean, const QString& in_VUnits, const QString& in_RUnits,
                               ApodizeFunction in_apodizeFxn, double in_gaussianSigma, PSDType in_output,
                               bool interpolateHoles) {

  _typeString = staticTypeString;
  _type = "PowerSpectrum";
  if (in_V) {
    _inputVectors[INVECTOR] = in_V;
  }
  _Frequency = in_freq;
  _Average = in_average;
  _Apodize = in_apodize;
  _apodizeFxn = in_apodizeFxn;
  _gaussianSigma = in_gaussianSigma;
  _prevOutput = PSDUndefined;
  _RemoveMean = in_removeMean;
  _vectorUnits = in_VUnits;
  _rateUnits = in_RUnits;
  _Output = in_output;
  _interpolateHoles = interpolateHoles;
  _averageLength = in_averageLen;

  _last_n_subsets = 0;
  _last_n_new = 0;

  _PSDLength = 1;

  Q_ASSERT(store);
  VectorPtr ov = store->createObject<Vector>();
  ov->setProvider(this);
  ov->setSlaveName("f");
  ov->resize(_PSDLength);
  _fVector = _outputVectors.insert(FVECTOR, ov).value();

  ov = store->createObject<Vector>();
  ov->setProvider(this);
  ov->setSlaveName("psd");
  ov->resize(_PSDLength);
  _sVector = _outputVectors.insert(SVECTOR, ov).value();

  updateVectorLabels();

  _shortName = "S"+QString::number(_psdnum);
  if (_psdnum>max_psdnum) 
    max_psdnum = _psdnum;
  _psdnum++;

}


PSD::~PSD() {
  _sVector = 0L;
  _fVector = 0L;
}


const CurveHintList *PSD::curveHints() const {
  _curveHints->clear();
  _curveHints->append(new CurveHint(i18n("PSD Curve"), _fVector->shortName(),
                      _sVector->shortName()));
  return _curveHints;
}


Object::UpdateType PSD::update() {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  writeLockInputsAndOutputs();

  VectorPtr iv = _inputVectors[INVECTOR];

  bool xUpdated = Object::UPDATE == iv->update();

  const int v_len = iv->length();

  // Don't touch _last_n_new if !xUpdated since it will certainly be wrong.
  if (!xUpdated && !force) {
    unlockInputsAndOutputs();
    return NO_CHANGE;
  }

  _last_n_new += iv->numNew();
  assert(_last_n_new >= 0);

  int n_subsets = v_len/_PSDLength;

  // determine if the PSD needs to be updated. if not using averaging, then we need at least _PSDLength/16 new data points. if averaging, then we want enough new data for a complete subset.
  if ( ((_last_n_new < _PSDLength/16) || (_Average && (n_subsets - _last_n_subsets < 1))) &&  iv->length() != iv->numNew() && !force) {
    unlockInputsAndOutputs();
    return NO_CHANGE;
  }

  _adjustLengths();

  double *psd = _sVector->value();
  double *f = _fVector->value();

  int i_samp;
  for (i_samp = 0; i_samp < _PSDLength; ++i_samp) {
    f[i_samp] = i_samp * 0.5 * _Frequency / (_PSDLength - 1);
  }

  _psdCalculator.calculatePowerSpectrum(iv->value(), v_len, psd, _PSDLength, _RemoveMean,  _interpolateHoles, _Average, _averageLength, _Apodize, _apodizeFxn, _gaussianSigma, _Output, _Frequency);

  _last_n_subsets = n_subsets;
  _last_n_new = 0;

  updateVectorLabels();
  _sVector->setDirty();
  _fVector->setDirty();

  unlockInputsAndOutputs();

  return UPDATE;
}


void PSD::_adjustLengths() {
  int nPSDLen = PSDCalculator::calculateOutputVectorLength(_inputVectors[INVECTOR]->length(), _Average, _averageLength);

  if (_PSDLength != nPSDLen) {
    _sVector->resize(nPSDLen);
    _fVector->resize(nPSDLen);

    if ( (_sVector->length() == nPSDLen) && (_fVector->length() == nPSDLen) ) {
      _PSDLength = nPSDLen;
    } else {
      Debug::self()->log(i18n("Attempted to create a PSD that used all memory."), Debug::Error);
    }

    _last_n_subsets = 0;
  }
}


void PSD::save(QXmlStreamWriter &s) {
  s.writeStartElement(staticTypeTag);
  s.writeAttribute("vector", _inputVectors[INVECTOR]->Name());
  s.writeAttribute("samplerate", QString::number(_Frequency));
  s.writeAttribute("gaussiansigma", QString::number(_gaussianSigma));
  s.writeAttribute("average", QVariant(_Average).toString());
  s.writeAttribute("fftlength", QString::number(int(ceil(log(double(_PSDLength*2)) / log(2.0)))));
  s.writeAttribute("removemean", QVariant(_RemoveMean).toString());
  s.writeAttribute("apodize", QVariant(_Apodize).toString());
  s.writeAttribute("apodizefunction", QString::number(_apodizeFxn));
  s.writeAttribute("interpolateholes", QVariant(_interpolateHoles).toString());
  s.writeAttribute("vectorunits", _vectorUnits);
  s.writeAttribute("rateunits", _rateUnits);
  s.writeAttribute("outputtype", QString::number(_Output));
  saveNameInfo(s, VNUM|PSDNUM|XNUM);

  s.writeEndElement();
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


double PSD::frequency() const {
  return _Frequency;
}


void PSD::setFrequency(double in_frequency) {
  setDirty();
  if (in_frequency > 0.0) {
    _Frequency = in_frequency;
  } else {
    _Frequency = 1.0;
  }
}


int PSD::length() const {
  return _averageLength;
}


void PSD::setLength(int in_length) {
  if (in_length != _averageLength) {
    _averageLength = in_length;
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


VectorPtr PSD::vector() const {
  return _inputVectors[INVECTOR];
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
  connect(new_v, SIGNAL(vectorUpdated(ObjectPtr)), this, SLOT(inputObjectUpdated(ObjectPtr)));
  setDirty();
}


bool PSD::slaveVectorsUsed() const {
  return true;
}


QString PSD::propertyString() const {
  return i18n("PSD: %1", _inputVectors[INVECTOR]->Name());
}


void PSD::showNewDialog() {
  DialogLauncher::self()->showPowerSpectrumDialog();
}


void PSD::showEditDialog() {
  DialogLauncher::self()->showPowerSpectrumDialog(this);
}


const QString& PSD::vectorUnits() const {
  return _vectorUnits;
}


void PSD::setVectorUnits(const QString& units) {
  _vectorUnits = units;
}


const QString& PSD::rateUnits() const {
  return _rateUnits;
}


void PSD::setRateUnits(const QString& units) {
  _rateUnits = units;
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


DataObjectPtr PSD::makeDuplicate() {

  PSDPtr powerspectrum = store()->createObject<PSD>();
  Q_ASSERT(powerspectrum);

  powerspectrum->writeLock();
  powerspectrum->setVector(_inputVectors[INVECTOR]);
  powerspectrum->setFrequency(_Frequency);
  powerspectrum->setAverage(_Average);
  powerspectrum->setLength(_averageLength);
  powerspectrum->setApodize(_Apodize);
  powerspectrum->setRemoveMean(_RemoveMean);
  powerspectrum->setVectorUnits(_vectorUnits);
  powerspectrum->setRateUnits(_rateUnits);
  powerspectrum->setApodizeFxn(_apodizeFxn);
  powerspectrum->setGaussianSigma(_gaussianSigma);
  powerspectrum->setOutput(_Output);
  powerspectrum->setInterpolateHoles(_interpolateHoles);
  if (descriptiveNameIsManual()) {
    powerspectrum->setDescriptiveName(descriptiveName());
  }
  powerspectrum->update();
  powerspectrum->unlock();

  return DataObjectPtr(powerspectrum);
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
      _sVector->setLabel(i18n("ASD \\[%1/%2^{1/2} \\]", _vectorUnits, _rateUnits));
      break;
    case 1: // power spectral density [V^2/Hz]
      _sVector->setLabel(i18n("PSD \\[%1^2/%2\\]", _vectorUnits, _rateUnits));
      break;
    case 2: // amplitude spectrum [V]
      _sVector->setLabel(i18n("Amplitude Spectrum\\[%1\\]", _vectorUnits));
      break;
    case 3: // power spectrum [V^2]
      _sVector->setLabel(i18n("Power Spectrum \\[%1^2\\]", _vectorUnits));
      break;
  }
  _fVector->setLabel(i18n("Frequency \\[%1\\]", _rateUnits));
}

QString PSD::_automaticDescriptiveName() const {
  return vector()->descriptiveName();
}

}
// vim: ts=2 sw=2 et
