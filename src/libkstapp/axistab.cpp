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

#include "axistab.h"
#include "plotdefines.h"
#include "ksttimezone.h"

namespace Kst {

AxisTab::AxisTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Axis"));

  _axisMajorTickSpacing->addItem(tr("Coarse"), 2);
  _axisMajorTickSpacing->addItem(tr("Normal"), 5);
  _axisMajorTickSpacing->addItem(tr("Fine"), 10);
  _axisMajorTickSpacing->addItem(tr("VeryFine"), 15);
  setAxisMajorTickSpacing(TicksNormal);

  _axisMajorLineStyle->addItem("SolidLine", (int)Qt::SolidLine);
  _axisMajorLineStyle->addItem("DashLine", (int)Qt::DashLine);
  _axisMajorLineStyle->addItem("DotLine", (int)Qt::DotLine);
  _axisMajorLineStyle->addItem("DashDotLine", (int)Qt::DashDotLine);
  _axisMajorLineStyle->addItem("DashDotDotLine", (int)Qt::DashDotDotLine);
  _axisMajorLineStyle->addItem("CustomDashLine", (int)Qt::CustomDashLine);
  setAxisMajorGridLineStyle(Qt::DashLine);

  _axisMinorLineStyle->addItem("SolidLine", (int)Qt::SolidLine);
  _axisMinorLineStyle->addItem("DashLine", (int)Qt::DashLine);
  _axisMinorLineStyle->addItem("DotLine", (int)Qt::DotLine);
  _axisMinorLineStyle->addItem("DashDotLine", (int)Qt::DashDotLine);
  _axisMinorLineStyle->addItem("DashDotDotLine", (int)Qt::DashDotDotLine);
  _axisMinorLineStyle->addItem("CustomDashLine", (int)Qt::CustomDashLine);
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
  connect(_axisMajorLineWidth, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_axisMinorLineWidth, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_autoMinorTicks, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_autoMinorTicks, SIGNAL(stateChanged(int)), this, SLOT(updateButtons()));

  connect(_hideBottomLeft, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_hideTopRight, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));

  connect(_scaleInterpret, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_scaleInterpret, SIGNAL(stateChanged(int)), this, SLOT(updateButtons()));
  connect(_scaleLog, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_scaleReverse, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_scaleDisplayType, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_scaleDisplayFormatString, SIGNAL(textChanged(QString)), this, SIGNAL(modified()));
  connect(_scaleInterpretType, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_timeZone, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));

  connect(_axisMinorTickCount, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));

  connect(_scaleAutoBaseOffset, SIGNAL(stateChanged(int)), this, SLOT(updateButtons()));
  connect(_scaleAutoBaseOffset, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_scaleBaseOffset, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_significantDigits, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));

  connect(_rotation, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));

  _timeZone->addItems(KstTimeZone::tzList());

  _scaleLog->setProperty("si","&Logarithmic");
  _scaleReverse->setProperty("si","&Reverse");
  _scaleInterpret->setProperty("si","&Interpret as:");
  _Label_6->setProperty("si","&Display as:");
  _hideTopRight->setProperty("si","Hide right");
  _hideBottomLeft->setProperty("si","Hide left");
  _scaleAutoBaseOffset->setProperty("si","Auto base / offset mode");
  _scaleBaseOffset->setProperty("si","&Base and offset mode");
  _drawAxisMajorTicks->setProperty("si","Dra&w ticks");
  _drawAxisMajorGridLines->setProperty("si","Draw &grid lines");
  _drawAxisMinorTicks->setProperty("si","Dr&aw ticks");
  _drawAxisMinorGridLines->setProperty("si","Draw grid lines");
  _autoMinorTicks->setProperty("si","Auto");
  _axisMinorLineStyleLabel->setProperty("si","Line st&yle:");
  _scaleLog->setProperty("si","&Logarithmic");
  _scaleReverse->setProperty("si","&Reverse");
  _scaleInterpret->setProperty("si","&Interpret as:");
  _Label_6->setProperty("si","&Display as:");
  _hideTopRight->setProperty("si","Hide top");
  _hideBottomLeft->setProperty("si","Hide bottom");
  _scaleAutoBaseOffset->setProperty("si","Auto base / offset mode");
  _scaleBaseOffset->setProperty("si","&Base and offset mode");
  _drawAxisMajorTicks->setProperty("si","Dra&w ticks");
  _drawAxisMajorGridLines->setProperty("si","Draw &grid lines");
  _drawAxisMinorTicks->setProperty("si","Dr&aw ticks");
  _drawAxisMinorGridLines->setProperty("si","Draw grid lines");
  _autoMinorTicks->setProperty("si","Auto");
  _axisMinorLineStyleLabel->setProperty("si","Line st&yle:");
  label_4->setProperty("si","&range: ");
  label_5->setProperty("si","f&rom: ");
  label_6->setProperty("si","&to: ");

}


