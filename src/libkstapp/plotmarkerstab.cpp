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

#include "plotmarkerstab.h"

namespace Kst {

PlotMarkersTab::PlotMarkersTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Plot Markers"));

  _xAxisMajorTickSpacing->addItem(tr("Coarse"), 2);
  _xAxisMajorTickSpacing->addItem(tr("Normal"), 5);
  _xAxisMajorTickSpacing->addItem(tr("Fine"), 10);
  _xAxisMajorTickSpacing->addItem(tr("VeryFine"), 15);
  setXAxisMajorTickSpacing(PlotItem::Normal);

  _yAxisMajorTickSpacing->addItem(tr("Coarse"), 2);
  _yAxisMajorTickSpacing->addItem(tr("Normal"), 5);
  _yAxisMajorTickSpacing->addItem(tr("Fine"), 10);
  _yAxisMajorTickSpacing->addItem(tr("VeryFine"), 15);
  setYAxisMajorTickSpacing(PlotItem::Normal);

  _xAxisMajorLineStyle->addItem("SolidLine", Qt::SolidLine);
  _xAxisMajorLineStyle->addItem("DashLine", Qt::DashLine);
  _xAxisMajorLineStyle->addItem("DotLine", Qt::DotLine);
  _xAxisMajorLineStyle->addItem("DashDotLine", Qt::DashDotLine);
  _xAxisMajorLineStyle->addItem("DashDotDotLine", Qt::DashDotDotLine);
  _xAxisMajorLineStyle->addItem("CustomDashLine", Qt::CustomDashLine);
  setXAxisMajorGridLineStyle(Qt::DashLine);

  _xAxisMinorLineStyle->addItem("SolidLine", Qt::SolidLine);
  _xAxisMinorLineStyle->addItem("DashLine", Qt::DashLine);
  _xAxisMinorLineStyle->addItem("DotLine", Qt::DotLine);
  _xAxisMinorLineStyle->addItem("DashDotLine", Qt::DashDotLine);
  _xAxisMinorLineStyle->addItem("DashDotDotLine", Qt::DashDotDotLine);
  _xAxisMinorLineStyle->addItem("CustomDashLine", Qt::CustomDashLine);
  setXAxisMinorGridLineStyle(Qt::DashLine);

  _yAxisMajorLineStyle->addItem("SolidLine", Qt::SolidLine);
  _yAxisMajorLineStyle->addItem("DashLine", Qt::DashLine);
  _yAxisMajorLineStyle->addItem("DotLine", Qt::DotLine);
  _yAxisMajorLineStyle->addItem("DashDotLine", Qt::DashDotLine);
  _yAxisMajorLineStyle->addItem("DashDotDotLine", Qt::DashDotDotLine);
  _yAxisMajorLineStyle->addItem("CustomDashLine", Qt::CustomDashLine);
  setYAxisMajorGridLineStyle(Qt::DashLine);

  _yAxisMinorLineStyle->addItem("SolidLine", Qt::SolidLine);
  _yAxisMinorLineStyle->addItem("DashLine", Qt::DashLine);
  _yAxisMinorLineStyle->addItem("DotLine", Qt::DotLine);
  _yAxisMinorLineStyle->addItem("DashDotLine", Qt::DashDotLine);
  _yAxisMinorLineStyle->addItem("DashDotDotLine", Qt::DashDotDotLine);
  _yAxisMinorLineStyle->addItem("CustomDashLine", Qt::CustomDashLine);
  setYAxisMinorGridLineStyle(Qt::DashLine);

  setXAxisMajorGridLineColor(Qt::gray);
  setXAxisMinorGridLineColor(Qt::gray);
  setYAxisMajorGridLineColor(Qt::gray);
  setYAxisMinorGridLineColor(Qt::gray);

  connect(_drawXAxisMajorTicks, SIGNAL(stateChanged(int)), this, SLOT(update()));
  connect(_drawXAxisMajorGridLines, SIGNAL(stateChanged(int)), this, SLOT(update()));
  connect(_drawYAxisMajorTicks, SIGNAL(stateChanged(int)), this, SLOT(update()));
  connect(_drawYAxisMajorGridLines, SIGNAL(stateChanged(int)), this, SLOT(update()));
  connect(_drawXAxisMinorGridLines, SIGNAL(stateChanged(int)), this, SLOT(update()));
  connect(_drawYAxisMinorGridLines, SIGNAL(stateChanged(int)), this, SLOT(update()));

