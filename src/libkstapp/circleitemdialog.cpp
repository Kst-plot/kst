/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2008 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "circleitemdialog.h"

#include "circledimensionstab.h"
#include "circleitem.h"
#include "dialogpage.h"
#include "application.h"
#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"

namespace Kst {

CircleItemDialog::CircleItemDialog(CircleItem *item, QWidget *parent)
  : ViewItemDialog(item, parent), _viewItem(item) {

  _circleDimensionsTab = new CircleDimensionsTab(item, this);

  DialogPage *circleDimensionsPage = new DialogPage(this);
  circleDimensionsPage->setPageTitle(tr("Size/Position"));
  circleDimensionsPage->addDialogTab(_circleDimensionsTab);
  addDialogPage(circleDimensionsPage);
  selectDialogPage(circleDimensionsPage);

  connect(_circleDimensionsTab, SIGNAL(apply()), this, SLOT(dimensionsChanged()));

  setupDimensions();

  connect(_circleDimensionsTab, SIGNAL(tabModified()), this, SLOT(modified()));

}


void CircleItemDialog::setupDimensions() {
  _circleDimensionsTab->enableSingleEditOptions(true);
  _circleDimensionsTab->setupDimensions();
}

void CircleItemDialog::saveDimensions(ViewItem *item) {
  Q_ASSERT(item);

  if (_circleDimensionsTab->lockPosToData() && item->dataPosLockable()) {
    QRectF dr;
    dr.setWidth(2*_circleDimensionsTab->radius());
    dr.setHeight(2*_circleDimensionsTab->radius());
    dr.moveCenter(QPointF(_circleDimensionsTab->x(), _circleDimensionsTab->y()));

    item->setDataRelativeRect(dr);
    bool lockPosToData = _circleDimensionsTab->lockPosToDataDirty() ? _circleDimensionsTab->lockPosToData() : item->lockPosToData();
    item->setLockPosToData(lockPosToData);

    item->applyDataLockedDimensions();
  } else {

    QRectF parentRect = item->parentRect();
    qreal parentWidth = parentRect.width();
    qreal parentHeight = parentRect.height();
    qreal parentX = parentRect.x();
    qreal parentY = parentRect.y();

    qreal relativeRadius = _circleDimensionsTab->radiusDirty() ? _circleDimensionsTab->radius() :item->relativeWidth()*0.5;
    bool lockPosToData = _circleDimensionsTab->lockPosToDataDirty() ? _circleDimensionsTab->lockPosToData() : item->lockPosToData();

    qreal radius = relativeRadius * parentWidth;
    item->setLockPosToData(lockPosToData);

    if (editMode() == Multiple) {
      item->setPos(parentX + item->relativeCenter().x()*parentWidth,
                   parentY + item->relativeCenter().y()*parentHeight);
    } else {
      item->setPos(parentX + _circleDimensionsTab->x()*parentWidth, parentY + _circleDimensionsTab->y()*parentHeight);
    }
    item->setViewRect(-radius, -radius, radius*2.0, radius*2.0);

    QTransform transform;

    item->setTransform(transform);
    item->updateRelativeSize();
  }
}
}
