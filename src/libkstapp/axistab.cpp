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

#include "axistab.h"
#include "plotdefines.h"

namespace Kst {

AxisTab::AxisTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Axis"));

  _axisMajorTickSpacing->addItem(tr("Coarse"), 2);
  _axisMajorTickSpacing->addItem(tr("Normal"), 5);
  _axisMajorTickSpacing->addItem(tr("Fine"), 10);
  _axisMajorTickSpacing->addItem(tr("VeryFine"), 15);
  setAxisMajorTickSpacing(PlotAxis::Normal);

  _axisMajorLineStyle->addItem("SolidLine", Qt::SolidLine);
  _axisMajorLineStyle->addItem("DashLine", Qt::DashLine);
  _axisMajorLineStyle->addItem("DotLine", Qt::DotLine);
  _axisMajorLineStyle->addItem("DashDotLine", Qt::DashDotLine);
  _axisMajorLineStyle->addItem("DashDotDotLine", Qt::DashDotDotLine);
  _axisMajorLineStyle->addItem("CustomDashLine", Qt::CustomDashLine);
  setAxisMajorGridLineStyle(Qt::DashLine);

  _axisMinorLineStyle->addItem("SolidLine", Qt::SolidLine);
  _axisMinorLineStyle->addItem("DashLine", Qt::DashLine);
  _axisMinorLineStyle->addItem("DotLine", Qt::DotLine);
  _axisMinorLineStyle->addItem("DashDotLine", Qt::DashDotLine);
  _axisMinorLineStyle->addItem("DashDotDotLine", Qt::DashDotDotLine);
  _axisMinorLineStyle->addItem("CustomDashLine", Qt::CustomDashLine);
  setAxisMinorGridLineStyle(Qt::DashLine);

  setAxisMajorGridLineColor(Qt::gray);
  setAxisMinorGridLineColor(Qt::gray);

  for (uint i = 0; i < numAxisDisplays; i++) {
    _scaleDisplayType->addItem(AxisDisplays[i].label, QVariant(AxisDisplays[i].type));
  }

  for (uint i = 0; i < numAxisInterpretations; i++) {
    _scaleInterpretType->addItem(AxisInterpretations[i].label, QVariant(AxisInterpretations[i].type));
  }

  connect(_drawAxisMajorTicks, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_drawAxisMajorGridLines, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_drawAxisMinorGridLines, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_drawAxisMinorTicks, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_axisMajorTickSpacing, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_axisMajorLineStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_axisMinorLineStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_axisMajorLineColor, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));
  connect(_axisMinorLineColor, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));

  connect(_scaleInterpret, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_scaleInterpret, SIGNAL(stateChanged(int)), this, SLOT(updateButtons()));
  connect(_scaleLog, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_scaleBaseOffset, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_scaleReverse, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_scaleDisplayType, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_scaleInterpretType, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));

  connect(_axisMinorTickCount, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_significantDigits, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));

}


AxisTab::~AxisTab() {
}


PlotAxis::MajorTickMode AxisTab::axisMajorTickSpacing() const {
  return PlotAxis::MajorTickMode(_axisMajorTickSpacing->itemData(_axisMajorTickSpacing->currentIndex()).toInt());
}


void AxisTab::setAxisMajorTickSpacing(PlotAxis::MajorTickMode spacing) {
  _axisMajorTickSpacing->setCurrentIndex(_axisMajorTickSpacing->findData(QVariant(spacing)));
}


bool AxisTab::drawAxisMajorTicks() const {
  return _drawAxisMajorTicks->isChecked();
}


void AxisTab::setDrawAxisMajorTicks(const bool enabled) {
  _drawAxisMajorTicks->setChecked(enabled);
}


bool AxisTab::drawAxisMajorGridLines() const {
  return _drawAxisMajorGridLines->isChecked();
}


void AxisTab::setDrawAxisMajorGridLines(const bool enabled) {
  _drawAxisMajorGridLines->setChecked(enabled);
}