  connect(_drawXAxisMajorTicks, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_drawXAxisMajorGridLines, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_drawXAxisMinorGridLines, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_drawXAxisMinorTicks, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_xAxisMajorTickSpacing, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_xAxisMajorLineStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_xAxisMinorLineStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_xAxisMajorLineColor, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));
  connect(_xAxisMinorLineColor, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));

  connect(_drawYAxisMajorTicks, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_drawYAxisMajorGridLines, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_drawYAxisMinorGridLines, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_drawYAxisMinorTicks, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_yAxisMajorTickSpacing, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_yAxisMajorLineStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_yAxisMinorLineStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_yAxisMajorLineColor, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));
  connect(_yAxisMinorLineColor, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));

  connect(_xAxisMinorTickCount, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
  connect(_yAxisMinorTickCount, SIGNAL(valueChanged(int)), this, SIGNAL(modified()));
}


PlotMarkersTab::~PlotMarkersTab() {
}


void PlotMarkersTab::update() {
  bool xOptionsEnabled = drawXAxisMajorTicks() || drawXAxisMajorGridLines();
  bool yOptionsEnabled = drawYAxisMajorTicks() || drawYAxisMajorGridLines();
  bool xMinorOptionsEnabled = drawXAxisMinorTicks() || drawXAxisMinorGridLines();
  bool yMinorOptionsEnabled = drawYAxisMinorTicks() || drawYAxisMinorGridLines();

  _drawXAxisMinorGridLines->setEnabled(xOptionsEnabled);
  _drawXAxisMinorTicks->setEnabled(xOptionsEnabled);
  _xAxisMajorTickSpacing->setEnabled(xOptionsEnabled);

  _drawYAxisMinorGridLines->setEnabled(yOptionsEnabled);
  _drawYAxisMinorTicks->setEnabled(yOptionsEnabled);
  _yAxisMajorTickSpacing->setEnabled(yOptionsEnabled);

  _xAxisMinorTickCount->setEnabled(xMinorOptionsEnabled);
  _yAxisMinorTickCount->setEnabled(yMinorOptionsEnabled);

  _xAxisMajorGridGroup->setEnabled(_drawXAxisMajorGridLines->isChecked() && _drawXAxisMajorGridLines->isEnabled());
  _xAxisMinorGridGroup->setEnabled(_drawXAxisMinorGridLines->isChecked() && _drawXAxisMinorGridLines->isEnabled());
  _yAxisMajorGridGroup->setEnabled(_drawYAxisMajorGridLines->isChecked() && _drawYAxisMajorGridLines->isEnabled());
  _yAxisMinorGridGroup->setEnabled(_drawYAxisMinorGridLines->isChecked() && _drawYAxisMinorGridLines->isEnabled());
}


PlotItem::MajorTickMode PlotMarkersTab::xAxisMajorTickSpacing() const {
  return PlotItem::MajorTickMode(_xAxisMajorTickSpacing->itemData(_xAxisMajorTickSpacing->currentIndex()).toInt());
}


void PlotMarkersTab::setXAxisMajorTickSpacing(PlotItem::MajorTickMode spacing) {
  _xAxisMajorTickSpacing->setCurrentIndex(_xAxisMajorTickSpacing->findData(QVariant(spacing)));
}


bool PlotMarkersTab::drawXAxisMajorTicks() const {
  return _drawXAxisMajorTicks->isChecked();
}


void PlotMarkersTab::setDrawXAxisMajorTicks(const bool enabled) {
  _drawXAxisMajorTicks->setChecked(enabled);
}


bool PlotMarkersTab::drawXAxisMajorGridLines() const {
  return _drawXAxisMajorGridLines->isChecked();
}


void PlotMarkersTab::setDrawXAxisMajorGridLines(const bool enabled) {
  _drawXAxisMajorGridLines->setChecked(enabled);
}


bool PlotMarkersTab::drawXAxisMinorTicks() const {
  return _drawXAxisMinorTicks->isChecked();
}


void PlotMarkersTab::setDrawXAxisMinorTicks(const bool enabled) {
  _drawXAxisMinorTicks->setChecked(enabled);
}


bool PlotMarkersTab::drawXAxisMinorGridLines() const {
  return _drawXAxisMinorGridLines->isChecked();
}


void PlotMarkersTab::setDrawXAxisMinorGridLines(const bool enabled) {
  _drawXAxisMinorGridLines->setChecked(enabled);
}


PlotItem::MajorTickMode PlotMarkersTab::yAxisMajorTickSpacing() const {
  return PlotItem::MajorTickMode(_yAxisMajorTickSpacing->itemData(_yAxisMajorTickSpacing->currentIndex()).toInt());
}


void PlotMarkersTab::setYAxisMajorTickSpacing(PlotItem::MajorTickMode spacing) {
  _yAxisMajorTickSpacing->setCurrentIndex(_yAxisMajorTickSpacing->findData(QVariant(spacing)));
}