AxisTab::~AxisTab() {
}


MajorTickMode AxisTab::axisMajorTickSpacing() const {
  return MajorTickMode(_axisMajorTickSpacing->itemData(_axisMajorTickSpacing->currentIndex()).toInt());
}


bool AxisTab::axisMajorTickSpacingDirty() const {
  return _axisMajorTickSpacing->currentIndex() != -1;
}


void AxisTab::setAxisMajorTickSpacing(MajorTickMode spacing) {
  _axisMajorTickSpacing->setCurrentIndex(_axisMajorTickSpacing->findData(QVariant(spacing)));
}


bool AxisTab::drawAxisMajorTicks() const {
  return _drawAxisMajorTicks->isChecked();
}


bool AxisTab::drawAxisMajorTicksDirty() const {
  return _drawAxisMajorTicks->checkState() != Qt::PartiallyChecked;
}


void AxisTab::setDrawAxisMajorTicks(const bool enabled) {
  _drawAxisMajorTicks->setChecked(enabled);
}


bool AxisTab::drawAxisMajorGridLines() const {
  return _drawAxisMajorGridLines->isChecked();
}


bool AxisTab::drawAxisMajorGridLinesDirty() const {
  return _drawAxisMajorGridLines->checkState() != Qt::PartiallyChecked;
}


void AxisTab::setDrawAxisMajorGridLines(const bool enabled) {
  _drawAxisMajorGridLines->setChecked(enabled);
}


bool AxisTab::drawAxisMinorTicks() const {
  return _drawAxisMinorTicks->isChecked();
}


bool AxisTab::drawAxisMinorTicksDirty() const {
  return _drawAxisMinorTicks->checkState() != Qt::PartiallyChecked;
}


void AxisTab::setDrawAxisMinorTicks(const bool enabled) {
  _drawAxisMinorTicks->setChecked(enabled);
}


bool AxisTab::drawAxisMinorGridLines() const {
  return _drawAxisMinorGridLines->isChecked();
}


bool AxisTab::drawAxisMinorGridLinesDirty() const {
  return _drawAxisMinorGridLines->checkState() != Qt::PartiallyChecked;
}


void AxisTab::setDrawAxisMinorGridLines(const bool enabled) {
  _drawAxisMinorGridLines->setChecked(enabled);
}


Qt::PenStyle AxisTab::axisMajorGridLineStyle() const {
  return Qt::PenStyle(_axisMajorLineStyle->itemData(_axisMajorLineStyle->currentIndex()).toInt());
}


bool AxisTab::axisMajorGridLineStyleDirty() const {
  return _axisMajorLineStyle->currentIndex() != -1;
}


void AxisTab::setAxisMajorGridLineStyle(Qt::PenStyle style) {
  _axisMajorLineStyle->setCurrentIndex(_axisMajorLineStyle->findData(QVariant((int)style)));
}


QColor AxisTab::axisMajorGridLineColor() const {
  return _axisMajorLineColor->color();
}


bool AxisTab::axisMajorGridLineColorDirty() const {
  return _axisMajorLineColor->colorDirty();
}


void AxisTab::setAxisMajorGridLineColor(const QColor &color) {
  _axisMajorLineColor->setColor(color);
}


qreal AxisTab::axisMajorGridLineWidth() const {
  return _axisMajorLineWidth->value();
}


bool AxisTab::axisMajorGridLineWidthDirty() const {
  return (!_axisMajorLineWidth->text().isEmpty());
}


void AxisTab::setAxisMajorGridLineWidth(qreal width) {
  _axisMajorLineWidth->setValue(width);
}


Qt::PenStyle AxisTab::axisMinorGridLineStyle() const {
  return Qt::PenStyle(_axisMinorLineStyle->itemData(_axisMinorLineStyle->currentIndex()).toInt());
}


bool AxisTab::axisMinorGridLineStyleDirty() const {
  return _axisMinorLineStyle->currentIndex() != -1;
}


void AxisTab::setAxisMinorGridLineStyle(Qt::PenStyle style) {
  _axisMinorLineStyle->setCurrentIndex(_axisMinorLineStyle->findData(QVariant((int)style)));
}