bool AxisTab::drawAxisMinorTicks() const {
  return _drawAxisMinorTicks->isChecked();
}


void AxisTab::setDrawAxisMinorTicks(const bool enabled) {
  _drawAxisMinorTicks->setChecked(enabled);
}


bool AxisTab::drawAxisMinorGridLines() const {
  return _drawAxisMinorGridLines->isChecked();
}


void AxisTab::setDrawAxisMinorGridLines(const bool enabled) {
  _drawAxisMinorGridLines->setChecked(enabled);
}


Qt::PenStyle AxisTab::axisMajorGridLineStyle() const {
  return Qt::PenStyle(_axisMajorLineStyle->itemData(_axisMajorLineStyle->currentIndex()).toInt());
}


void AxisTab::setAxisMajorGridLineStyle(Qt::PenStyle style) {
  _axisMajorLineStyle->setCurrentIndex(_axisMajorLineStyle->findData(QVariant(style)));
}


QColor AxisTab::axisMajorGridLineColor() const {
  return _axisMajorLineColor->color();
}


void AxisTab::setAxisMajorGridLineColor(const QColor &color) {
  _axisMajorLineColor->setColor(color);
}


Qt::PenStyle AxisTab::axisMinorGridLineStyle() const {
  return Qt::PenStyle(_axisMinorLineStyle->itemData(_axisMinorLineStyle->currentIndex()).toInt());
}


void AxisTab::setAxisMinorGridLineStyle(Qt::PenStyle style) {
  _axisMinorLineStyle->setCurrentIndex(_axisMinorLineStyle->findData(QVariant(style)));
}


QColor AxisTab::axisMinorGridLineColor() const {
  return _axisMinorLineColor->color();
}


void AxisTab::setAxisMinorGridLineColor(const QColor &color) {
  _axisMinorLineColor->setColor(color);
}


int AxisTab::axisMinorTickCount() const {
  return _axisMinorTickCount->value();
}


void AxisTab::setAxisMinorTickCount(const int count) {
  _axisMinorTickCount->setValue(count);
}


int AxisTab::significantDigits() const {
  return _significantDigits->value();
}


void AxisTab::setSignificantDigits(const int digits) {
  _significantDigits->setValue(digits);
}


bool AxisTab::isLog() const {
  return _scaleLog->isChecked();
}


void AxisTab::setLog(const bool enabled) {
  _scaleLog->setChecked(enabled);
}


bool AxisTab::isBaseOffset() const {
  return _scaleBaseOffset->isChecked();
}


void AxisTab::setBaseOffset(const bool enabled) {
  _scaleBaseOffset->setChecked(enabled);
}


bool AxisTab::isReversed() const {
  return _scaleReverse->isChecked();
}


void AxisTab::setReversed(const bool enabled) {
  _scaleReverse->setChecked(enabled);
}


bool AxisTab::isInterpret() const {
  return _scaleInterpret->isChecked();
}


void AxisTab::setInterpret(const bool enabled) {
  _scaleInterpret->setChecked(enabled);
}


AxisDisplayType AxisTab::axisDisplay() const {
  return AxisDisplayType(_scaleDisplayType->itemData(_scaleDisplayType->currentIndex()).toInt());
}


void AxisTab::setAxisDisplay(AxisDisplayType display) {
  _scaleDisplayType->setCurrentIndex(_scaleDisplayType->findData(QVariant(display)));
}


AxisInterpretationType AxisTab::axisInterpretation() const {
  return AxisInterpretationType(_scaleInterpretType->itemData(_scaleInterpretType->currentIndex()).toInt());
}


void AxisTab::setAxisInterpretation(AxisInterpretationType interpret) {
  _scaleInterpretType->setCurrentIndex(_scaleInterpretType->findData(QVariant(interpret)));
}


void AxisTab::updateButtons() {
  _scaleBaseOffset->setEnabled(!isInterpret());
}

}

// vim: ts=2 sw=2 et
