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

#include "applicationsettings.h"
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
  _generalTab->setUseOpenGL(ApplicationSettings::self()->useOpenGL());
  _generalTab->setReferenceViewWidth(ApplicationSettings::self()->referenceViewWidthCM());
  _generalTab->setReferenceViewHeight(ApplicationSettings::self()->referenceViewHeightCM());
  _generalTab->setReferenceFontSize(ApplicationSettings::self()->referenceFontSize());
  _generalTab->setMinimumFontSize(ApplicationSettings::self()->minimumFontSize());
  _generalTab->setDefaultFontFamily(ApplicationSettings::self()->defaultFontFamily());
}


void ApplicationSettingsDialog::setupGrid() {
  _gridTab->setShowGrid(ApplicationSettings::self()->showGrid());
  _gridTab->setSnapToGrid(ApplicationSettings::self()->snapToGrid());
  _gridTab->setGridHorizontalSpacing(ApplicationSettings::self()->gridHorizontalSpacing());
  _gridTab->setGridVerticalSpacing(ApplicationSettings::self()->gridVerticalSpacing());
}


void ApplicationSettingsDialog::generalChanged() {
  //Need to block the signals so that the modified signal only goes out once...
  ApplicationSettings::self()->blockSignals(true);
  ApplicationSettings::self()->setUseOpenGL(_generalTab->useOpenGL());
  ApplicationSettings::self()->setReferenceViewWidthCM(_generalTab->referenceViewWidth());
  ApplicationSettings::self()->setReferenceViewHeightCM(_generalTab->referenceViewHeight());
  ApplicationSettings::self()->setReferenceFontSize(_generalTab->referenceFontSize());
  ApplicationSettings::self()->setMinimumFontSize(_generalTab->minimumFontSize());
  ApplicationSettings::self()->setDefaultFontFamily(_generalTab->defaultFontFamily());
  ApplicationSettings::self()->blockSignals(false);

  emit ApplicationSettings::self()->modified();
}


void ApplicationSettingsDialog::gridChanged() {
  //Need to block the signals so that the modified signal only goes out once...
  ApplicationSettings::self()->blockSignals(true);
  ApplicationSettings::self()->setShowGrid(_gridTab->showGrid());
  ApplicationSettings::self()->setSnapToGrid(_gridTab->snapToGrid());
  ApplicationSettings::self()->setGridHorizontalSpacing(_gridTab->gridHorizontalSpacing());
  ApplicationSettings::self()->setGridVerticalSpacing(_gridTab->gridVerticalSpacing());
  ApplicationSettings::self()->blockSignals(false);

  emit ApplicationSettings::self()->modified();
}

}

// vim: ts=2 sw=2 et
