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
  : DialogTab(parent), _plotItem(plotItem), _activeLineEdit(0), _fontDirty(false) {

  setupUi(this);

  _globalLabelBold->setIcon(QPixmap(":kst_bold.png"));
  _globalLabelBold->setFixedWidth(32);
  _globalLabelItalic->setIcon(QPixmap(":kst_italic.png"));
  _globalLabelItalic->setFixedWidth(32);
  _globalLabelColor->setFixedWidth(32);
  _globalLabelColor->setFixedHeight(32);

  setTabTitle(tr("Labels"));

  setGlobalFont(_plotItem->globalFont());
  _globalLabelColor->setColor(_plotItem->globalFontColor());
  _globalLabelFontSize->setValue(_plotItem->globalFontScale());

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

  connect(_autoScaleNumberAxis, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
  connect(_showLegend, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));

  connect(_editLegendContents, SIGNAL(pressed()), _plotItem->legend(), SLOT(edit()));

  connect(_globalLabelFontSize, SIGNAL(valueChanged(double)), this, SIGNAL(globalFontUpdate()));
  connect(_globalLabelBold, SIGNAL(toggled(bool)), this, SIGNAL(globalFontUpdate()));
  connect(_globalLabelItalic, SIGNAL(toggled(bool)), this, SIGNAL(globalFontUpdate()));
  connect(_globalLabelFontFamily, SIGNAL(currentFontChanged(const QFont &)), this, SIGNAL(globalFontUpdate()));
  connect(_globalLabelColor, SIGNAL(changed(const QColor &)), this, SIGNAL(globalFontUpdate()));
  connect(_globalLabelBold, SIGNAL(toggled(bool)), this, SLOT(buttonUpdate()));
  connect(_globalLabelItalic, SIGNAL(toggled(bool)), this, SLOT(buttonUpdate()));

  connect(_topLabelAuto, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_leftLabelAuto, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_bottomLabelAuto, SIGNAL(toggled(bool)), this, SIGNAL(modified()));
  connect(_rightLabelAuto, SIGNAL(toggled(bool)), this, SIGNAL(modified()));

  connect(_topLabelAuto, SIGNAL(toggled(bool)), this, SLOT(activateFields()));
  connect(_leftLabelAuto, SIGNAL(toggled(bool)), this, SLOT(activateFields()));
  connect(_bottomLabelAuto, SIGNAL(toggled(bool)), this, SLOT(activateFields()));
  connect(_rightLabelAuto, SIGNAL(toggled(bool)), this, SLOT(activateFields()));
}


LabelTab::~LabelTab() {
}


void LabelTab::update() {
}


void LabelTab::buttonUpdate() {
  _fontDirty = true;
}


void LabelTab::activateFields() {
  _topLabelText->setEnabled(_topLabelAuto->checkState() != Qt::Checked);
  _leftLabelText->setEnabled(_leftLabelAuto->checkState() != Qt::Checked);
  _bottomLabelText->setEnabled(_bottomLabelAuto->checkState() != Qt::Checked);
  _rightLabelText->setEnabled(_rightLabelAuto->checkState() != Qt::Checked);
}


QString LabelTab::leftLabel() const {
  return _leftLabelText->text();
}


bool LabelTab::leftLabelDirty() const {
  return (!_leftLabelText->text().isEmpty());
}


void LabelTab::setLeftLabel(const QString &label) {
  _leftLabelText->setText(label);
}


bool LabelTab::leftLabelAuto() const {
  return _leftLabelAuto->isChecked();
}


bool LabelTab::leftLabelAutoDirty() const {
  return _leftLabelAuto->checkState() != Qt::PartiallyChecked;
}


void LabelTab::setLeftLabelAuto(bool a) {
  _leftLabelAuto->setChecked(a);
}


QString LabelTab::bottomLabel() const {
  return _bottomLabelText->text();
}


bool LabelTab::bottomLabelDirty() const {
  return (!_bottomLabelText->text().isEmpty());
}


void LabelTab::setBottomLabel(const QString &label) {
  _bottomLabelText->setText(label);
}


bool LabelTab::bottomLabelAuto() const {
  return _bottomLabelAuto->isChecked();
}


bool LabelTab::bottomLabelAutoDirty() const {
  return _bottomLabelAuto->checkState() != Qt::PartiallyChecked;
}


void LabelTab::setBottomLabelAuto(bool a) {
  _bottomLabelAuto->setChecked(a);
}


QString LabelTab::rightLabel() const {
  return _rightLabelText->text();
}


