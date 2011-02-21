/***************************************************************************
                          psd.cpp: Power Spectra for KST
                             -------------------
    begin                : Fri Feb 10 2002
    copyright            : (C) 2002 by C. Barth Netterfield
    email                : netterfield@astro.utoronto.ca
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
  _typeString = staticTypeString;
  _type = "PowerSpectrum";
  _initializeShortName();

  Q_ASSERT(store);
  VectorPtr ov = store->createObject<Vector>();
  ov->setProvider(this);
  ov->setSlaveName("f");
  ov->resize(2);
  _fVector = _outputVectors.insert(FVECTOR, ov).value();

  ov = store->createObject<Vector>();
  ov->setProvider(this);
  ov->setSlaveName("psd");
  ov->resize(2);
  _sVector = _outputVectors.insert(SVECTOR, ov).value();
}

void PSD::_initializeShortName() {
  _shortName = 'S'+QString::number(_psdnum);
  if (_psdnum>max_psdnum)
    max_psdnum = _psdnum;
  _psdnum++;
}

void PSD::change(VectorPtr in_V,
                               double in_freq, bool in_average, int in_averageLen, bool in_apodize,
                               bool in_removeMean, const QString& in_VUnits, const QString& in_RUnits,
                               ApodizeFunction in_apodizeFxn, double in_gaussianSigma, PSDType in_output,
                               bool interpolateHoles) {

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

  _fVector->resize(_PSDLength);
  _sVector->resize(_PSDLength);

  updateVectorLabels();
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


void PSD::internalUpdate() {

  writeLockInputsAndOutputs();

  VectorPtr iv = _inputVectors[INVECTOR];

  const int v_len = iv->length();

  _last_n_new += iv->numNew();
  assert(_last_n_new >= 0);

  int n_subsets = v_len/_PSDLength;

  // determine if the PSD needs to be updated.
  // if not using averaging, then we need at least _PSDLength/16 new data points.
  // if averaging, then we want enough new data for a complete subset.
  if ( ((_last_n_new < _PSDLength/16) || (_Average && (n_subsets - _last_n_subsets < 1))) &&
       iv->length() != iv->numNew()) {
    unlockInputsAndOutputs();

    return;
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

  // should be updated by the update manager
  //_sVector->update();
  //_fVector->update();

  unlockInputsAndOutputs();

  return;
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
  _Apodize = in_apodize;
}


bool PSD::removeMean() const {
  return _RemoveMean;
}


void PSD::setRemoveMean(bool in_removeMean) {
  _RemoveMean = in_removeMean;
}


bool PSD::average() const {
  return _Average;
}


void PSD::setAverage(bool in_average) {
  _Average = in_average;
}


double PSD::frequency() const {
  return _Frequency;
}


void PSD::setFrequency(double in_frequency) {
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
  }
}


PSDType PSD::output() const {
  return _Output;
}


void PSD::setOutput(PSDType in_output)  {
  if (in_output != _Output) {
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
    _apodizeFxn = in_apodizeFxn;
  }
}


double PSD::gaussianSigma() const {
  return _gaussianSigma;
}


void PSD::setGaussianSigma(double in_gaussianSigma) {
  if (_gaussianSigma != in_gaussianSigma) {
    _gaussianSigma = in_gaussianSigma;
  }
}


DataObjectPtr PSD::makeDuplicate() const {

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
  powerspectrum->registerChange();
  powerspectrum->unlock();

  return DataObjectPtr(powerspectrum);
}


bool PSD::interpolateHoles() const {
  return _interpolateHoles;
}


void PSD::setInterpolateHoles(bool interpolate) {
  if (interpolate != _interpolateHoles) {
    _interpolateHoles = interpolate;
  }
}

void PSD::updateVectorLabels() {
  LabelInfo label_info;

  switch (_Output) {
    default:
    case 0: // amplitude spectral density (default) [V/Hz^1/2]
      label_info.quantity = i18n("Spectral Density");
      if (_vectorUnits.isEmpty() || _rateUnits.isEmpty()) {
        label_info.units = QString();
      } else {
        label_info.units = QString("%1/%2^{1/2} ").arg(_vectorUnits).arg(_rateUnits);
      }
      break;
    case 1: // power spectral density [V^2/Hz]
      label_info.quantity = i18n("PSD");
      if (_vectorUnits.isEmpty() || _rateUnits.isEmpty()) {
        label_info.units = QString();
      } else {
        label_info.units = QString("%1^2/%2").arg(_vectorUnits).arg(_rateUnits);
      }
      break;
    case 2: // amplitude spectrum [V]
      label_info.quantity = i18n("Amplitude Spectrum");
      if (_vectorUnits.isEmpty() || _rateUnits.isEmpty()) {
        label_info.units = QString();
      } else {
        label_info.units = QString("%1").arg(_vectorUnits);
      }
      break;
    case 3: // power spectrum [V^2]
      label_info.quantity = i18n("Power Spectrum");
      if (_vectorUnits.isEmpty() || _rateUnits.isEmpty()) {
        label_info.units = QString();
      } else {
        label_info.units = QString("%1^2").arg(_vectorUnits);
      }
      break;
  }
  label_info.name = QString();
  _sVector->setLabelInfo(label_info);

  label_info.quantity = i18n("Frequency");
  label_info.units = _rateUnits;
  _fVector->setLabelInfo(label_info);

  label_info.quantity.clear();
  label_info.units.clear();
  label_info.name = _inputVectors[INVECTOR]->labelInfo().name;
  _sVector->setTitleInfo(label_info);

}

QString PSD::_automaticDescriptiveName() const {
  return vector()->descriptiveName();
}

QString PSD::descriptionTip() const {
  QString tip;

  tip = i18n("Spectrum: %1\n  FFT Length: 2^%2").arg(Name()).arg(length());

  if (average() || apodize() || removeMean()) {
    tip += "\n  ";
    if (average()) tip += i18n("Average; ");
    if (apodize()) tip += i18n("Apodize; ");
    if (removeMean()) tip += i18n("Remove Mean;");
  }
  tip += i18n("\nInput: %1").arg(_inputVectors[INVECTOR]->descriptionTip());
  return tip;
}
}
// vim: ts=2 sw=2 et
