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
  circleDimensionsPage->setPageTitle(tr("Position/Size"));
  circleDimensionsPage->addDialogTab(_circleDimensionsTab);
  addDialogPage(circleDimensionsPage);
  selectDialogPage(circleDimensionsPage);

  connect(_circleDimensionsTab, SIGNAL(apply()), this, SLOT(dimensionsChanged()));
  setupProperties();

  connect(_circleDimensionsTab, SIGNAL(tabModified()), this, SLOT(modified()));

}

void CircleItemDialog::setupProperties() {
  _circleDimensionsTab->enableSingleEditOptions(true);
  _circleDimensionsTab->setupDimensions();
}

void CircleItemDialog::saveDimensions(ViewItem *item) {
  Q_ASSERT(item);
  qreal parentWidth;
  qreal parentHeight;
  qreal parentX;
  qreal parentY;

  if (item->parentViewItem()) {
    parentWidth = item->parentViewItem()->width();
    parentHeight = item->parentViewItem()->height();
    parentX = item->parentViewItem()->rect().x();
    parentY = item->parentViewItem()->rect().y();
  } else if (item->view()) {
    parentWidth = item->view()->width();
    parentHeight = item->view()->height();
    parentX = item->view()->rect().x();
    parentY = item->view()->rect().y();
  } else {
    Q_ASSERT_X(false,"parent test", "item has no parentview item");
    parentWidth = parentHeight = 1.0;
    parentX = parentY = 0.0;
  }

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
