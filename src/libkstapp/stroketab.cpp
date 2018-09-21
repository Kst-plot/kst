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

#include <QDebug>

#include "stroketab.h"

namespace Kst {

StrokeTab::StrokeTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Stroke"));

  _style->addItem(tr("No Pen", "Pen type"), (int)Qt::NoPen);
  _style->addItem(tr("Solid Line", "Pen type"), (int)Qt::SolidLine);
  _style->addItem(tr("Dash Line", "Pen type"), (int)Qt::DashLine);
  _style->addItem(tr("Dot Line", "Pen type"), (int)Qt::DotLine);
  _style->addItem(tr("Dash Dot Line", "Pen type"), (int)Qt::DashDotLine);
  _style->addItem(tr("Dash Dot Dot Line", "Pen type"), (int)Qt::DashDotDotLine);
  _style->addItem(tr("Custom Dash Line", "Pen type"), (int)Qt::CustomDashLine);

  _brushStyle->addItem(tr("No Brush", "Brush type"), (int)Qt::NoBrush);
  _brushStyle->addItem(tr("Solid Pattern", "Brush type"), (int)Qt::SolidPattern);
  _brushStyle->addItem(tr("Dense Pattern 1", "Brush type"), (int)Qt::Dense1Pattern);
  _brushStyle->addItem(tr("Dense Pattern 2", "Brush type"), (int)Qt::Dense2Pattern);
  _brushStyle->addItem(tr("Dense Pattern 3", "Brush type"), (int)Qt::Dense3Pattern);
  _brushStyle->addItem(tr("Dense Pattern 4", "Brush type"), (int)Qt::Dense4Pattern);
  _brushStyle->addItem(tr("Dense Pattern 5", "Brush type"), (int)Qt::Dense5Pattern);
  _brushStyle->addItem(tr("Dense Pattern 6", "Brush type"), (int)Qt::Dense6Pattern);
  _brushStyle->addItem(tr("Dense Pattern 7", "Brush type"), (int)Qt::Dense7Pattern);
  _brushStyle->addItem(tr("Horizontal Pattern", "Brush type"), (int)Qt::HorPattern);
  _brushStyle->addItem(tr("Vertical Pattern", "Brush type"), (int)Qt::VerPattern);
  _brushStyle->addItem(tr("Cross Pattern", "Brush type"), (int)Qt::CrossPattern);
  _brushStyle->addItem(tr("Diagonal Pattern 1", "Brush type"), (int)Qt::BDiagPattern);
  _brushStyle->addItem(tr("Diagonal Pattern 2", "Brush type"), (int)Qt::FDiagPattern);
  _brushStyle->addItem(tr("Diagonal Cross Pattern", "Brush type"), (int)Qt::DiagCrossPattern);

  _joinStyle->addItem(tr("Miter Join", "line join style"), Qt::MiterJoin);
  _joinStyle->addItem(tr("Bevel Join", "line join style"), Qt::BevelJoin);
  _joinStyle->addItem(tr("Round Join", "line join style"), Qt::RoundJoin);
  _joinStyle->addItem(tr("SVG Miter Join", "line join style based on SVG files"), Qt::SvgMiterJoin);

  _capStyle->addItem(tr("Flat Cap", "line cap style"), Qt::FlatCap);
  _capStyle->addItem(tr("Square Cap", "line cap style"),  Qt::SquareCap);
  _capStyle->addItem(tr("Round Cap", "line cap style"),  Qt::RoundCap);

  connect(_style, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_width, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_brushColor, SIGNAL(changed(QColor)), this, SIGNAL(modified()));
  connect(_brushStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_joinStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_capStyle, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
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