QColor AxisTab::axisMinorGridLineColor() const {
  return _axisMinorLineColor->color();
}


bool AxisTab::axisMinorGridLineColorDirty() const {
  return _axisMinorLineColor->colorDirty();
}


void AxisTab::setAxisMinorGridLineColor(const QColor &color) {
  _axisMinorLineColor->setColor(color);
}


qreal AxisTab::axisMinorGridLineWidth() const {
  return _axisMinorLineWidth->value();
}


bool AxisTab::axisMinorGridLineWidthDirty() const {
return (!_axisMinorLineWidth->text().isEmpty());}


void AxisTab::setAxisMinorGridLineWidth(qreal width) {
  _axisMinorLineWidth->setValue(width);
}


int AxisTab::axisMinorTickCount() const {
  return _axisMinorTickCount->value();
}


bool AxisTab::axisMinorTickCountDirty() const {
  return (!_axisMinorTickCount->text().isEmpty());
}


void AxisTab::setAxisMinorTickCount(const int count) {
  _axisMinorTickCount->setValue(count);
}


bool AxisTab::isAutoMinorTickCount() const {
  return _autoMinorTicks->isChecked();
}


bool AxisTab::isAutoMinorTickCountDirty() const {
  return _autoMinorTicks->checkState() != Qt::PartiallyChecked;
}


void AxisTab::setAutoMinorTickCount(const bool enabled) {
  _autoMinorTicks->setChecked(enabled);
}


int AxisTab::significantDigits() const {
  return _significantDigits->value();
}


bool AxisTab::significantDigitsDirty() const {
  return (!_significantDigits->text().isEmpty());
}


void AxisTab::setSignificantDigits(const int digits) {
  _significantDigits->setValue(digits);
}


bool AxisTab::isLog() const {
  return _scaleLog->isChecked();
}


bool AxisTab::isLogDirty() const {
  return _scaleLog->checkState() != Qt::PartiallyChecked;
}


void AxisTab::setLog(const bool enabled) {
  _scaleLog->setChecked(enabled);
}


bool AxisTab::isAutoBaseOffset() const {
  return _scaleAutoBaseOffset->isChecked();
}


bool AxisTab::isAutoBaseOffsetDirty() const {
  return _scaleAutoBaseOffset->checkState() != Qt::PartiallyChecked;
}


void AxisTab::setAutoBaseOffset(const bool enabled) {
  _scaleAutoBaseOffset->setChecked(enabled);
}


bool AxisTab::isBaseOffset() const {
  return _scaleBaseOffset->isChecked();
}


bool AxisTab::isBaseOffsetDirty() const {
  return _scaleBaseOffset->checkState() != Qt::PartiallyChecked;
}


void AxisTab::setBaseOffset(const bool enabled) {
  _scaleBaseOffset->setChecked(enabled);
}


bool AxisTab::isReversed() const {
  return _scaleReverse->isChecked();
}


bool AxisTab::isReversedDirty() const {
  return _scaleReverse->checkState() != Qt::PartiallyChecked;
}


void AxisTab::setReversed(const bool enabled) {
  _scaleReverse->setChecked(enabled);
}


bool AxisTab::isInterpret() const {
  return _scaleInterpret->isChecked();
}


bool AxisTab::isInterpretDirty() const {
  return _scaleInterpret->checkState() != Qt::PartiallyChecked;
}


void AxisTab::setInterpret(const bool enabled) {
  _scaleInterpret->setChecked(enabled);
}


AxisDisplayType AxisTab::axisDisplay() const {
  return AxisDisplayType(_scaleDisplayType->itemData(_scaleDisplayType->currentIndex()).toInt());
}

bool AxisTab::axisDisplayDirty() const {
  return _scaleDisplayType->currentIndex() != -1;
}

void AxisTab::setAxisDisplay(AxisDisplayType display) {
  _scaleDisplayType->setCurrentIndex(_scaleDisplayType->findData(QVariant(display)));
}

QString AxisTab::axisDisplayFormatString() const {
  return _scaleDisplayFormatString->text();
}

bool AxisTab::axisDisplayFormatStringDirty() const {
  return !_scaleDisplayFormatString->text().isEmpty();
}

void AxisTab::setAxisDisplayFormatString(const QString& formatString) {
  _scaleDisplayFormatString->setText(formatString);
}


QString AxisTab::timezone() const {
  return _timeZone->currentText();
}


