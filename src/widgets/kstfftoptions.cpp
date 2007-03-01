/***************************************************************************
                   kstfftoptions.cpp
                             -------------------
    begin                : 02/27/07
    copyright            : (C) 2007 The University of Toronto
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

#include "kstfftoptions.h"

#include <kmessagebox.h>

#include <kstobjectdefaults.h>

#include <kst_export.h>

KstFFTOptions::KstFFTOptions(QWidget *parent)
    : QWidget(parent) {
  setupUi(this);

  connect(Interleaved, SIGNAL(clicked()), this, SLOT(clickedInterleaved()));

  connect(Apodize, SIGNAL(clicked()), this, SLOT(clickedApodize()));

  connect(ApodizeFxn, SIGNAL(activated(int)), this, SLOT(changedApodizeFxn()));

  connect(Apodize, SIGNAL(clicked()), this, SLOT(changedApodizeFxn()));
}


KstFFTOptions::~KstFFTOptions() {}


void KstFFTOptions::init() {
  update();
}


void KstFFTOptions::update() {
  KST::objectDefaults.sync();

  FFTLen->setValue(KST::objectDefaults.fftLen());
  SampRate->setText(QString::number(KST::objectDefaults.psdFreq()));
  VectorUnits->setText(KST::objectDefaults.vUnits());
  RateUnits->setText(KST::objectDefaults.rUnits());
  Apodize->setChecked(KST::objectDefaults.apodize());
  RemoveMean->setChecked(KST::objectDefaults.removeMean());
  Interleaved->setChecked(KST::objectDefaults.psdAverage());
  ApodizeFxn->setCurrentIndex(KST::objectDefaults.apodizeFxn());
  Output->setCurrentIndex(KST::objectDefaults.output());
  InterpolateHoles->setChecked(KST::objectDefaults.interpolateHoles());

  clickedInterleaved();
  clickedApodize();
  changedApodizeFxn();
}


void KstFFTOptions::changedApodizeFxn() {
  int gaussianIndex = 5;
  if (ApodizeFxn->itemText(0).isEmpty()) {
    ++gaussianIndex;
  }
  Sigma->setEnabled(ApodizeFxn->currentIndex() == gaussianIndex && Apodize->isChecked());
}


void KstFFTOptions::clickedInterleaved() {
  FFTLen->setEnabled(Interleaved->isChecked());
}


void KstFFTOptions::clickedApodize() {
  ApodizeFxn->setEnabled(Apodize->isChecked());
}


void KstFFTOptions::synch() {
  clickedInterleaved();
  clickedApodize();
}


bool KstFFTOptions::checkValues() {
  double new_freq = SampRate->text().toDouble();
  int new_len = FFTLen->text().toInt();
  return checkGivenValues(new_freq, new_len);
}


bool KstFFTOptions::checkGivenValues(double sampRate, int FFTLen) {
  if (sampRate <= 0) {
    KMessageBox::sorry(this, i18n("The sample rate must be greater than 0"));
    return false;
  }
  if (FFTLen < 2) {
    KMessageBox::sorry(this, i18n("The FFT length must be greater than 2^2"));
    return false;
  }
  return true;
}

#include "kstfftoptions.moc"

// vim: ts=2 sw=2 et