bool LabelTab::rightLabelDirty() const {
  return (!_rightLabelText->text().isEmpty());
}


void LabelTab::setRightLabel(const QString &label) {
  _rightLabelText->setText(label);
}


bool LabelTab::rightLabelAuto() const {
  return _rightLabelAuto->isChecked();
}


bool LabelTab::rightLabelAutoDirty() const {
  return _rightLabelAuto->checkState() != Qt::PartiallyChecked;
}


void LabelTab::setRightLabelAuto(bool a) {
  _rightLabelAuto->setChecked(a);
}


QString LabelTab::topLabel() const {
  return _topLabelText->text();
}


bool LabelTab::topLabelDirty() const {
  return (!_topLabelText->text().isEmpty());
}



void LabelTab::setTopLabel(const QString &label) {
  _topLabelText->setText(label);
}


bool LabelTab::topLabelAuto() const {
  return _topLabelAuto->isChecked();
}


bool LabelTab::topLabelAutoDirty() const {
  return _topLabelAuto->checkState() != Qt::PartiallyChecked;
}


void LabelTab::setTopLabelAuto(bool a) {
  _topLabelAuto->setChecked(a);
}


void LabelTab::setGlobalFont(const QFont &font) {
  _fontDirty = false;
  _globalLabelFontFamily->setCurrentFont(font);
  _globalLabelBold->setChecked(font.bold());
  _globalLabelItalic->setChecked(font.italic());
}


QFont LabelTab::globalLabelFont(const QFont ref_font) const {
  QString family = (_globalLabelFontFamily->currentIndex() == -1) ?
                   ref_font.family() : _globalLabelFontFamily->currentFont().family();
  QFont font(family);
  font.setItalic(_globalLabelItalic->isChecked());
  font.setBold(_globalLabelBold->isChecked());
  return font;
}


bool LabelTab::globalLabelFontDirty() const {
  return (_globalLabelFontFamily->currentIndex() != -1 || _fontDirty);
}


qreal LabelTab::globalLabelFontScale() const {
  return _globalLabelFontSize->value();
}


bool LabelTab::globalLabelFontScaleDirty() const {
  return (!_globalLabelFontSize->text().isEmpty());
}


QColor LabelTab::globalLabelColor() const {
  return _globalLabelColor->color();
}


bool LabelTab::globalLabelColorDirty() const {
  return _globalLabelColor->colorDirty();
}


bool LabelTab::showLegend() const {
  return _showLegend->isChecked();
}


bool LabelTab::showLegendDirty() const {
  return _showLegend->checkState() != Qt::PartiallyChecked;
}


void LabelTab::setShowLegend(const bool show) {
  _showLegend->setChecked(show);
}


bool LabelTab::autoScaleNumbers() const {
  return _autoScaleNumberAxis->isChecked();
}


bool LabelTab::autoScaleNumbersDirty() const {
  return _autoScaleNumberAxis->checkState() != Qt::PartiallyChecked;
}


void LabelTab::setAutoScaleNumbers(const bool scale) {
  _autoScaleNumberAxis->setChecked(scale);
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


void LabelTab::clearTabValues() {
  _showLegend->setCheckState(Qt::PartiallyChecked);
  _globalLabelFontSize->clear();
  _globalLabelFontFamily->setCurrentIndex(-1);

  _globalLabelBold->setChecked(false);
  _globalLabelItalic->setChecked(false);
  _autoScaleNumberAxis->setCheckState(Qt::PartiallyChecked);

  _topLabelText->clear();
  _bottomLabelText->clear();
  _leftLabelText->clear();
  _rightLabelText->clear();

  _globalLabelColor->clearSelection();

  _topLabelAuto->setCheckState(Qt::PartiallyChecked);
  _bottomLabelAuto->setCheckState(Qt::PartiallyChecked);
  _leftLabelAuto->setCheckState(Qt::PartiallyChecked);
  _rightLabelAuto->setCheckState(Qt::PartiallyChecked);
   activateFields();
}


void LabelTab::enableSingleEditOptions(bool enabled) {
  _editLegendContents->setEnabled(enabled);
  if (enabled) {
    setGlobalFont(_plotItem->globalFont());
    _showLegend->setTristate(false);
    _topLabelAuto->setTristate(false);
    _bottomLabelAuto->setTristate(false);
    _leftLabelAuto->setTristate(false);
    _rightLabelAuto->setTristate(false);
    _autoScaleNumberAxis->setTristate(false);
    activateFields();
  }
}

}

// vim: ts=2 sw=2 et
