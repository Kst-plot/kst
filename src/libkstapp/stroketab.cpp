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

#include "stroketab.h"

namespace Kst {

StrokeTab::StrokeTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Stroke"));

  _style->addItem("NoPen", Qt::NoPen);
  _style->addItem("SolidLine", Qt::SolidLine);
  _style->addItem("DashLine", Qt::DashLine);
  _style->addItem("DotLine", Qt::DotLine);
  _style->addItem("DashDotLine", Qt::DashDotLine);
  _style->addItem("DashDotDotLine", Qt::DashDotDotLine);
  _style->addItem("CustomDashLine", Qt::CustomDashLine);

  _brushStyle->addItem("NoBrush", Qt::NoBrush);
  _brushStyle->addItem("SolidPattern", Qt::SolidPattern);
  _brushStyle->addItem("Dense1Pattern", Qt::Dense1Pattern);
  _brushStyle->addItem("Dense2Pattern", Qt::Dense2Pattern);
  _brushStyle->addItem("Dense3Pattern", Qt::Dense3Pattern);
  _brushStyle->addItem("Dense4Pattern", Qt::Dense4Pattern);
  _brushStyle->addItem("Dense5Pattern", Qt::Dense5Pattern);
  _brushStyle->addItem("Dense6Pattern", Qt::Dense6Pattern);
  _brushStyle->addItem("Dense7Pattern", Qt::Dense7Pattern);
  _brushStyle->addItem("HorPattern", Qt::HorPattern);
  _brushStyle->addItem("VerPattern", Qt::VerPattern);
  _brushStyle->addItem("CrossPattern", Qt::CrossPattern);
  _brushStyle->addItem("BDiagPattern", Qt::BDiagPattern);
  _brushStyle->addItem("FDiagPattern", Qt::FDiagPattern);
  _brushStyle->addItem("DiagCrossPattern", Qt::DiagCrossPattern);

  _joinStyle->addItem("MiterJoin", Qt::MiterJoin);
  _joinStyle->addItem("BevelJoin", Qt::BevelJoin);
  _joinStyle->addItem("RoundJoin", Qt::RoundJoin);
  _joinStyle->addItem("SvgMiterJoin", Qt::SvgMiterJoin);

  _capStyle->addItem("FlatCap", Qt::FlatCap);
  _capStyle->addItem("SquareCap", Qt::SquareCap);
  _capStyle->addItem("RoundCap", Qt::RoundCap);

  connect(_style, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_width, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_brushColor, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));
  connect(_brushStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_joinStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_capStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
}


StrokeTab::~StrokeTab() {
}

Qt::PenStyle StrokeTab::style() const {
  return Qt::PenStyle(_style->itemData(_style->currentIndex()).toInt());
}


bool StrokeTab::styleDirty() const {
  return _style->currentIndex() != -1;
}


void StrokeTab::setStyle(Qt::PenStyle style) {
  _style->setCurrentIndex(_style->findData(QVariant(style)));
}


qreal StrokeTab::width() const {
  return _width->value();
}


bool StrokeTab::widthDirty() const {
  return (!_width->text().isEmpty());
}


void StrokeTab::setWidth(qreal width) {
  _width->setValue(width);
}


QColor StrokeTab::brushColor() const {
  return _brushColor->color();
}


bool StrokeTab::brushColorDirty() const {
  return _brushColor->colorDirty();
}


void StrokeTab::setBrushColor(const QColor &color) {
  _brushColor->setColor(color);
}


Qt::BrushStyle StrokeTab::brushStyle() const {
  return Qt::BrushStyle(_brushStyle->itemData(_brushStyle->currentIndex()).toInt());
}


bool StrokeTab::brushStyleDirty() const {
  return _brushStyle->currentIndex() != -1;
}


void StrokeTab::setBrushStyle(Qt::BrushStyle style) {
  _brushStyle->setCurrentIndex(_brushStyle->findData(QVariant(style)));
}


Qt::PenJoinStyle StrokeTab::joinStyle() const {
  return Qt::PenJoinStyle(_joinStyle->itemData(_joinStyle->currentIndex()).toInt());
}


bool StrokeTab::joinStyleDirty() const {
  return _joinStyle->currentIndex() != -1;
}


void StrokeTab::setJoinStyle(Qt::PenJoinStyle style) {
  _joinStyle->setCurrentIndex(_joinStyle->findData(QVariant(style)));
}


Qt::PenCapStyle StrokeTab::capStyle() const {
  return Qt::PenCapStyle(_capStyle->itemData(_capStyle->currentIndex()).toInt());
}


bool StrokeTab::capStyleDirty() const {
  return _capStyle->currentIndex() != -1;
}


void StrokeTab::setCapStyle(Qt::PenCapStyle style) {
  _capStyle->setCurrentIndex(_capStyle->findData(QVariant(style)));
}


void StrokeTab::clearTabValues() {
  _width->clear();

  _style->setCurrentIndex(-1);
  _brushStyle->setCurrentIndex(-1);
  _joinStyle->setCurrentIndex(-1);
  _capStyle->setCurrentIndex(-1);

  _brushColor->clearSelection();
}


}

// vim: ts=2 sw=2 et
