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
  connect(_refViewWidth, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_refViewHeight, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_refFontSize, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_minFontSize, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));

  _refViewWidth->setEnabled(false);
  _refViewHeight->setEnabled(false);
  _refFontSize->setEnabled(false);
  _minFontSize->setEnabled(false);
}


GeneralTab::~GeneralTab() {
}


bool GeneralTab::useOpenGL() const {
  return _useOpenGL->isChecked();
}


void GeneralTab::setUseOpenGL(bool useOpenGL) {
  _useOpenGL->setChecked(useOpenGL);
}


qreal GeneralTab::referenceViewWidth() const {
  return _refViewWidth->value();
}


void GeneralTab::setReferenceViewWidth(qreal width) {
  _refViewWidth->setValue(width);
}


qreal GeneralTab::referenceViewHeight() const {
  return _refViewHeight->value();
}


void GeneralTab::setReferenceViewHeight(qreal height) {
  _refViewHeight->setValue(height);
}


int GeneralTab::referenceFontSize() const {
  return _refFontSize->value();
}


void GeneralTab::setReferenceFontSize(int points) {
  _refFontSize->setValue(points);
}


int GeneralTab::minimumFontSize() const {
  return _minFontSize->value();
}


void GeneralTab::setMinimumFontSize(int points) {
  _minFontSize->setValue(points);
}

}

// vim: ts=2 sw=2 et
