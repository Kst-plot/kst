/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "overridelabeltab.h"

#include "application.h"
#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"

namespace Kst {

OverrideLabelTab::OverrideLabelTab(QString title, QWidget *parent)
  : DialogTab(parent), _fontDirty(false) {

  setupUi(this);
  _bold->setIcon(QPixmap(":kst_bold.png"));
  _bold->setFixedWidth(32);
  _italic->setIcon(QPixmap(":kst_italic.png"));
  _italic->setFixedWidth(32);
  _labelColor->setFixedWidth(32);
  _labelColor->setFixedHeight(32);

  setTabTitle(title);

  connect(_fontSize, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_bold, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_italic, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_family, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_labelColor, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));
  connect(_useDefault, SIGNAL(toggled(bool)), this, SIGNAL(useDefaultChanged(bool)));

  connect(_bold, SIGNAL(toggled(bool)), this, SLOT(buttonUpdate()));
  connect(_italic, SIGNAL(toggled(bool)), this, SLOT(buttonUpdate()));
}


OverrideLabelTab::~OverrideLabelTab() {
}


void OverrideLabelTab::buttonUpdate() {
  _fontDirty = true;
}


QFont OverrideLabelTab::labelFont(QFont ref_font) const {
  QString family = (_family->currentIndex() == -1) ?
                   ref_font.family() : _family->currentFont().family();
  QFont font(family);
  font.setItalic(_italic->isChecked());
  font.setBold(_bold->isChecked());
  return font;
}


bool OverrideLabelTab::labelFontDirty() const {
  return (_family->currentIndex() != -1 || _fontDirty);
}


void OverrideLabelTab::setLabelFont(const QFont &font) {
  _fontDirty = false;
  _family->setCurrentFont(font);
  _bold->setChecked(font.bold());
  _italic->setChecked(font.italic());
}


qreal OverrideLabelTab::labelFontScale() const {
  return _fontSize->value();
}


bool OverrideLabelTab::labelFontScaleDirty() const {
  return (!_fontSize->text().isEmpty());
}


void OverrideLabelTab::setLabelFontScale(const qreal scale) {
  _fontSize->setValue(scale);
}


QColor OverrideLabelTab::labelColor() const { 
  return _labelColor->color();
}


bool OverrideLabelTab::labelColorDirty() const {
  return _labelColor->colorDirty();
}


void OverrideLabelTab::setLabelColor(const QColor &color) {
  _labelColor->setColor(color);
}


void OverrideLabelTab::setFontSpecsIfDefault(const QFont &font, const qreal scale, const QColor &color) {
  if (_useDefault->isChecked()) {
    _fontDirty = false;
    setLabelFontScale(scale);
    setLabelFont(font);
    setLabelColor(color);
  }
}


void OverrideLabelTab::setUseDefault(bool use_default) {
  _useDefault->setChecked(use_default);
}


bool OverrideLabelTab::useDefault() const {
  return (_useDefault->isChecked());
}


bool OverrideLabelTab::useDefaultDirty() const {
  return _useDefault->checkState() != Qt::PartiallyChecked;
}


void OverrideLabelTab::clearTabValues() {
  _fontDirty = false;
  _useDefault->setCheckState(Qt::PartiallyChecked);
  _fontSize->clear();
  _family->setCurrentIndex(-1);

  _bold->setChecked(false);
  _italic->setChecked(false);

  _labelColor->clearSelection();
}


void OverrideLabelTab::enableSingleEditOptions(bool enabled) {
  if (enabled) {
    _useDefault->setTristate(false);
  }
}


}

// vim: ts=2 sw=2 et
