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

#include "labelpropertiestab.h"

namespace Kst {

LabelPropertiesTab::LabelPropertiesTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Label Properties"));

  connect(_labelText, SIGNAL(textChanged(const QString &)), this, SIGNAL(modified()));
  connect(_labelFontScale, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_labelColor, SIGNAL(changed(const QColor &)), this, SIGNAL(modified()));
}


LabelPropertiesTab::~LabelPropertiesTab() {
}


QString LabelPropertiesTab::labelText() const { 
  return _labelText->text(); 
}


void LabelPropertiesTab::setLabelText(const QString &text) {
  _labelText->setText(text);
}


qreal LabelPropertiesTab::labelScale() const { 
  return _labelFontScale->value(); 
}


void LabelPropertiesTab::setLabelScale(const qreal scale) { 
  _labelFontScale->setValue(scale);
}


QColor LabelPropertiesTab::labelColor() const { 
  return _labelColor->color();
}


void LabelPropertiesTab::setLabelColor(const QColor &color) {
  _labelColor->setColor(color);
}

}
// vim: ts=2 sw=2 et
