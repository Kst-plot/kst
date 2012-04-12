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

#include "labelitemdialog.h"

#include "labeldimensionstab.h"

#include "labelitem.h"
#include "labelpropertiestab.h"
#include "dialogpage.h"
#include "application.h"
#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"

namespace Kst {

LabelItemDialog::LabelItemDialog(LabelItem *item, QWidget *parent)
    : ViewItemDialog(item, parent), _labelItem(item) {

  _propertiesTab = new LabelPropertiesTab(this);
  DialogPage *propertiesPage = new DialogPage(this);
  propertiesPage->setPageTitle(tr("Properties"));
  propertiesPage->addDialogTab(_propertiesTab);
  addDialogPage(propertiesPage);
  selectDialogPage(propertiesPage);
  connect(_propertiesTab, SIGNAL(apply()), this, SLOT(propertiesChanged()));

  setupProperties();
  _saveAsDefault->show();

  _labelDimensionsTab = new LabelDimensionsTab(item, this);

  DialogPage *labelDimensionsPage = new DialogPage(this);
  labelDimensionsPage->setPageTitle(tr("Size/Position"));
  labelDimensionsPage->addDialogTab(_labelDimensionsTab);
  addDialogPage(labelDimensionsPage);
  selectDialogPage(labelDimensionsPage);

  connect(_labelDimensionsTab, SIGNAL(apply()), this, SLOT(dimensionsChanged()));

  setupDimensions();

  connect(_labelDimensionsTab, SIGNAL(tabModified()), this, SLOT(modified()));

}


LabelItemDialog::~LabelItemDialog() {
}


void LabelItemDialog::setupProperties() {
  _propertiesTab->setLabelText(_labelItem->labelText());
  _propertiesTab->setLabelScale(_labelItem->labelScale());
  _propertiesTab->setLabelColor(_labelItem->labelColor());
  _propertiesTab->setLabelFont(_labelItem->labelFont());
}


void LabelItemDialog::propertiesChanged() {
  _labelItem->setLabelText(_propertiesTab->labelText());
  _labelItem->setLabelScale(_propertiesTab->labelScale());
  _labelItem->setLabelColor(_propertiesTab->labelColor());
  _labelItem->setLabelFont(_propertiesTab->labelFont());
  //saveDimensions(_labelItem);
  if (_saveAsDefault->isChecked()) {
    _labelItem->saveAsDialogDefaults();
  }
}


void LabelItemDialog::setupDimensions() {
  _labelDimensionsTab->enableSingleEditOptions(true);
  _labelDimensionsTab->setupDimensions();
}

void LabelItemDialog::saveDimensions(ViewItem *viewitem) {
  Q_ASSERT(viewitem);

  LabelItem *item = qobject_cast<LabelItem*>(viewitem);
  Q_ASSERT(item);

  QPointF xy(_labelDimensionsTab->x(),_labelDimensionsTab->y());
  qreal theta = _labelDimensionsTab->rotation();
  bool fix_left = _labelDimensionsTab->fixLeft();

  if (_labelDimensionsTab->lockPosToData() && item->dataPosLockable()) {
    QRectF dr = item->dataRelativeRect();
    if (fix_left) {
      dr.moveTopLeft(xy);
      item->setFixLeft(true);
    } else {
      dr.moveBottomRight(xy);
      item->setFixLeft(false);
    }
    item->setDataRelativeRect(dr);

    bool lockPosToData = _labelDimensionsTab->lockPosToDataDirty() ? _labelDimensionsTab->lockPosToData() : item->lockPosToData();
    item->setLockPosToData(lockPosToData);

    item->applyDataLockedDimensions();

  } else {
    QRectF parentRect = item->parentRect();
    qreal parentWidth = parentRect.width();
    qreal parentHeight = parentRect.height();
    qreal parentX = parentRect.x();
    qreal parentY = parentRect.y();
    bool lockPosToData = _labelDimensionsTab->lockPosToDataDirty() ? _labelDimensionsTab->lockPosToData() : item->lockPosToData();
    item->setLockPosToData(lockPosToData);
    qreal width = item->rect().width();
    qreal height = item->rect().height();

    item->setPos(parentX + xy.x()*parentWidth, parentY + xy.y()*parentHeight);
    if (fix_left) {
      item->setViewRect(0,-height, width, height);
      item->setFixLeft(true);
    } else {
      item->setViewRect(-width,-height, width, height);
      item->setFixLeft(false);
    }
  }
  QTransform transform;
  transform.rotate(theta);
  item->setTransform(transform);
  item->updateRelativeSize(true);

}
}

// vim: ts=2 sw=2 et
