/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "arrowpropertiestab.h"

namespace Kst {

ArrowPropertiesTab::ArrowPropertiesTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("ArrowProperties"));

  connect(_startArrow, SIGNAL(stateChanged(int)), this, SLOT(update()));
  connect(_endArrow, SIGNAL(stateChanged(int)), this, SLOT(update()));

  connect(_startArrow, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_endArrow, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_startArrowScale, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_endArrowScale, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));

  update();
}


ArrowPropertiesTab::~ArrowPropertiesTab() {
}


void ArrowPropertiesTab::update() {
  _startArrowScale->setEnabled(_startArrow->isChecked());
  _endArrowScale->setEnabled(_endArrow->isChecked());
  _startArrowLabel->setEnabled(_startArrow->isChecked());
  _endArrowLabel->setEnabled(_endArrow->isChecked());
}


bool ArrowPropertiesTab::startArrowHead() { 
  return _startArrow->isChecked(); 
}


void ArrowPropertiesTab::setStartArrowHead(const bool enabled) {
  _startArrow->setChecked(enabled);
}


bool ArrowPropertiesTab::endArrowHead() { 
  return _endArrow->isChecked(); 
}


void ArrowPropertiesTab::setEndArrowHead(const bool enabled) {
  _endArrow->setChecked(enabled);
}


qreal ArrowPropertiesTab::startArrowScale() { 
  return _startArrowScale->value(); 
}


void ArrowPropertiesTab::setStartArrowScale(const qreal scale) { 
  _startArrowScale->setValue(scale);
}


qreal ArrowPropertiesTab::endArrowScale() { 
  return _endArrowScale->value(); 
}


void ArrowPropertiesTab::setEndArrowScale(const qreal scale) { 
  _endArrowScale->setValue(scale);
}

}
// vim: ts=2 sw=2 et
