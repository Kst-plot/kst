/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "applicationsettingsdialog.h"

#include "gridtab.h"
#include "generaltab.h"
#include "dialogpage.h"

#include <QDebug>

namespace Kst {

ApplicationSettingsDialog::ApplicationSettingsDialog(QWidget *parent)
    : Dialog(parent) {

  setWindowTitle(tr("Kst Settings"));

  _generalTab = new GeneralTab(this);
  _gridTab = new GridTab(this);
  connect(_generalTab, SIGNAL(apply()), this, SLOT(generalChanged()));
  connect(_gridTab, SIGNAL(apply()), this, SLOT(gridChanged()));

  DialogPage *general = new DialogPage(this);
  general->setPageTitle(tr("General"));
  general->addDialogTab(_generalTab);
  addDialogPage(general);

  DialogPage *grid = new DialogPage(this);
  grid->setPageTitle(tr("Grid"));
  grid->addDialogTab(_gridTab);
  addDialogPage(grid);

  setupGeneral();
  setupGrid();
}


ApplicationSettingsDialog::~ApplicationSettingsDialog() {
}


void ApplicationSettingsDialog::setupGeneral() {
}


void ApplicationSettingsDialog::setupGrid() {
}


void ApplicationSettingsDialog::generalChanged() {
}


void ApplicationSettingsDialog::gridChanged() {
}

}

// vim: ts=2 sw=2 et
