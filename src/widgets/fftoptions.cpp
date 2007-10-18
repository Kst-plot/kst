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

  connect(Interleaved, SIGNAL(clicked()), this, SLOT(clickedInterleaved()));

  connect(Apodize, SIGNAL(clicked()), this, SLOT(clickedApodize()));

  connect(ApodizeFxn, SIGNAL(activated(int)), this, SLOT(changedApodizeFxn()));

  connect(Apodize, SIGNAL(clicked()), this, SLOT(changedApodizeFxn()));
}


FFTOptions::~FFTOptions() {}


void FFTOptions::init() {
  update();
}


void FFTOptions::update() {
  objectDefaults.sync();

  FFTLen->setValue(objectDefaults.fftLen());
  SampRate->setText(QString::number(objectDefaults.psdFreq()));
  VectorUnits->setText(objectDefaults.vUnits());
  RateUnits->setText(objectDefaults.rUnits());
  Apodize->setChecked(objectDefaults.apodize());
  RemoveMean->setChecked(objectDefaults.removeMean());
  Interleaved->setChecked(objectDefaults.psdAverage());
  ApodizeFxn->setCurrentIndex(objectDefaults.apodizeFxn());
  Output->setCurrentIndex(objectDefaults.output());
  InterpolateHoles->setChecked(objectDefaults.interpolateHoles());

  clickedInterleaved();
  clickedApodize();
  changedApodizeFxn();
}


void FFTOptions::changedApodizeFxn() {
  int gaussianIndex = 5;
  if (ApodizeFxn->itemText(0).isEmpty()) {
    ++gaussianIndex;
  }
  Sigma->setEnabled(ApodizeFxn->currentIndex() == gaussianIndex && Apodize->isChecked());
}


void FFTOptions::clickedInterleaved() {
  FFTLen->setEnabled(Interleaved->isChecked());
}


void FFTOptions::clickedApodize() {
  ApodizeFxn->setEnabled(Apodize->isChecked());
}


void FFTOptions::synch() {
  clickedInterleaved();
  clickedApodize();
}


bool FFTOptions::checkValues() {
  double new_freq = SampRate->text().toDouble();
  int new_len = FFTLen->text().toInt();
  return checkGivenValues(new_freq, new_len);
}


bool FFTOptions::checkGivenValues(double sampRate, int FFTLen) {
  if (sampRate <= 0) {
    return false;
  }
  if (FFTLen < 2) {
    return false;
  }
  return true;
}

}
// vim: ts=2 sw=2 et
