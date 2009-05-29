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

#include "layouttab.h"

namespace Kst {

LayoutTab::LayoutTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Layout"));

  connect(_horMargin, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_verMargin, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_horSpacing, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
  connect(_verSpacing, SIGNAL(valueChanged(double)), this, SIGNAL(modified()));
}


LayoutTab::~LayoutTab() {
}


double LayoutTab::horizontalMargin() const {
  return _horMargin->value();
}


bool LayoutTab::horizontalMarginDirty() const {
  return (!_horMargin->text().isEmpty());
}


void LayoutTab::setHorizontalMargin(double horizontalMargin) {
  _horMargin->setValue(horizontalMargin);
}


double LayoutTab::verticalMargin() const {
  return _verMargin->value();
}


bool LayoutTab::verticalMarginDirty() const {
  return (!_verMargin->text().isEmpty());
}


void LayoutTab::setVerticalMargin(double verticalMargin) {
  _verMargin->setValue(verticalMargin);
}


double LayoutTab::horizontalSpacing() const {
  return _horSpacing->value();
}


bool LayoutTab::horizontalSpacingDirty() const {
  return (!_horSpacing->text().isEmpty());
}


void LayoutTab::setHorizontalSpacing(double horizontalSpacing) {
  _horSpacing->setValue(horizontalSpacing);
}


double LayoutTab::verticalSpacing() const {
  return _verSpacing->value();
}


bool LayoutTab::verticalSpacingDirty() const {
  return (!_verSpacing->text().isEmpty());
}


void LayoutTab::setVerticalSpacing(double verticalSpacing) {
  _verSpacing->setValue(verticalSpacing);
}


void LayoutTab::clearTabValues() {
  _horMargin->clear();
  _verMargin->clear();
  _horSpacing->clear();
  _verSpacing->clear();
}

}

// vim: ts=2 sw=2 et
