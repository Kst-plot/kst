//
// C++ Interface: dimensionstab
//
// Description: 
//
//
// Author: Barth Netterfield <netterfield@physics.utoronto.ca>, (C) 2008

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QDebug>
#include "dimensionstab.h"

namespace Kst {
DimensionsTab::DimensionsTab(ViewItem* viewItem, QWidget *parent) 
    : DialogTab(parent), _viewItem(viewItem) {
  setupUi(this);
  setTabTitle(tr("Dimensions"));

  connect(_fixAspectRatio, SIGNAL(toggled(bool)), this, SLOT(updateButtons()));
}


DimensionsTab::~DimensionsTab() {
}


void DimensionsTab::setupDimensions() {
  _x->setValue(_viewItem->relativeCenter().x());
  _y->setValue(_viewItem->relativeCenter().y());
  _width->setValue(_viewItem->relativeWidth());
  _height->setValue(_viewItem->relativeHeight());
  _rotation->setValue(_viewItem->rotationAngle());

  _fixAspectRatio->setChecked(_viewItem->lockAspectRatio());
  _fixAspectRatio->setEnabled(!_viewItem->lockAspectRatioFixed());
  _fixAspectRatio->setTristate(false);
  _height->setHidden(_viewItem->lockAspectRatio());
  _dimXlabel->setHidden(_viewItem->lockAspectRatio());
  if (_viewItem->fixedSize()) {
    _height->setHidden(true);
    _dimXlabel->setHidden(true);
    _width->setEnabled(false);
    _fixAspectRatio->setHidden(true);
  }
}


void DimensionsTab::updateButtons() {
  _height->setHidden(_fixAspectRatio->checkState() == Qt::Checked);
  _dimXlabel->setHidden(_fixAspectRatio->checkState() == Qt::Checked);
}


void DimensionsTab::modified() {
  emit tabModified();
}


void DimensionsTab::clearTabValues() {
  _fixAspectRatio->setCheckState(Qt::PartiallyChecked);
  _width->clear();
  _height->clear();
  _rotation->clear();
}


void DimensionsTab::enableSingleEditOptions(bool enabled) {
  _x->setEnabled(enabled);
  _y->setEnabled(enabled);
}


bool DimensionsTab::widthDirty() const {
  return (!_width->text().isEmpty());
}


bool DimensionsTab::heightDirty() const {
  return (!_height->text().isEmpty());
}


bool DimensionsTab::rotationDirty() const {
  return (!_rotation->text().isEmpty());
}


bool DimensionsTab::fixedAspectDirty() const {
  return _fixAspectRatio->checkState() != Qt::PartiallyChecked;
}


}
