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

#include "labelcreator.h"

#include "application.h"
#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"

#include "applicationsettings.h"
#include "dialogdefaults.h"


namespace Kst {

LabelCreator::LabelCreator(QWidget *parent)
  : QDialog(parent) {

  setupUi(this);
  setWindowTitle(tr("Create Label Dialog"));
  _bold->setFixedWidth(32);
  _bold->setFixedHeight(32);
  _bold->setIcon(QPixmap(":kst_bold.png"));
  _italic->setFixedWidth(32);
  _italic->setFixedHeight(32);
  _italic->setIcon(QPixmap(":kst_italic.png"));
  _labelColor->setFixedWidth(32);
  _labelColor->setFixedHeight(32);

  _labelText->setObjectStore(kstApp->mainWindow()->document()->objectStore());

  QFont font;
  font.fromString(_dialogDefaults->value("label/font",font.toString()).toString());
  _family->setCurrentFont(font);
  _bold->setChecked(font.bold());
  _italic->setChecked(font.italic());

  _labelColor->setColor(_dialogDefaults->value("label/color",QColor(Qt::black)).value<QColor>());
  _labelFontScale->setValue(_dialogDefaults->value("label/fontScale",12).toDouble());

  _saveAsDefault->show();

  _Label_11->setProperty("si","Font &size:");
  _Label_12->setProperty("si","Font famil&y:");
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
  return font;
}

}

// vim: ts=2 sw=2 et
