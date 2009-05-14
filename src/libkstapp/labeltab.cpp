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

  connect(_showLegend, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));

  connect(_editLegendContents, SIGNAL(pressed()), _plotItem->legend(), SLOT(edit()));

  connect(_globalLabelFontSize, SIGNAL(valueChanged(double)), this, SIGNAL(globalFontUpdate()));
  connect(_globalLabelBold, SIGNAL(toggled(bool)), this, SIGNAL(globalFontUpdate()));
  connect(_globalLabelItalic, SIGNAL(toggled(bool)), this, SIGNAL(globalFontUpdate()));
  connect(_globalLabelFontFamily, SIGNAL(currentFontChanged(const QFont &)), this, SIGNAL(globalFontUpdate()));
  connect(_globalLabelColor, SIGNAL(changed(const QColor &)), this, SIGNAL(globalFontUpdate()));

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

bool LabelTab::leftLabelAuto() const {
  return _leftLabelAuto->isChecked();
}

void LabelTab::setLeftLabelAuto(bool a) {
  _leftLabelAuto->setChecked(a);
}

QString LabelTab::bottomLabel() const {
  return _bottomLabelText->text();
}


void LabelTab::setBottomLabel(const QString &label) {
  _bottomLabelText->setText(label);
}

bool LabelTab::bottomLabelAuto() const {
  return _bottomLabelAuto->isChecked();
}

void LabelTab::setBottomLabelAuto(bool a) {
  _bottomLabelAuto->setChecked(a);
}


QString LabelTab::rightLabel() const {
  return _rightLabelText->text();
}


void LabelTab::setRightLabel(const QString &label) {
  _rightLabelText->setText(label);
}

bool LabelTab::rightLabelAuto() const {
  return _rightLabelAuto->isChecked();
}

void LabelTab::setRightLabelAuto(bool a) {
  _rightLabelAuto->setChecked(a);
}


QString LabelTab::topLabel() const {
  return _topLabelText->text();
}


void LabelTab::setTopLabel(const QString &label) {
  _topLabelText->setText(label);
}

bool LabelTab::topLabelAuto() const {
  return _topLabelAuto->isChecked();
}

void LabelTab::setTopLabelAuto(bool a) {
  _topLabelAuto->setChecked(a);
}


void LabelTab::setGlobalFont(const QFont &font) {
  _globalLabelFontFamily->setCurrentFont(font);
  _globalLabelBold->setChecked(font.bold());
  _globalLabelItalic->setChecked(font.italic());
}


QFont LabelTab::globalLabelFont() const {
  QFont font(_globalLabelFontFamily->currentFont());
  font.setItalic(_globalLabelItalic->isChecked());
  font.setBold(_globalLabelBold->isChecked());
  return font;
}


qreal LabelTab::globalLabelFontScale() const {
  return _globalLabelFontSize->value();
}


QColor LabelTab::globalLabelColor() const {
  return _globalLabelColor->color();
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
