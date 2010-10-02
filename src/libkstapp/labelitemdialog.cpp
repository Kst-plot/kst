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
}

}

// vim: ts=2 sw=2 et
