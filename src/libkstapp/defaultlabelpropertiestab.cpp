/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "kst_i18n.h"
#include "defaultlabelpropertiestab.h"

namespace Kst {

const double USLetterWidth = (11.0 - 1.0)*2.54; // 11 inches with a 1/2" margin in cm
const double USLetterHeight = ((8.5 - 1)*2.54); // 8.5 inches, with a 1/2" margin, in cm.
const double A4Width = (29.7 - 3.0); //  A4 with a 1.5 cm margin;
const double A4Height = (21.0 - 3.0); // A4 with a 1.5 cm margin;
const double J2cWidth = 9.0;
const double J2cHeight = 9.0;

DefaultLabelPropertiesTab::DefaultLabelPropertiesTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Fonts"));

  _referenceViewSizeCombo->addItem(i18n("Letter"/*, "US Letter sized paper"*/));
  _referenceViewSizeCombo->addItem(i18n("A4"/*, "A4 sized paper"*/));
  _referenceViewSizeCombo->addItem(i18n("Journal Plot"));
  _referenceViewSizeCombo->addItem(i18n("Custom"/*, "Custom page size"*/));

  _referenceViewSizeCombo->setCurrentIndex(0);
  referenceViewSizeComboChanged(0);

  connect(_referenceViewSizeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(referenceViewSizeComboChanged(int)));

  connect(_labelFontScale, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_refViewWidth, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_refViewHeight, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_minFontSize, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_labelColor, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));
  connect(_family, SIGNAL(currentFontChanged(const QFont &)), this, SIGNAL(modified()));
}


DefaultLabelPropertiesTab::~DefaultLabelPropertiesTab() {
}


void DefaultLabelPropertiesTab::referenceViewSizeComboChanged(int i) {
  switch (i) {
  case 0: // US Letter
    _refViewWidth->setValue(USLetterWidth);
    _refViewWidth->setEnabled(false);
    _refViewHeight->setValue(USLetterHeight); // 8.5 inches, in cm, with a 1/2" margin.
    _refViewHeight->setEnabled(false);
    break;
  case 1: // A4 210 x 297 mm,
    _refViewWidth->setValue(A4Width); //  A4 with a 1.5 cm margin
    _refViewWidth->setEnabled(false);
    _refViewHeight->setValue(A4Height); // A4 with a 1.5 cm margin
    _refViewHeight->setEnabled(false);
    break;
  case 2: // half a page..
    _refViewWidth->setValue(J2cWidth);
    _refViewWidth->setEnabled(false);
    _refViewHeight->setValue(J2cHeight);
    _refViewHeight->setEnabled(false);
    break;
  case 3:
    _refViewWidth->setEnabled(true);
    _refViewHeight->setEnabled(true);
    break;
  default:
    _refViewWidth->setEnabled(true);
    _refViewHeight->setEnabled(true);

  }
}

double DefaultLabelPropertiesTab::referenceViewWidth() const {
  return (_refViewWidth->value());
}


void DefaultLabelPropertiesTab::setReferenceViewWidth(const double width) {
  _refViewWidth->setValue(width);
  checkSizeDefaults();
}


double DefaultLabelPropertiesTab::referenceViewHeight() const {
  return (_refViewHeight->value());
}

#define isClose(x,y) (fabs(x-y)<0.0001)
void DefaultLabelPropertiesTab::checkSizeDefaults() {
  if (isClose(referenceViewHeight(),USLetterHeight) && isClose(referenceViewWidth(),USLetterWidth) &&
      (_referenceViewSizeCombo->currentIndex() !=0)) {
    _referenceViewSizeCombo->setCurrentIndex(0);
  } else if (isClose(referenceViewHeight(),A4Height) && isClose(referenceViewWidth(),A4Width) &&
      (_referenceViewSizeCombo->currentIndex() !=1)) {
    _referenceViewSizeCombo->setCurrentIndex(1);
  } else if (isClose(referenceViewHeight(),J2cHeight) && isClose(referenceViewWidth(),J2cWidth) &&
      (_referenceViewSizeCombo->currentIndex() !=2)) {
    _referenceViewSizeCombo->setCurrentIndex(2);
  } else {
    _referenceViewSizeCombo->setCurrentIndex(3); // custom
  }
}

void DefaultLabelPropertiesTab::setReferenceViewHeight(const double height) {
  _refViewHeight->setValue(height);
  checkSizeDefaults();
}


int DefaultLabelPropertiesTab::minimumFontSize() const {
  return _minFontSize->value();
}


void DefaultLabelPropertiesTab::setMinimumFontSize(const int points) {
  _minFontSize->setValue(points);
}


qreal DefaultLabelPropertiesTab::labelScale() const { 
  return _labelFontScale->value(); 
}


void DefaultLabelPropertiesTab::setLabelScale(const qreal scale) { 
  _labelFontScale->setValue(scale);
}


QColor DefaultLabelPropertiesTab::labelColor() const { 
  return _labelColor->color();
}


void DefaultLabelPropertiesTab::setLabelColor(const QColor &color) {
  _labelColor->setColor(color);
}


QFont DefaultLabelPropertiesTab::labelFont() const {
  QFont font(_family->currentFont());
  return font;
}


void DefaultLabelPropertiesTab::setLabelFont(const QFont &font) {
  _family->setCurrentFont(font);
}

}

// vim: ts=2 sw=2 et
