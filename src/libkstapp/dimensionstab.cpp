/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007  Barth Netterfield                               *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "dimensionstab.h"
#include "plotitem.h"
#include <QDebug>

namespace Kst {
DimensionsTab::DimensionsTab(ViewItem* viewItem, QWidget *parent) 
    : DialogTab(parent), _viewItem(viewItem) {
  setupUi(this);
  setTabTitle(tr("Size/Position"));

  connect(_fixAspectRatio, SIGNAL(toggled(bool)), this, SLOT(updateButtons()));
  connect(_lockPosToData, SIGNAL(clicked(bool)), this, SLOT(fillDimensions(bool)));

  label_5->setProperty("si","&Rotation:");
  label_3->setProperty("si","geo X");
  _dimXlabel->setProperty("si","geo Y");
  label->setProperty("si","pos X");
  label_2->setProperty("si","pos Y");
  _fixAspectRatio->setProperty("si","&Fix aspect ratio");
  _lockPosToData->setProperty("si", "Lock Position To Data");
}


DimensionsTab::~DimensionsTab() {
}


void DimensionsTab::fillDimensions(bool lock_pos_to_data) {
  if (lock_pos_to_data) {
    _x->setRange(-1E308, 1E308);
    _y->setRange(-1E308, 1E308);
    _width->setRange(0,1E308);
    _height->setRange(0,1E308);
    _x->setValue(_viewItem->dataRelativeRect().center().x());
    _y->setValue(_viewItem->dataRelativeRect().center().y());
    _width->setValue(_viewItem->dataRelativeRect().width());
    _height->setValue(_viewItem->dataRelativeRect().height());
  } else {
    _x->setRange(0, 1);
    _y->setRange(0, 1);
    _width->setRange(0,1);
    _height->setRange(0,1);
    _x->setValue(_viewItem->relativeCenter().x());
    _y->setValue(_viewItem->relativeCenter().y());
    _width->setValue(_viewItem->relativeWidth());
    _height->setValue(_viewItem->relativeHeight());
  }
}

void DimensionsTab::setupDimensions() {

  fillDimensions(_viewItem->dataPosLockable() && _viewItem->lockPosToData());

  double theta = _viewItem->rotationAngle();

  _rotation->setValue(theta);

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
  _lockPosToData->setChecked(_viewItem->lockPosToData());
  if (_viewItem->dataPosLockable()) {
      _lockPosToData->show();
  } else {
      _lockPosToData->hide();
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
  _lockPosToData->setCheckState(Qt::PartiallyChecked);
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


bool DimensionsTab::lockPosToDataDirty() const {
  return _lockPosToData->checkState() != Qt::PartiallyChecked;
}

}
