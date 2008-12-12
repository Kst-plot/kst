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

#include "labelcreator.h"

#include "application.h"
#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"

#include "applicationsettings.h"

namespace Kst {

LabelCreator::LabelCreator(QWidget *parent)
  : QDialog(parent) {

  setupUi(this);

  _labelText->setObjectStore(kstApp->mainWindow()->document()->objectStore());

  QFont defaultFont(ApplicationSettings::self()->defaultFont());
  _family->setCurrentFont(defaultFont);
  _bold->setChecked(defaultFont.bold());
  _underline->setChecked(defaultFont.underline());
  _italic->setChecked(defaultFont.italic());
  _labelColor->setColor(ApplicationSettings::self()->defaultFontColor());
  _labelFontScale->setValue(ApplicationSettings::self()->defaultFontScale());
}


LabelCreator::~LabelCreator() {
}


QString LabelCreator::labelText() {
  return _labelText->labelText();
}


qreal LabelCreator::labelScale() const { 
  return _labelFontScale->value(); 
}


QColor LabelCreator::labelColor() const { 
  return _labelColor->color();
}


QFont LabelCreator::labelFont() const {
  QFont font(_family->currentFont());
  font.setItalic(_italic->isChecked());
  font.setBold(_bold->isChecked());
  font.setUnderline(_underline->isChecked());
  return font;
}

}

// vim: ts=2 sw=2 et
