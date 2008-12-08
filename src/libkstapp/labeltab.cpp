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

namespace Kst {

LabelTab::LabelTab(PlotItem* plotItem, QWidget *parent)
  : DialogTab(parent), _plotItem(plotItem) {

  setupUi(this);
  setTabTitle(tr("Labels"));

  QFont font;
  setGlobalFont(font);

  _topLabel->setObjectStore(kstApp->mainWindow()->document()->objectStore());
  _leftLabel->setObjectStore(kstApp->mainWindow()->document()->objectStore());
  _bottomLabel->setObjectStore(kstApp->mainWindow()->document()->objectStore());
  _rightLabel->setObjectStore(kstApp->mainWindow()->document()->objectStore());

  connect(_topLabel, SIGNAL(labelChanged(const QString&)), this, SIGNAL(modified()));
  connect(_leftLabel, SIGNAL(labelChanged(const QString&)), this, SIGNAL(modified()));
  connect(_bottomLabel, SIGNAL(labelChanged(const QString&)), this, SIGNAL(modified()));
  connect(_rightLabel, SIGNAL(labelChanged(const QString&)), this, SIGNAL(modified()));

  connect(_topFontSize, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_leftFontSize, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_bottomFontSize, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_rightFontSize, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_numberFontSize, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));

  connect(_topBold, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_leftBold, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_bottomBold, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_rightBold, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_numberBold, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));

  connect(_topUnderline, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_leftUnderline, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_bottomUnderline, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_rightUnderline, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_numberUnderline, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));

  connect(_topItalic, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_leftItalic, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_bottomItalic, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_rightItalic, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_numberItalic, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));

  connect(_topFamily, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_leftFamily, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_bottomFamily, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_rightFamily, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));
  connect(_numberFamily, SIGNAL(currentIndexChanged(int)), this, SIGNAL(modified()));

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
  return _leftLabel->labelText();
}


void LabelTab::setLeftLabel(const QString &label) {
  _leftLabel->setLabelText(label);
}


QString LabelTab::bottomLabel() const {
  return _bottomLabel->labelText();
}


void LabelTab::setBottomLabel(const QString &label) {
  _bottomLabel->setLabelText(label);
}


QString LabelTab::rightLabel() const {
  return _rightLabel->labelText();
}


void LabelTab::setRightLabel(const QString &label) {
  _rightLabel->setLabelText(label);
}


QString LabelTab::topLabel() const {
  return _topLabel->labelText();
}


void LabelTab::setTopLabel(const QString &label) {
  _topLabel->setLabelText(label);
}


void LabelTab::setGlobalFont(const QFont &font) {
  _globalLabelFontFamily->setCurrentFont(font);
  _globalLabelBold->setChecked(font.bold());
  _globalLabelUnderline->setChecked(font.underline());
  _globalLabelItalic->setChecked(font.italic());
}


QFont LabelTab::leftLabelFont() const {
  QFont font(_leftFamily->currentFont());
  font.setItalic(_leftItalic->isChecked());
  font.setBold(_leftBold->isChecked());
  font.setUnderline(_leftUnderline->isChecked());
  return font;
}


void LabelTab::setLeftLabelFont(const QFont &font) {
  _leftFamily->setCurrentFont(font);
  _leftBold->setChecked(font.bold());
  _leftUnderline->setChecked(font.underline());
  _leftItalic->setChecked(font.italic());
}


QFont LabelTab::rightLabelFont() const {
  QFont font(_rightFamily->currentFont());
  font.setItalic(_rightItalic->isChecked());
  font.setBold(_rightBold->isChecked());
  font.setUnderline(_rightUnderline->isChecked());
  return font;
}


void LabelTab::setRightLabelFont(const QFont &font) {
  _rightFamily->setCurrentFont(font);
  _rightBold->setChecked(font.bold());
  _rightUnderline->setChecked(font.underline());
  _rightItalic->setChecked(font.italic());
}


