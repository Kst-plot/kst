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

#include "filltab.h"

#include <QDebug>

namespace Kst {

FillTab::FillTab(QWidget *parent)
  : DialogTab(parent), _multiEdit(false)
 {

  setupUi(this);
  setTabTitle(tr("Fill"));

  _style->addItem("NoBrush", (int)Qt::NoBrush);
  _style->addItem("SolidPattern", (int)Qt::SolidPattern);
  _style->addItem("Dense1Pattern", (int)Qt::Dense1Pattern);
  _style->addItem("Dense2Pattern", (int)Qt::Dense2Pattern);
  _style->addItem("Dense3Pattern", (int)Qt::Dense3Pattern);
  _style->addItem("Dense4Pattern", (int)Qt::Dense4Pattern);
  _style->addItem("Dense5Pattern", (int)Qt::Dense5Pattern);
  _style->addItem("Dense6Pattern", (int)Qt::Dense6Pattern);
  _style->addItem("Dense7Pattern", (int)Qt::Dense7Pattern);
  _style->addItem("HorPattern", (int)Qt::HorPattern);
  _style->addItem("VerPattern", (int)Qt::VerPattern);
  _style->addItem("CrossPattern", (int)Qt::CrossPattern);
  _style->addItem("BDiagPattern", (int)Qt::BDiagPattern);
  _style->addItem("FDiagPattern", (int)Qt::FDiagPattern);
  _style->addItem("DiagCrossPattern", (int)Qt::DiagCrossPattern);

  connect(_color, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));
  connect(_style, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_gradientEditor, SIGNAL(changed(const QGradient &)), this, SIGNAL(modified()));
  connect(_gradientReset, SIGNAL(pressed()), this, SIGNAL(modified()));
  connect(_useGradient, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_gradientReset, SIGNAL(pressed()), _gradientEditor, SLOT(resetGradient()));
  connect(_useGradient, SIGNAL(stateChanged(int)), this, SLOT(updateButtons()));

  updateButtons();
}


FillTab::~FillTab() {
}


void FillTab::updateButtons() { 
  if (!_multiEdit) {
    _color->setEnabled(!_useGradient->isChecked());
    _style->setEnabled(!_useGradient->isChecked());
    _gradientReset->setEnabled(_useGradient->isChecked());
    _gradientEditor->setEnabled(_useGradient->isChecked());
  }
}


QColor FillTab::color() const {
  return _color->color();
}


void FillTab::setColor(const QColor &color) {
  if (color.isValid()) {
    _color->setColor(color);
  } else {
    _color->setColor(Qt::white);
  }
}


bool FillTab::colorDirty() const {
  return _color->colorDirty();
}


Qt::BrushStyle FillTab::style() const {
  return Qt::BrushStyle(_style->itemData(_style->currentIndex()).toInt());
}


bool FillTab::styleDirty() const {
  return _style->currentIndex() != -1;
}


void FillTab::setStyle(Qt::BrushStyle style) {
  if (style == Qt::LinearGradientPattern) {
    _style->setCurrentIndex(Qt::SolidPattern);
  } else {
    _style->setCurrentIndex(_style->findData(QVariant((int)style)));
  }
}


QGradient FillTab::gradient() const {
  if (_useGradient->isChecked()) {
    return _gradientEditor->gradient();
  } else {
    return QGradient();
  }
}


bool FillTab::gradientDirty() const {
  return _gradientEditor->dirty();
}


void FillTab::setGradient(const QGradient &gradient) {
  _useGradient->setChecked(!gradient.stops().empty());
  _gradientEditor->setGradient(gradient);
  updateButtons();
}


bool FillTab::useGradient() const {
  return _useGradient->isChecked();
}


bool FillTab::useGradientDirty() const {
  return _useGradient->checkState() != Qt::PartiallyChecked;
}


void FillTab::setUseGradient(const bool useGradient) {
  _useGradient->setChecked(useGradient);
  updateButtons();
}


void FillTab::clearTabValues() {
  _useGradient->setCheckState(Qt::PartiallyChecked);
  _style->setCurrentIndex(-1);

  _color->clearSelection();

  _color->setEnabled(true);
  _style->setEnabled(true);
  _gradientReset->setEnabled(true);
  _gradientEditor->setEnabled(true);
}


void FillTab::enableSingleEditOptions(bool enabled) {
  _multiEdit = !enabled;
  if (enabled) {
    _useGradient->setTristate(false);
  }
}

void FillTab::initialize(QBrush *b) {
  setColor(b->color());
  setStyle(b->style());

  if (const QGradient *gradient = b->gradient()) {
    setGradient(*gradient);
  } else {
    setUseGradient(false);
  }
}


QBrush FillTab::brush(QBrush b) const {

  QColor this_color = colorDirty() ? color() : b.color();
  Qt::BrushStyle this_style = styleDirty() ? style() : b.style();

  if (useGradientDirty()) {
    // Apply / unapply gradient
    if (useGradient()) {
      b = QBrush(gradient());
    } else {
      b.setColor(this_color);
      b.setStyle(this_style);
    }
  } else {
    // Leave gradient but make other changes.
    QGradient this_gradient;
    if (const QGradient *grad = b.gradient()) {
      if (gradientDirty()) {
        this_gradient = gradient();
      } else {
        this_gradient = *grad;
      }
      b = QBrush(this_gradient);
    } else {
      b.setColor(this_color);
      b.setStyle(this_style);
    }
  }

  return b;
}

}

// vim: ts=2 sw=2 et
