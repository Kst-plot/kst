/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2012  Barth Netterfield                               *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "circledimensionstab.h"
#include "plotitem.h"
#include <QDebug>

namespace Kst {
CircleDimensionsTab::CircleDimensionsTab(ViewItem* viewItem, QWidget *parent)
  : DialogTab(parent), _viewItem(viewItem) {
  setupUi(this);
}

void CircleDimensionsTab::setupDimensions() {
  double x0 = _viewItem->relativeCenter().x();
  double y0 = _viewItem->relativeCenter().y();
  double r = _viewItem->relativeWidth()*0.5;

  _x->setValue(x0);
  _y->setValue(y0);
  _radius->setValue(r);

  _lockPosToData->setChecked(_viewItem->lockPosToData());
  if (_viewItem->dataPosLockable()) {
      _lockPosToData->show();
  } else {
      _lockPosToData->hide();
  }
}


void CircleDimensionsTab::modified() {
  emit tabModified();
}


void CircleDimensionsTab::clearTabValues() {
  _radius->clear();
  _lockPosToData->setCheckState(Qt::PartiallyChecked);
}


void CircleDimensionsTab::enableSingleEditOptions(bool enabled) {
  _x->setEnabled(enabled);
  _y->setEnabled(enabled);
}


bool CircleDimensionsTab::radiusDirty() const {
  return (!_radius->text().isEmpty());
}


bool CircleDimensionsTab::lockPosToDataDirty() const {
  return _lockPosToData->checkState() != Qt::PartiallyChecked;
}

}
