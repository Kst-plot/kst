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
  connect(_lockAspectRatio, SIGNAL(stateChanged(int)), this, SIGNAL(modified()));
}


LayoutTab::~LayoutTab() {
}


void LayoutTab::setLayoutEnabled(bool layoutEnabled) {
  _horMargin->setEnabled(layoutEnabled);
  _verMargin->setEnabled(layoutEnabled);
  _horSpacing->setEnabled(layoutEnabled);
  _verSpacing->setEnabled(layoutEnabled);
}


double LayoutTab::horizontalMargin() const {
  return _horMargin->value();
}


void LayoutTab::setHorizontalMargin(double horizontalMargin) {
  _horMargin->setValue(horizontalMargin);
}


double LayoutTab::verticalMargin() const {
  return _verMargin->value();
}


void LayoutTab::setVerticalMargin(double verticalMargin) {
  _verMargin->setValue(verticalMargin);
}


double LayoutTab::horizontalSpacing() const {
  return _horSpacing->value();
}


void LayoutTab::setHorizontalSpacing(double horizontalSpacing) {
  _horSpacing->setValue(horizontalSpacing);
}


double LayoutTab::verticalSpacing() const {
  return _verSpacing->value();
}


void LayoutTab::setVerticalSpacing(double verticalSpacing) {
  _verSpacing->setValue(verticalSpacing);
}


bool LayoutTab::lockAspectRatio() const {
  return _lockAspectRatio->isChecked();
}


void LayoutTab::setLockAspectRatio(const bool lock) {
  _lockAspectRatio->setChecked(lock);
}


void LayoutTab::enableLockAspectRatio(const bool enable) {
  _lockAspectRatio->setEnabled(enable);
}

}

// vim: ts=2 sw=2 et
