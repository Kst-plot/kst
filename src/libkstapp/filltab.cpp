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

#include "filltab.h"

namespace Kst {

FillTab::FillTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Fill"));

  _style->addItem("NoBrush", Qt::NoBrush);
  _style->addItem("SolidPattern", Qt::SolidPattern);
  _style->addItem("Dense1Pattern", Qt::Dense1Pattern);
  _style->addItem("Dense2Pattern", Qt::Dense2Pattern);
  _style->addItem("Dense3Pattern", Qt::Dense3Pattern);
  _style->addItem("Dense4Pattern", Qt::Dense4Pattern);
  _style->addItem("Dense5Pattern", Qt::Dense5Pattern);
  _style->addItem("Dense6Pattern", Qt::Dense6Pattern);
  _style->addItem("Dense7Pattern", Qt::Dense7Pattern);
  _style->addItem("HorPattern", Qt::HorPattern);
  _style->addItem("VerPattern", Qt::VerPattern);
  _style->addItem("CrossPattern", Qt::CrossPattern);
  _style->addItem("BDiagPattern", Qt::BDiagPattern);
  _style->addItem("FDiagPattern", Qt::FDiagPattern);
  _style->addItem("DiagCrossPattern", Qt::DiagCrossPattern);

  connect(_color, SIGNAL(changed(const QColor &)), this, SIGNAL(changed()));
  connect(_style, SIGNAL(currentIndexChanged(int)), this, SIGNAL(changed()));
  connect(_gradientEditor, SIGNAL(changed(const QGradient &)), this, SIGNAL(changed()));

  //FIXME gradient editor is disabled for now as it is not ready
  _gradientEditor->setEnabled(false);
}


FillTab::~FillTab() {
}


QColor FillTab::color() const {
  return _color->color();
}


void FillTab::setColor(const QColor &color) {
  _color->setColor(color);
}


Qt::BrushStyle FillTab::style() const {
  return Qt::BrushStyle(_style->itemData(_style->currentIndex()).toInt());
}


void FillTab::setStyle(Qt::BrushStyle style) {
  _style->setCurrentIndex(_style->findData(QVariant(style)));
}


QGradient FillTab::gradient() const {
  return _gradientEditor->gradient();
}


void FillTab::setGradient(const QGradient &gradient) {
  _gradientEditor->setGradient(gradient);
}

}

// vim: ts=2 sw=2 et
