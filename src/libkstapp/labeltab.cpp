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

#include "labeltab.h"

#include "application.h"
#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"

#include "applicationsettings.h"

namespace Kst {

LabelTab::LabelTab(PlotItem* plotItem, QWidget *parent)
  : DialogTab(parent), _plotItem(plotItem), _activeLineEdit(0) {

  setupUi(this);
  setTabTitle(tr("Labels"));

  QFont font;
  setGlobalFont(font);

  setGlobalFont(ApplicationSettings::self()->defaultFont());
  _globalLabelColor->setColor(ApplicationSettings::self()->defaultFontColor());
  _globalLabelFontSize->setValue(ApplicationSettings::self()->defaultFontScale());

  _scalars->setObjectStore(kstApp->mainWindow()->document()->objectStore());
  _strings->setObjectStore(kstApp->mainWindow()->document()->objectStore());

  connect(_topLabelText, SIGNAL(textChanged(const QString &)), this, SIGNAL(modified()));
  connect(_leftLabelText, SIGNAL(textChanged(const QString &)), this, SIGNAL(modified()));
  connect(_bottomLabelText, SIGNAL(textChanged(const QString &)), this, SIGNAL(modified()));
  connect(_rightLabelText, SIGNAL(textChanged(const QString &)), this, SIGNAL(modified()));

  connect(_topLabelText, SIGNAL(inFocus()), this, SLOT(labelSelected()));
  connect(_leftLabelText, SIGNAL(inFocus()), this, SLOT(labelSelected()));
  connect(_bottomLabelText, SIGNAL(inFocus()), this, SLOT(labelSelected()));
  connect(_rightLabelText, SIGNAL(inFocus()), this, SLOT(labelSelected()));

  connect(_strings, SIGNAL(selectionChanged(QString)), this, SLOT(labelUpdate(const QString&)));
  connect(_scalars, SIGNAL(selectionChanged(QString)), this, SLOT(labelUpdate(const QString&)));

  connect(_showLegend, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));

  connect(_applyGlobalsButton, SIGNAL(pressed()), this, SLOT(applyGlobals()));
  connect(_autoLabel, SIGNAL(pressed()), this, SLOT(autoLabel()));

  connect(_editLegendContents, SIGNAL(pressed()), _plotItem->legend(), SLOT(edit()));
}


LabelTab::~LabelTab() {
}


void LabelTab::update() {
}


QString LabelTab::leftLabel() const {
  return _leftLabelText->text();
}


void LabelTab::setLeftLabel(const QString &label) {
  _leftLabelText->setText(label);
}


QString LabelTab::bottomLabel() const {
  return _bottomLabelText->text();
}


void LabelTab::setBottomLabel(const QString &label) {
  _bottomLabelText->setText(label);
}


QString LabelTab::rightLabel() const {
  return _rightLabelText->text();
}


void LabelTab::setRightLabel(const QString &label) {
  _rightLabelText->setText(label);
}


QString LabelTab::topLabel() const {
  return _topLabelText->text();
}


void LabelTab::setTopLabel(const QString &label) {
  _topLabelText->setText(label);
}


void LabelTab::setGlobalFont(const QFont &font) {
  _globalLabelFontFamily->setCurrentFont(font);
  _globalLabelBold->setChecked(font.bold());
  _globalLabelUnderline->setChecked(font.underline());
  _globalLabelItalic->setChecked(font.italic());
}


QFont LabelTab::globalLabelFont() const {
  QFont font(_globalLabelFontFamily->currentFont());
  font.setItalic(_globalLabelItalic->isChecked());
  font.setBold(_globalLabelBold->isChecked());
  font.setUnderline(_globalLabelUnderline->isChecked());
  return font;
}


qreal LabelTab::globalLabelFontScale() const {
  return _globalLabelFontSize->value();
}


QColor LabelTab::globalLabelColor() const {
  return _globalLabelColor->color();
}


void LabelTab::applyGlobals() {
  emit globalFontUpdate();
  emit modified();
}


void LabelTab::autoLabel() {
  setLeftLabel(_plotItem->leftLabel());
  setBottomLabel(_plotItem->bottomLabel());
  setTopLabel(_plotItem->topLabel());
  setRightLabel(_plotItem->rightLabel());
  emit modified();
}


bool LabelTab::showLegend() const {
  return _showLegend->isChecked();
}


void LabelTab::setShowLegend(const bool show) {
  _showLegend->setChecked(show);
}


void LabelTab::labelUpdate(const QString& string) {
  if (_activeLineEdit) {
    QString label = _activeLineEdit->text();
    label += "[" + string + "]";
    _activeLineEdit->setText(label); 
  }
}


void LabelTab::labelSelected() {
  if (_rightLabelText->hasFocus()) {
    _activeLineEdit = _rightLabelText;
  } else if (_bottomLabelText->hasFocus()) {
    _activeLineEdit = _bottomLabelText;
  } else if (_leftLabelText->hasFocus()) {
    _activeLineEdit = _leftLabelText;
  } else {
    _activeLineEdit = _topLabelText;
  }
}
}

// vim: ts=2 sw=2 et
