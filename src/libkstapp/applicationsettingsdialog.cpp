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
#include "filltab.h"
#include "dialogpage.h"
#include "childviewoptionstab.h"
#include "defaultlabelpropertiestab.h"

#include <QDebug>

namespace Kst {

ApplicationSettingsDialog::ApplicationSettingsDialog(QWidget *parent)
    : Dialog(parent) {

  setWindowTitle(tr("Kst Settings"));

  _generalTab = new GeneralTab(this);
  _gridTab = new GridTab(this);
  _fillTab = new FillTab(this);
  _defaultLabelPropertiesTab = new DefaultLabelPropertiesTab(this);
//   _childViewOptionsTab = new ChildViewOptionsTab(this);

  connect(_generalTab, SIGNAL(apply()), this, SLOT(generalChanged()));
  connect(_gridTab, SIGNAL(apply()), this, SLOT(gridChanged()));
  connect(_fillTab, SIGNAL(apply()), this, SLOT(fillChanged()));
//   connect(_childViewOptionsTab, SIGNAL(apply()), this, SLOT(childViewOptionsChanged()));
  connect(_defaultLabelPropertiesTab, SIGNAL(apply()), this, SLOT(defaultLabelPropertiesChanged()));

  DialogPage *general = new DialogPage(this);
  general->setPageTitle(tr("General"));
  general->addDialogTab(_generalTab);
  addDialogPage(general);

  DialogPage *grid = new DialogPage(this);
  grid->setPageTitle(tr("Grid"));
  grid->addDialogTab(_gridTab);
  addDialogPage(grid);

  DialogPage *fill = new DialogPage(this);
  fill->setPageTitle(tr("Fill"));
  fill->addDialogTab(_fillTab);
  addDialogPage(fill);

  DialogPage *defaultLabelProperties = new DialogPage(this);
  defaultLabelProperties->setPageTitle(tr("Default Label Properties"));
  defaultLabelProperties->addDialogTab(_defaultLabelPropertiesTab);
  addDialogPage(defaultLabelProperties);

//   DialogPage *childViewOptions = new DialogPage(this);
//   childViewOptions->setPageTitle(tr("Child View Options"));
//   childViewOptions->addDialogTab(_childViewOptionsTab);
//   addDialogPage(childViewOptions);

  setupGeneral();
  setupGrid();
  setupFill();
  setupDefaultLabelProperties();
//   setupChildViewOptions();

  selectDialogPage(general);
}


ApplicationSettingsDialog::~ApplicationSettingsDialog() {
}


void ApplicationSettingsDialog::setupGeneral() {
  _generalTab->setUseOpenGL(ApplicationSettings::self()->useOpenGL());
  _generalTab->setReferenceViewWidth(ApplicationSettings::self()->referenceViewWidthCM());
  _generalTab->setReferenceViewHeight(ApplicationSettings::self()->referenceViewHeightCM());
  _generalTab->setReferenceFontSize(ApplicationSettings::self()->referenceFontSize());
  _generalTab->setMinimumFontSize(ApplicationSettings::self()->minimumFontSize());
  _generalTab->setMinimumUpdatePeriod(ApplicationSettings::self()->minimumUpdatePeriod());
}


void ApplicationSettingsDialog::setupGrid() {
  _gridTab->setShowGrid(ApplicationSettings::self()->showGrid());
  _gridTab->setSnapToGrid(ApplicationSettings::self()->snapToGrid());
  _gridTab->setGridHorizontalSpacing(ApplicationSettings::self()->gridHorizontalSpacing());
  _gridTab->setGridVerticalSpacing(ApplicationSettings::self()->gridVerticalSpacing());
}


void ApplicationSettingsDialog::setupFill() {
  QGradientStops stops;
  stops.append(qMakePair(1.0, QColor(Qt::white)));
  stops.append(qMakePair(0.0, QColor(Qt::lightGray)));
  _fillTab->gradientEditor()->setDefaultGradientStops(stops);

  QBrush b = ApplicationSettings::self()->backgroundBrush();

  _fillTab->setColor(b.color());
  _fillTab->setStyle(b.style());

  if (const QGradient *gradient = b.gradient()) {
    _fillTab->setGradient(*gradient);
  }
}


void ApplicationSettingsDialog::setupDefaultLabelProperties() {
  _defaultLabelPropertiesTab->setLabelFont(ApplicationSettings::self()->defaultFont());
  _defaultLabelPropertiesTab->setLabelScale(ApplicationSettings::self()->defaultFontScale());
  _defaultLabelPropertiesTab->setLabelColor(ApplicationSettings::self()->defaultFontColor());
}


void ApplicationSettingsDialog::generalChanged() {
  //Need to block the signals so that the modified signal only goes out once...
  ApplicationSettings::self()->blockSignals(true);
  ApplicationSettings::self()->setUseOpenGL(_generalTab->useOpenGL());
  ApplicationSettings::self()->setReferenceViewWidthCM(_generalTab->referenceViewWidth());
  ApplicationSettings::self()->setReferenceViewHeightCM(_generalTab->referenceViewHeight());
  ApplicationSettings::self()->setReferenceFontSize(_generalTab->referenceFontSize());
  ApplicationSettings::self()->setMinimumFontSize(_generalTab->minimumFontSize());
  ApplicationSettings::self()->setMinimumUpdatePeriod(_generalTab->minimumUpdatePeriod());
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


void ApplicationSettingsDialog::fillChanged() {
  QBrush b = ApplicationSettings::self()->backgroundBrush();

  b.setColor(_fillTab->color());
  b.setStyle(_fillTab->style());

  QGradient gradient = _fillTab->gradient();
  if (gradient.type() != QGradient::NoGradient) {
    QLinearGradient linearGradient;
    linearGradient.setStops(gradient.stops());
    b = QBrush(linearGradient);
  }

  ApplicationSettings::self()->setBackgroundBrush(b);

  emit ApplicationSettings::self()->modified();
}


void ApplicationSettingsDialog::defaultLabelPropertiesChanged() {
  //Need to block the signals so that the modified signal only goes out once...
  ApplicationSettings::self()->blockSignals(true);
  ApplicationSettings::self()->setDefaultFont(_defaultLabelPropertiesTab->labelFont());
  ApplicationSettings::self()->setDefaultFontScale(_defaultLabelPropertiesTab->labelScale());
  ApplicationSettings::self()->setDefaultFontColor(_defaultLabelPropertiesTab->labelColor());
  ApplicationSettings::self()->blockSignals(false);

  emit ApplicationSettings::self()->modified();
}


void ApplicationSettingsDialog::setupChildViewOptions() {
  _childViewOptionsTab->setShareAxis(ApplicationSettings::self()->shareAxis());
}


void ApplicationSettingsDialog::childViewOptionsChanged() {
  ApplicationSettings::self()->setShareAxis(_childViewOptionsTab->shareAxis());
}

}

// vim: ts=2 sw=2 et
