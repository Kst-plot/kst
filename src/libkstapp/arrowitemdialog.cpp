/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "arrowitemdialog.h"

#include "arrowitem.h"
#include "arrowpropertiestab.h"
#include "dialogpage.h"
#include "dialogdefaults.h"
#include "application.h"
#include "objectstore.h"
#include "mainwindow.h"
#include "document.h"

namespace Kst {

ArrowItemDialog::ArrowItemDialog(ArrowItem *item, QWidget *parent)
    : LineItemDialog(item, parent), _arrowItem(item) {

  _propertiesTab = new ArrowPropertiesTab(this);
  DialogPage *propertiesPage = new DialogPage(this);
  propertiesPage->setPageTitle(tr("Properties"));
  propertiesPage->addDialogTab(_propertiesTab);
  addDialogPage(propertiesPage);
  selectDialogPage(propertiesPage);
  connect(_propertiesTab, SIGNAL(apply()), this, SLOT(propertiesChanged()));

  setupProperties();
}


ArrowItemDialog::~ArrowItemDialog() {
}


void ArrowItemDialog::setupProperties() {
  _propertiesTab->setStartArrowHead(_arrowItem->startArrowHead());
  _propertiesTab->setEndArrowHead(_arrowItem->endArrowHead());
  _propertiesTab->setStartArrowScale(_arrowItem->startArrowScale());
  _propertiesTab->setEndArrowScale(_arrowItem->endArrowScale());
}


void ArrowItemDialog::propertiesChanged() {
  _arrowItem->setStartArrowHead(_propertiesTab->startArrowHead());
  _arrowItem->setEndArrowHead(_propertiesTab->endArrowHead());
  _arrowItem->setStartArrowScale(_propertiesTab->startArrowScale());
  _arrowItem->setEndArrowScale(_propertiesTab->endArrowScale());

  if (_saveAsDefault->isChecked()) {
    _dialogDefaults->setValue("arrow/hasEndHead", QVariant(_arrowItem->endArrowHead()).toString());
    if (_arrowItem->endArrowHead()) {
      _dialogDefaults->setValue("arrow/endHeadScale", _arrowItem->endArrowScale());
    }
    _dialogDefaults->setValue("arrow/hasStartHead", QVariant(_arrowItem->startArrowHead()).toString());
    if (_arrowItem->startArrowHead()) {
      _dialogDefaults->setValue("arrow/startHeadScale", _arrowItem->startArrowScale());
    }
  }
}

}

// vim: ts=2 sw=2 et
