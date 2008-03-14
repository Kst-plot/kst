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

#include "generaltab.h"

namespace Kst {

GeneralTab::GeneralTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("General"));

  connect(_useOpenGL, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_refViewWidth, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_refViewHeight, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_refFontSize, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_minFontSize, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_defaultFontFamily, SIGNAL(currentFontChanged(const QFont &)), this, SIGNAL(modified()));
}


GeneralTab::~GeneralTab() {
}


bool GeneralTab::useOpenGL() const {
  return _useOpenGL->isChecked();
}


void GeneralTab::setUseOpenGL(const bool useOpenGL) {
  _useOpenGL->setChecked(useOpenGL);
}


int GeneralTab::referenceViewWidth() const {
  return _refViewWidth->value();
}


void GeneralTab::setReferenceViewWidth(const int width) {
  _refViewWidth->setValue(width);
}


int GeneralTab::referenceViewHeight() const {
  return _refViewHeight->value();
}


void GeneralTab::setReferenceViewHeight(const int height) {
  _refViewHeight->setValue(height);
}


int GeneralTab::referenceFontSize() const {
  return _refFontSize->value();
}


void GeneralTab::setReferenceFontSize(const int points) {
  _refFontSize->setValue(points);
}


int GeneralTab::minimumFontSize() const {
  return _minFontSize->value();
}


void GeneralTab::setMinimumFontSize(const int points) {
  _minFontSize->setValue(points);
}


QString GeneralTab::defaultFontFamily() const {
  return _defaultFontFamily->currentFont().family();
}


void GeneralTab::setDefaultFontFamily(const QString &fontFamily) {
  _defaultFontFamily->setCurrentFont(QFont(fontFamily));
}

}

// vim: ts=2 sw=2 et