bool PlotMarkersTab::drawYAxisMajorTicks() const {
  return _drawYAxisMajorTicks->isChecked();
}


void PlotMarkersTab::setDrawYAxisMajorTicks(const bool enabled) {
  _drawYAxisMajorTicks->setChecked(enabled);
}


bool PlotMarkersTab::drawYAxisMajorGridLines() const {
  return _drawYAxisMajorGridLines->isChecked();
}


void PlotMarkersTab::setDrawYAxisMajorGridLines(const bool enabled) {
  _drawYAxisMajorGridLines->setChecked(enabled);
}


bool PlotMarkersTab::drawYAxisMinorTicks() const {
  return _drawYAxisMinorTicks->isChecked();
}


void PlotMarkersTab::setDrawYAxisMinorTicks(const bool enabled) {
  _drawYAxisMinorTicks->setChecked(enabled);
}


bool PlotMarkersTab::drawYAxisMinorGridLines() const {
  return _drawYAxisMinorGridLines->isChecked();
}


void PlotMarkersTab::setDrawYAxisMinorGridLines(const bool enabled) {
  _drawYAxisMinorGridLines->setChecked(enabled);
}


Qt::PenStyle PlotMarkersTab::xAxisMajorGridLineStyle() const {
  return Qt::PenStyle(_xAxisMajorLineStyle->itemData(_xAxisMajorLineStyle->currentIndex()).toInt());
}


void PlotMarkersTab::setXAxisMajorGridLineStyle(Qt::PenStyle style) {
  _xAxisMajorLineStyle->setCurrentIndex(_xAxisMajorLineStyle->findData(QVariant(style)));
}


QColor PlotMarkersTab::xAxisMajorGridLineColor() const {
  return _xAxisMajorLineColor->color();
}


void PlotMarkersTab::setXAxisMajorGridLineColor(const QColor &color) {
  _xAxisMajorLineColor->setColor(color);
}


Qt::PenStyle PlotMarkersTab::xAxisMinorGridLineStyle() const {
  return Qt::PenStyle(_xAxisMinorLineStyle->itemData(_xAxisMinorLineStyle->currentIndex()).toInt());
}


void PlotMarkersTab::setXAxisMinorGridLineStyle(Qt::PenStyle style) {
  _xAxisMinorLineStyle->setCurrentIndex(_xAxisMinorLineStyle->findData(QVariant(style)));
}


QColor PlotMarkersTab::xAxisMinorGridLineColor() const {
  return _xAxisMinorLineColor->color();
}


void PlotMarkersTab::setXAxisMinorGridLineColor(const QColor &color) {
  _xAxisMinorLineColor->setColor(color);
}


Qt::PenStyle PlotMarkersTab::yAxisMajorGridLineStyle() const {
  return Qt::PenStyle(_yAxisMajorLineStyle->itemData(_yAxisMajorLineStyle->currentIndex()).toInt());
}


void PlotMarkersTab::setYAxisMajorGridLineStyle(Qt::PenStyle style) {
  _yAxisMajorLineStyle->setCurrentIndex(_yAxisMajorLineStyle->findData(QVariant(style)));
}


QColor PlotMarkersTab::yAxisMajorGridLineColor() const {
  return _yAxisMajorLineColor->color();
}


void PlotMarkersTab::setYAxisMajorGridLineColor(const QColor &color) {
  _yAxisMajorLineColor->setColor(color);
}


Qt::PenStyle PlotMarkersTab::yAxisMinorGridLineStyle() const {
  return Qt::PenStyle(_yAxisMinorLineStyle->itemData(_yAxisMinorLineStyle->currentIndex()).toInt());
}


void PlotMarkersTab::setYAxisMinorGridLineStyle(Qt::PenStyle style) {
  _yAxisMinorLineStyle->setCurrentIndex(_yAxisMinorLineStyle->findData(QVariant(style)));
}


QColor PlotMarkersTab::yAxisMinorGridLineColor() const {
  return _yAxisMinorLineColor->color();
}


void PlotMarkersTab::setYAxisMinorGridLineColor(const QColor &color) {
  _yAxisMinorLineColor->setColor(color);
}


int PlotMarkersTab::xAxisMinorTickCount() const {
  return _xAxisMinorTickCount->value();
}


void PlotMarkersTab::setXAxisMinorTickCount(const int count) {
  _xAxisMinorTickCount->setValue(count);
}


int PlotMarkersTab::yAxisMinorTickCount() const {
  return _yAxisMinorTickCount->value();
}


void PlotMarkersTab::setYAxisMinorTickCount(const int count) {
  _yAxisMinorTickCount->setValue(count);
}

}

// vim: ts=2 sw=2 et
