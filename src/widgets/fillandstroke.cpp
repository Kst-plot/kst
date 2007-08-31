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

#include "fillandstroke.h"

namespace Kst {

FillAndStroke::FillAndStroke(QWidget *parent)
  : QWidget(parent) {

  setupUi(this);

  _strokeStyle->addItem("NoPen", Qt::NoPen);
  _strokeStyle->addItem("SolidLine", Qt::SolidLine);
  _strokeStyle->addItem("DashLine", Qt::DashLine);
  _strokeStyle->addItem("DotLine", Qt::DotLine);
  _strokeStyle->addItem("DashDotLine", Qt::DashDotLine);
  _strokeStyle->addItem("DashDotDotLine", Qt::DashDotDotLine);
  _strokeStyle->addItem("CustomDashLine", Qt::CustomDashLine);

  _fillStyle->addItem("NoBrush", Qt::NoBrush);
  _fillStyle->addItem("SolidPattern", Qt::SolidPattern);
  _fillStyle->addItem("Dense1Pattern", Qt::Dense1Pattern);
  _fillStyle->addItem("Dense2Pattern", Qt::Dense2Pattern);
  _fillStyle->addItem("Dense3Pattern", Qt::Dense3Pattern);
  _fillStyle->addItem("Dense4Pattern", Qt::Dense4Pattern);
  _fillStyle->addItem("Dense5Pattern", Qt::Dense5Pattern);
  _fillStyle->addItem("Dense6Pattern", Qt::Dense6Pattern);
  _fillStyle->addItem("Dense7Pattern", Qt::Dense7Pattern);
  _fillStyle->addItem("HorPattern", Qt::HorPattern);
  _fillStyle->addItem("VerPattern", Qt::VerPattern);
  _fillStyle->addItem("CrossPattern", Qt::CrossPattern);
  _fillStyle->addItem("BDiagPattern", Qt::BDiagPattern);
  _fillStyle->addItem("FDiagPattern", Qt::FDiagPattern);
  _fillStyle->addItem("DiagCrossPattern", Qt::DiagCrossPattern);

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


  connect(_color, SIGNAL(changed(const QColor &)), this, SIGNAL(fillChanged()));
  connect(_fillStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(fillChanged()));
  connect(_gradientEditor, SIGNAL(changed(const QGradient &)), this, SIGNAL(fillChanged()));

  connect(_strokeStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(strokeChanged()));
  connect(_width, SIGNAL(valueChanged(double)), this, SIGNAL(strokeChanged()));
  connect(_brushColor, SIGNAL(changed(const QColor &)), this, SIGNAL(strokeChanged()));
  connect(_brushStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(strokeChanged()));
  connect(_joinStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(strokeChanged()));
  connect(_capStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(strokeChanged()));

  _gradientEditor->setEnabled(false);
}


FillAndStroke::~FillAndStroke() {
}


QColor FillAndStroke::fillColor() const {
  return _color->color();
}


void FillAndStroke::setFillColor(const QColor &color) {
  _color->setColor(color);
}


Qt::BrushStyle FillAndStroke::fillStyle() const {
  return Qt::BrushStyle(_fillStyle->itemData(_fillStyle->currentIndex()).toInt());
}


void FillAndStroke::setFillStyle(Qt::BrushStyle style) {
  _fillStyle->setCurrentIndex(_fillStyle->findData(QVariant(style)));
}


QGradient FillAndStroke::fillGradient() const {
  return _gradientEditor->gradient();
}


void FillAndStroke::setFillGradient(const QGradient &gradient) {
  _gradientEditor->setGradient(gradient);
}


Qt::PenStyle FillAndStroke::strokeStyle() const {
  return Qt::PenStyle(_strokeStyle->itemData(_strokeStyle->currentIndex()).toInt());
}


void FillAndStroke::setStrokeStyle(Qt::PenStyle style) {
  _strokeStyle->setCurrentIndex(_strokeStyle->findData(QVariant(style)));
}


qreal FillAndStroke::strokeWidth() const {
  return _width->value();
}


void FillAndStroke::setStrokeWidth(qreal width) {
  _width->setValue(width);
}


QColor FillAndStroke::brushColor() const {
  return _brushColor->color();
}


void FillAndStroke::setBrushColor(const QColor &color) {
  _brushColor->setColor(color);
}


Qt::BrushStyle FillAndStroke::brushStyle() const {
  return Qt::BrushStyle(_brushStyle->itemData(_brushStyle->currentIndex()).toInt());
}


void FillAndStroke::setBrushStyle(Qt::BrushStyle style) {
  _brushStyle->setCurrentIndex(_brushStyle->findData(QVariant(style)));
}


Qt::PenJoinStyle FillAndStroke::joinStyle() const {
  return Qt::PenJoinStyle(_joinStyle->itemData(_joinStyle->currentIndex()).toInt());
}


void FillAndStroke::setJoinStyle(Qt::PenJoinStyle style) {
  _joinStyle->setCurrentIndex(_joinStyle->findData(QVariant(style)));
}


Qt::PenCapStyle FillAndStroke::capStyle() const {
  return Qt::PenCapStyle(_capStyle->itemData(_capStyle->currentIndex()).toInt());
}


void FillAndStroke::setCapStyle(Qt::PenCapStyle style) {
  _capStyle->setCurrentIndex(_capStyle->findData(QVariant(style)));
}

}

// vim: ts=2 sw=2 et
