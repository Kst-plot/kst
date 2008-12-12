/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
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
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(title);

  connect(_fontSize, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_bold, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_underline, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_italic, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_family, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_labelColor, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));
}


OverrideLabelTab::~OverrideLabelTab() {
}


QFont OverrideLabelTab::labelFont() const {
  QFont font(_family->currentFont());
  font.setItalic(_italic->isChecked());
  font.setBold(_bold->isChecked());
  font.setUnderline(_underline->isChecked());
  return font;
}


void OverrideLabelTab::setLabelFont(const QFont &font) {
  _family->setCurrentFont(font);
  _bold->setChecked(font.bold());
  _underline->setChecked(font.underline());
  _italic->setChecked(font.italic());
}


qreal OverrideLabelTab::labelFontScale() const {
  return _fontSize->value();
}


void OverrideLabelTab::setLabelFontScale(const qreal scale) {
  _fontSize->setValue(scale);
}


QColor OverrideLabelTab::labelColor() const { 
  return _labelColor->color();
}


void OverrideLabelTab::setLabelColor(const QColor &color) {
  _labelColor->setColor(color);
}
}

// vim: ts=2 sw=2 et
