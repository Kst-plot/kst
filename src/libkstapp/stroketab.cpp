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

#include "stroketab.h"

namespace Kst {

StrokeTab::StrokeTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Stroke"));

  _style->addItem("NoPen", (int)Qt::NoPen);
  _style->addItem("SolidLine", (int)Qt::SolidLine);
  _style->addItem("DashLine", (int)Qt::DashLine);
  _style->addItem("DotLine", (int)Qt::DotLine);
  _style->addItem("DashDotLine", (int)Qt::DashDotLine);
  _style->addItem("DashDotDotLine", (int)Qt::DashDotDotLine);
  _style->addItem("CustomDashLine", (int)Qt::CustomDashLine);

  _brushStyle->addItem("NoBrush", (int)Qt::NoBrush);
  _brushStyle->addItem("SolidPattern", (int)Qt::SolidPattern);
  _brushStyle->addItem("Dense1Pattern", (int)Qt::Dense1Pattern);
  _brushStyle->addItem("Dense2Pattern", (int)Qt::Dense2Pattern);
  _brushStyle->addItem("Dense3Pattern", (int)Qt::Dense3Pattern);
  _brushStyle->addItem("Dense4Pattern", (int)Qt::Dense4Pattern);
  _brushStyle->addItem("Dense5Pattern", (int)Qt::Dense5Pattern);
  _brushStyle->addItem("Dense6Pattern", (int)Qt::Dense6Pattern);
  _brushStyle->addItem("Dense7Pattern", (int)Qt::Dense7Pattern);
  _brushStyle->addItem("HorPattern", (int)Qt::HorPattern);
  _brushStyle->addItem("VerPattern", (int)Qt::VerPattern);
  _brushStyle->addItem("CrossPattern", (int)Qt::CrossPattern);
  _brushStyle->addItem("BDiagPattern", (int)Qt::BDiagPattern);
  _brushStyle->addItem("FDiagPattern", (int)Qt::FDiagPattern);
  _brushStyle->addItem("DiagCrossPattern", (int)Qt::DiagCrossPattern);

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

  _brushColor->setProperty("si","stroke brush color");
}


StrokeTab::~StrokeTab() {
}


void StrokeTab::initialize(QPen *p) {
  QBrush b = p->brush();

  setStyle(p->style());
  setWidth(p->widthF());

  setBrushColor(b.color());
  setBrushStyle(b.style());

  setJoinStyle(p->joinStyle());
  setCapStyle(p->capStyle());
}

QPen StrokeTab::pen(QPen p) const {
  QBrush b = p.brush();

  Qt::PenStyle this_style = styleDirty() ? style() : p.style();
  qreal this_width = widthDirty() ? width() : p.widthF();
  QColor this_brushColor = brushColorDirty() ? brushColor() : b.color();
  Qt::BrushStyle this_brushStyle = brushStyleDirty() ? brushStyle() : b.style();

  Qt::PenJoinStyle this_joinStyle = joinStyleDirty() ? joinStyle() : p.joinStyle();
  Qt::PenCapStyle this_capStyle = capStyleDirty() ? capStyle() : p.capStyle();


  p.setStyle(this_style);
  p.setWidthF(this_width);

  b.setColor(this_brushColor);
  b.setStyle(this_brushStyle);

  p.setJoinStyle(this_joinStyle);
  p.setCapStyle(this_capStyle);
  p.setBrush(b);
#ifdef Q_OS_WIN32
  if (p.isCosmetic()) {
    p.setWidth(1);
  }
#endif

  return p;
}

Qt::PenStyle StrokeTab::style() const {
  return Qt::PenStyle(_style->itemData(_style->currentIndex()).toInt());
}


bool StrokeTab::styleDirty() const {
  return _style->currentIndex() != -1;
}


void StrokeTab::setStyle(Qt::PenStyle style) {
  _style->setCurrentIndex(_style->findData(QVariant((int)style)));
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
  _brushStyle->setCurrentIndex(_brushStyle->findData(QVariant((int)style)));
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
