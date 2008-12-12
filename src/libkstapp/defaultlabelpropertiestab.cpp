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

#include "defaultlabelpropertiestab.h"

namespace Kst {

DefaultLabelPropertiesTab::DefaultLabelPropertiesTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Default Label Properties"));

  connect(_labelFontScale, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_labelColor, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));
  connect(_bold, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_underline, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_italic, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_family, SIGNAL(currentFontChanged(const QFont &)), this, SIGNAL(modified()));
}


DefaultLabelPropertiesTab::~DefaultLabelPropertiesTab() {
}


qreal DefaultLabelPropertiesTab::labelScale() const { 
  return _labelFontScale->value(); 
}


void DefaultLabelPropertiesTab::setLabelScale(const qreal scale) { 
  _labelFontScale->setValue(scale);
}


QColor DefaultLabelPropertiesTab::labelColor() const { 
  return _labelColor->color();
}


void DefaultLabelPropertiesTab::setLabelColor(const QColor &color) {
  _labelColor->setColor(color);
}


QFont DefaultLabelPropertiesTab::labelFont() const {
  QFont font(_family->currentFont());
  font.setItalic(_italic->isChecked());
  font.setBold(_bold->isChecked());
  font.setUnderline(_underline->isChecked());
  return font;
}


void DefaultLabelPropertiesTab::setLabelFont(const QFont &font) {
  _family->setCurrentFont(font);
  _bold->setChecked(font.bold());
  _underline->setChecked(font.underline());
  _italic->setChecked(font.italic());
}

}

// vim: ts=2 sw=2 et