QFont LabelTab::topLabelFont() const {
  QFont font(_topFamily->currentFont());
  font.setItalic(_topItalic->isChecked());
  font.setBold(_topBold->isChecked());
  font.setUnderline(_topUnderline->isChecked());
  return font;
}


void LabelTab::setTopLabelFont(const QFont &font) {
  _topFamily->setCurrentFont(font);
  _topBold->setChecked(font.bold());
  _topUnderline->setChecked(font.underline());
  _topItalic->setChecked(font.italic());
}


QFont LabelTab::bottomLabelFont() const {
  QFont font(_bottomFamily->currentFont());
  font.setItalic(_bottomItalic->isChecked());
  font.setBold(_bottomBold->isChecked());
  font.setUnderline(_bottomUnderline->isChecked());
  return font;
}


void LabelTab::setBottomLabelFont(const QFont &font) {
  _bottomFamily->setCurrentFont(font);
  _bottomBold->setChecked(font.bold());
  _bottomUnderline->setChecked(font.underline());
  _bottomItalic->setChecked(font.italic());
}


QFont LabelTab::numberLabelFont() const {
  QFont font(_numberFamily->currentFont());
  font.setItalic(_numberItalic->isChecked());
  font.setBold(_numberBold->isChecked());
  font.setUnderline(_numberUnderline->isChecked());
  return font;
}


void LabelTab::setNumberLabelFont(const QFont &font) {
  _numberFamily->setCurrentFont(font);
  _numberBold->setChecked(font.bold());
  _numberUnderline->setChecked(font.underline());
  _numberItalic->setChecked(font.italic());
}


void LabelTab::applyGlobals() {
  QFont font(_globalLabelFontFamily->currentFont());
  font.setItalic(_globalLabelItalic->isChecked());
  font.setBold(_globalLabelBold->isChecked());
  font.setUnderline(_globalLabelUnderline->isChecked());

  setLeftLabelFont(font);
  setRightLabelFont(font);
  setTopLabelFont(font);
  setBottomLabelFont(font);
  setNumberLabelFont(font);

  setLeftLabelFontScale(_globalLabelFontSize->value());
  setRightLabelFontScale(_globalLabelFontSize->value());
  setTopLabelFontScale(_globalLabelFontSize->value());
  setBottomLabelFontScale(_globalLabelFontSize->value());
  setNumberLabelFontScale(_globalLabelFontSize->value());

}


void LabelTab::autoLabel() {
  setLeftLabel(_plotItem->leftLabel());
  setBottomLabel(_plotItem->bottomLabel());
  setTopLabel(_plotItem->topLabel());
  setRightLabel(_plotItem->rightLabel());
  emit modified();
}


qreal LabelTab::rightLabelFontScale() const {
  return _rightFontSize->value();
}


void LabelTab::setRightLabelFontScale(const qreal scale) {
  _rightFontSize->setValue(scale);
}


qreal LabelTab::leftLabelFontScale() const {
  return _leftFontSize->value();
}


void LabelTab::setLeftLabelFontScale(const qreal scale) {
  _leftFontSize->setValue(scale);
}


qreal LabelTab::topLabelFontScale() const {
  return _topFontSize->value();
}


void LabelTab::setTopLabelFontScale(const qreal scale) {
  _topFontSize->setValue(scale);
}


qreal LabelTab::bottomLabelFontScale() const {
  return _bottomFontSize->value();
}


void LabelTab::setBottomLabelFontScale(const qreal scale) {
  _bottomFontSize->setValue(scale);
}


qreal LabelTab::numberLabelFontScale() const {
  return _numberFontSize->value();
}


void LabelTab::setNumberLabelFontScale(const qreal scale) {
  _numberFontSize->setValue(scale);
}


bool LabelTab::showLegend() const {
  return _showLegend->isChecked();
}


void LabelTab::setShowLegend(const bool show) {
  _showLegend->setChecked(show);
}

}

// vim: ts=2 sw=2 et
