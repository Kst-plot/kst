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
#include "linedimensionstab.h"
#include "plotitem.h"
#include <QDebug>

namespace Kst {

LineDimensionsTab::LineDimensionsTab(ViewItem* viewItem, QWidget *parent)
  : DialogTab(parent), _viewItem(viewItem) {
  setupUi(this);

  connect(_lockPosToData, SIGNAL(clicked(bool)), this, SLOT(fillDimensions(bool)));
}

void LineDimensionsTab::fillDimensions(bool lock_pos_to_data) {
  if (lock_pos_to_data) {
    _p1X->setRange(-1E308, 1E308);
    _p2X->setRange(-1E308, 1E308);
    _p1Y->setRange(-1E308, 1E308);
    _p2Y->setRange(-1E308, 1E308);
    QPointF P1 = _viewItem->dataRelativeRect().topLeft();
    QPointF P2 = _viewItem->dataRelativeRect().bottomRight();
    _p1X->setValue(P1.x());
    _p1Y->setValue(P1.y());
    _p2X->setValue(P2.x());
    _p2Y->setValue(P2.y());
  } else {
    _p1X->setRange(0, 1);
    _p1Y->setRange(0, 1);
    _p2X->setRange(0, 1);
    _p2Y->setRange(0, 1);
    QRectF pr = _viewItem->parentRect();
    QPointF P1 = _viewItem->mapToParent(QPoint(_viewItem->rect().left(), _viewItem->rect().center().y()));
    QPointF P2 = _viewItem->mapToParent(QPoint(_viewItem->rect().right(), _viewItem->rect().center().y()));
    //qDebug() << pr << P1 << P2;
    _p1X->setValue((P1.x()-pr.left())/pr.width());
    _p2X->setValue((P2.x()-pr.left())/pr.width());
    _p1Y->setValue((P1.y()-pr.top())/pr.height());
    _p2Y->setValue((P2.y()-pr.top())/pr.height());
  }
}

void LineDimensionsTab::setupDimensions() {

  fillDimensions(_viewItem->dataPosLockable() && _viewItem->lockPosToData());

  _lockPosToData->setChecked(_viewItem->lockPosToData());
  if (_viewItem->dataPosLockable()) {
      _lockPosToData->show();
  } else {
      _lockPosToData->hide();
  }
}


void LineDimensionsTab::modified() {
  emit tabModified();
}


void LineDimensionsTab::clearTabValues() {
  _lockPosToData->setCheckState(Qt::PartiallyChecked);
}


void LineDimensionsTab::enableSingleEditOptions(bool enabled) {
  //_x->setEnabled(enabled);
  //_y->setEnabled(enabled);
}


bool LineDimensionsTab::lockPosToDataDirty() const {
  return _lockPosToData->checkState() != Qt::PartiallyChecked;
}

}
