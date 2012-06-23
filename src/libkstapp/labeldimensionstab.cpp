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
#include "labeldimensionstab.h"
#include "plotitem.h"
#include <QDebug>

namespace Kst {

LabelDimensionsTab::LabelDimensionsTab(LabelItem* labelItem, QWidget *parent)
  : DialogTab(parent), _labelItem(labelItem) {
  setupUi(this);

  connect(_lockPosToData, SIGNAL(clicked(bool)), this, SLOT(lockPosToDataChanged(bool)));
  connect(_left, SIGNAL(toggled(bool)), this, SLOT(fixLeftChanged(bool)));
}


void LabelDimensionsTab::fixLeftChanged(bool fix_left) {
  fillDimensions(lockPosToData(), fix_left);
}


void LabelDimensionsTab::lockPosToDataChanged(bool lock_pos_to_data) {
  fillDimensions(lock_pos_to_data, fixLeft());
}

void LabelDimensionsTab::fillDimensions(bool lock_pos_to_data, bool fix_left) {

  if (lock_pos_to_data && _labelItem->dataPosLockable()) {
    PlotRenderItem *render_item = dynamic_cast<PlotRenderItem *>(_labelItem->parentViewItem());
    if (render_item) {
      qreal parentWidth = render_item->width();
      qreal parentHeight = render_item->height();
      qreal parentX = render_item->rect().x();
      qreal parentY = render_item->rect().y();
      qreal parentDX = render_item->plotItem()->xMax() - render_item->plotItem()->xMin();
      qreal parentDY = render_item->plotItem()->yMax() - render_item->plotItem()->yMin();

      _x->setRange(-1E308, 1E308);
      _y->setRange(-1E308, 1E308);

      QPointF drP1 = _labelItem->dataRelativeRect().topLeft();
      QPointF drP2 = _labelItem->dataRelativeRect().bottomRight();

      QPointF P1(parentX + parentWidth*(drP1.x()-render_item->plotItem()->xMin())/parentDX,
                 parentY + parentHeight*(render_item->plotItem()->yMax() - drP1.y())/parentDY);
      QPointF P2(parentX + parentWidth*(drP2.x()-render_item->plotItem()->xMin())/parentDX,
                 parentY + parentHeight*(render_item->plotItem()->yMax() - drP2.y())/parentDY);

      qreal theta = atan2(P2.y() - P1.y(), P2.x() - P1.x());

      QPointF P;
      if (fix_left) {
        P = drP1;
        _left->setChecked(true);
      } else {
        P = drP2;
        _right->setChecked(true);
      }
      _x->setValue(P.x());
      _y->setValue(P.y());
      _rotation->setValue(theta*180.0/M_PI);
    }
  } else {
    _x->setRange(0, 1);
    _y->setRange(0, 1);
    QRectF pr = _labelItem->parentRect();
    QPointF P1 = _labelItem->mapToParent(_labelItem->rect().bottomLeft()) - pr.topLeft();
    QPointF P2 = _labelItem->mapToParent(_labelItem->rect().bottomRight()) - pr.topLeft();
    qreal theta = atan2(P2.y() - P1.y(), P2.x() - P1.x());
    if (fix_left) {
      _x->setValue(P1.x()/pr.width());
      _y->setValue(P1.y()/pr.height());
      _left->setChecked(true);
    } else {
      _x->setValue(P2.x()/pr.width());
      _y->setValue(P2.y()/pr.height());
      _right->setChecked(true);
    }
    _rotation->setValue(theta*180.0/M_PI);
  }
}


void LabelDimensionsTab::setupDimensions() {

  fillDimensions(_labelItem->dataPosLockable() && _labelItem->lockPosToData(), _labelItem->fixLeft());

  _lockPosToData->setChecked(_labelItem->lockPosToData());
  if (_labelItem->dataPosLockable()) {
      _lockPosToData->show();
  } else {
      _lockPosToData->hide();
  }
}


void LabelDimensionsTab::modified() {
  emit tabModified();
}


void LabelDimensionsTab::clearTabValues() {
  _lockPosToData->setCheckState(Qt::PartiallyChecked);
}


void LabelDimensionsTab::enableSingleEditOptions(bool enabled) {
  //_x->setEnabled(enabled);
  //_y->setEnabled(enabled);
}


bool LabelDimensionsTab::lockPosToDataDirty() const {
  return _lockPosToData->checkState() != Qt::PartiallyChecked;
}

}
