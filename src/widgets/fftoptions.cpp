/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "fftoptions.h"

#include <objectdefaults.h>

namespace Kst {

FFTOptions::FFTOptions(QWidget *parent)
    : QWidget(parent) {
  setupUi(this);

  connect(_interleavedAverage, SIGNAL(clicked()), this, SLOT(clickedInterleaved()));

  connect(_apodize, SIGNAL(clicked()), this, SLOT(clickedApodize()));

  connect(_apodizeFunction, SIGNAL(activated(int)), this, SLOT(changedApodizeFxn()));

  connect(_apodize, SIGNAL(clicked()), this, SLOT(changedApodizeFxn()));
}


FFTOptions::~FFTOptions() {}


void FFTOptions::init() {
  update();
}


void FFTOptions::update() {
  objectDefaults.sync();

  _FFTLength->setValue(objectDefaults.fftLen());
  _sampleRate->setText(QString::number(objectDefaults.psdFreq()));
  _vectorUnits->setText(objectDefaults.vUnits());
  _rateUnits->setText(objectDefaults.rUnits());
  _apodize->setChecked(objectDefaults.apodize());
  _removeMean->setChecked(objectDefaults.removeMean());
  _interleavedAverage->setChecked(objectDefaults.psdAverage());
  _apodizeFunction->setCurrentIndex(objectDefaults.apodizeFxn());
  _output->setCurrentIndex(objectDefaults.output());
  _interpolateOverHoles->setChecked(objectDefaults.interpolateHoles());

  clickedInterleaved();
  clickedApodize();
  changedApodizeFxn();
}


double FFTOptions::sampleRate() const {
  return _sampleRate->text().toDouble();
}


void FFTOptions::setSampleRate(const double sampleRate) {
  _sampleRate->setText(QString::number(sampleRate));
}


double FFTOptions::sigma() const {
  return _sigma->value();
}


void FFTOptions::setSigma(const double sigma) {
  _sigma->setValue(sigma);
}


bool FFTOptions::interleavedAverage() const {
  return _interleavedAverage->isChecked();
}


void FFTOptions::setInterleavedAverage(const bool interleavedAverage) {
  _interleavedAverage->setChecked(interleavedAverage);
}


bool FFTOptions::apodize() const {
  return _apodize->isChecked();
}


void FFTOptions::setApodize(const bool apodize) {
  _apodize->setChecked(apodize);
}


bool FFTOptions::removeMean() const {
  return _removeMean->isChecked();
}


void FFTOptions::setRemoveMean(const bool removeMean) {
  _removeMean->setChecked(removeMean);
}


bool FFTOptions::interpolateOverHoles() const {
  return _interpolateOverHoles->isChecked();
}


void FFTOptions::setInterpolateOverHoles(const bool interpolateOverHoles) {
  _interpolateOverHoles->setChecked(interpolateOverHoles);
}


int FFTOptions::FFTLength() const {
  return _FFTLength->value();
}


void FFTOptions::setFFTLength(const int FFTLength) {
  _FFTLength->setValue(FFTLength);
}


QString FFTOptions::vectorUnits() const {
  return _vectorUnits->text();
}


void FFTOptions::setVectorUnits(const QString vectorUnits) {
  _vectorUnits->setText(vectorUnits);
}


QString FFTOptions::rateUnits() const {
  return _rateUnits->text();
}


void FFTOptions::setRateUnits(const QString rateUnits) {
  _rateUnits->setText(rateUnits);
}


ApodizeFunction FFTOptions::apodizeFunction() const {
  return (ApodizeFunction)_apodizeFunction->currentIndex();
}


void FFTOptions::setApodizeFunction(const ApodizeFunction apodizeFunction) {
  _apodizeFunction->setCurrentItem((ApodizeFunction)apodizeFunction);
}


PSDType FFTOptions::output() const {
  return (PSDType)_output->currentIndex();
}


void FFTOptions::setOutput(const PSDType output) {
  _output->setCurrentItem((PSDType)output);
}


void FFTOptions::changedApodizeFxn() {
  int gaussianIndex = 5;
  if (_apodizeFunction->itemText(0).isEmpty()) {
    ++gaussianIndex;
  }
  _sigma->setEnabled(_apodizeFunction->currentIndex() == gaussianIndex && _apodize->isChecked());
}


void FFTOptions::clickedInterleaved() {
  _FFTLength->setEnabled(_interleavedAverage->isChecked());
}


void FFTOptions::clickedApodize() {
  _apodizeFunction->setEnabled(_apodize->isChecked());
}


void FFTOptions::synch() {
  clickedInterleaved();
  clickedApodize();
}


bool FFTOptions::checkValues() {
  double new_freq = _sampleRate->text().toDouble();
  int new_len = _FFTLength->text().toInt();
  return checkGivenValues(new_freq, new_len);
}


bool FFTOptions::checkGivenValues(double sampleRate, int FFTLength) {
  if (sampleRate <= 0) {
    return false;
  }
  if (FFTLength < 2) {
    return false;
  }
  return true;
}

}
// vim: ts=2 sw=2 et
