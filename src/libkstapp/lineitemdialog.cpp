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

#include "lineitemdialog.h"

#include "linedimensionstab.h"
#include "lineitem.h"
#include "dialogpage.h"
#include "application.h"
#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"

namespace Kst {

LineItemDialog::LineItemDialog(LineItem *item, QWidget *parent)
  : ViewItemDialog(item, parent), _viewItem(item) {
  _lineDimensionsTab = new LineDimensionsTab(item, this);

  DialogPage *lineDimensionsPage = new DialogPage(this);
  lineDimensionsPage->setPageTitle(tr("Size/Position"));
  lineDimensionsPage->addDialogTab(_lineDimensionsTab);
  addDialogPage(lineDimensionsPage);
  selectDialogPage(lineDimensionsPage);

  connect(_lineDimensionsTab, SIGNAL(apply()), this, SLOT(dimensionsChanged()));

  setupDimensions();

  connect(_lineDimensionsTab, SIGNAL(tabModified()), this, SLOT(modified()));

}

void LineItemDialog::setupDimensions() {
  _lineDimensionsTab->enableSingleEditOptions(true);
  _lineDimensionsTab->setupDimensions();
}

void LineItemDialog::saveDimensions(ViewItem *item) {
  Q_ASSERT(item);

  qreal x1 = _lineDimensionsTab->x1();
  qreal y1 = _lineDimensionsTab->y1();
  qreal x2 = _lineDimensionsTab->x2();
  qreal y2 = _lineDimensionsTab->y2();

  if (_lineDimensionsTab->lockPosToData() && item->dataPosLockable()) {
    QRectF dr;
    dr.setTopLeft(QPointF(x1,y1));
    dr.setBottomRight(QPointF(x2,y2));

    item->setDataRelativeRect(dr);

    bool lockPosToData = _lineDimensionsTab->lockPosToDataDirty() ? _lineDimensionsTab->lockPosToData() : item->lockPosToData();
    item->setLockPosToData(lockPosToData);

    item->applyDataLockedDimensions();
  } else {
    QRectF parentRect = item->parentRect();
    qreal parentWidth = parentRect.width();
    qreal parentHeight = parentRect.height();
    qreal parentX = parentRect.x();
    qreal parentY = parentRect.y();

    x1 = x1*parentWidth + parentX;
    x2 = x2*parentWidth + parentX;
    y1 = y1*parentHeight + parentY;
    y2 = y2*parentHeight + parentY;
    qreal dx = x2-x1;
    qreal dy = y2-y1;

    qreal w = sqrt(dx*dx + dy*dy);
    qreal h = item->height();

    item->setPos(x1,y1);
    item->setViewRect(0,-h/2,w,h);

    qreal rotation = atan2(dy,dx);

    QTransform transform;
    transform.rotateRadians(rotation);

    item->setTransform(transform);
    item->updateRelativeSize();
  }
}
}