bool AxisTab::timezoneDirty() const {
  return _timeZone->currentIndex() != -1;
}


void AxisTab::setTimezone(QString timezone) {
  _timeZone->setCurrentIndex(_timeZone->findText(timezone));
}


AxisInterpretationType AxisTab::axisInterpretation() const {
  return AxisInterpretationType(_scaleInterpretType->itemData(_scaleInterpretType->currentIndex()).toInt());
}


bool AxisTab::axisInterpretationDirty() const {
  return _scaleInterpretType->currentIndex() != -1;
}


void AxisTab::setAxisInterpretation(AxisInterpretationType interpret) {
  _scaleInterpretType->setCurrentIndex(_scaleInterpretType->findData(QVariant(interpret)));
}


int AxisTab::labelRotation() const {
  return _rotation->value();
}


bool AxisTab::labelRotationDirty() const {
  return (!_rotation->text().isEmpty());
}


void AxisTab::setLabelRotation(const int rotation) {
  _rotation->setValue(rotation);
}

bool AxisTab::hideTopRight() const {
  return (_hideTopRight->isChecked());
}

bool AxisTab::hideTopRightDirty() const {
  return _hideTopRight->checkState() != Qt::PartiallyChecked;
}

void AxisTab::setHideTopRight(bool hide) {
  _hideTopRight->setChecked(hide);
}

bool AxisTab::hideBottomLeft() const {
  return (_hideBottomLeft->isChecked());
}

bool AxisTab::hideBottomLeftDirty() const {
  return _hideBottomLeft->checkState() != Qt::PartiallyChecked;
}

void AxisTab::setHideBottomLeft(bool hide) {
  _hideBottomLeft->setChecked(hide);
}


void AxisTab::updateButtons() {
  _scaleBaseOffset->setEnabled(!(_scaleInterpret->checkState() == Qt::PartiallyChecked || _scaleAutoBaseOffset->checkState() == Qt::PartiallyChecked));
  _axisMinorTickCount->setEnabled(_autoMinorTicks->checkState() != Qt::Checked);
}


void AxisTab::enableSingleEditOptions(bool enabled) {
  if (enabled) {
    _scaleLog->setTristate(false);
    _scaleBaseOffset->setTristate(false);
    _scaleReverse->setTristate(false);
    _scaleInterpret->setTristate(false);
    _drawAxisMajorTicks->setTristate(false);
    _drawAxisMajorGridLines->setTristate(false);
    _drawAxisMinorTicks->setTristate(false);
    _drawAxisMinorGridLines->setTristate(false);
    _autoMinorTicks->setTristate(false);
  }
}


void AxisTab::clearTabValues() {
  _scaleLog->setCheckState(Qt::PartiallyChecked);
  _scaleAutoBaseOffset->setCheckState(Qt::PartiallyChecked);
  _scaleBaseOffset->setCheckState(Qt::PartiallyChecked);
  _scaleReverse->setCheckState(Qt::PartiallyChecked);
  _scaleInterpret->setCheckState(Qt::PartiallyChecked);
  _autoMinorTicks->setCheckState(Qt::PartiallyChecked);
  _significantDigits->clear();
  _rotation->clear();
  _scaleInterpretType->setCurrentIndex(-1);
  _scaleDisplayType->setCurrentIndex(-1);
  _scaleDisplayFormatString->setText("hh:mm:ss.zzz");
  _timeZone->setCurrentIndex(-1);

  _drawAxisMajorTicks->setCheckState(Qt::PartiallyChecked);
  _drawAxisMajorGridLines->setCheckState(Qt::PartiallyChecked);
  _drawAxisMinorTicks->setCheckState(Qt::PartiallyChecked);
  _drawAxisMinorGridLines->setCheckState(Qt::PartiallyChecked);

  _axisMajorTickSpacing->setCurrentIndex(-1);
  _axisMajorLineStyle->setCurrentIndex(-1);
  _axisMinorLineStyle->setCurrentIndex(-1);
  _axisMajorLineColor->clearSelection();
  _axisMinorLineColor->clearSelection();
  _axisMinorTickCount->clear();

  _hideBottomLeft->setCheckState(Qt::PartiallyChecked);
  _hideTopRight->setCheckState(Qt::PartiallyChecked);
}

void AxisTab::setAsYAxis() {
  _hideBottomLeft->setText(i18n("Hide left"));
  _hideTopRight->setText(i18n("Hide right"));
}

}

// vim: ts=2 sw=2 et

