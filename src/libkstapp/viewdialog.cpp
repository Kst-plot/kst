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

#include "viewdialog.h"

#include "view.h"
#include "filltab.h"
#include "gridtab.h"
#include "childviewoptionstab.h"
#include "dialogpage.h"
#include "document.h"
#include "mainwindow.h"
#include "application.h"

#include <QBrush>
#include <QDebug>

namespace Kst {

ViewDialog::ViewDialog(View *view, QWidget *parent)
    : Dialog(parent), _view(view) {

  setWindowTitle(tr("Edit View"));

  _fillTab = new FillTab(this);
  _gridTab = new GridTab(this);
//   _childViewOptionsTab = new ChildViewOptionsTab(this);

  connect(_fillTab, SIGNAL(apply()), this, SLOT(fillChanged()));
  connect(_gridTab, SIGNAL(apply()), this, SLOT(gridChanged()));
//   connect(_childViewOptionsTab, SIGNAL(apply()), this, SLOT(childViewOptionsChanged()));

  DialogPage *grid = new DialogPage(this);
  grid->setPageTitle(tr("Grid"));
  grid->addDialogTab(_gridTab);
  addDialogPage(grid);

  DialogPage *fill = new DialogPage(this);
  fill->setPageTitle(tr("Fill"));
  fill->addDialogTab(_fillTab);
  addDialogPage(fill);

//   DialogPage *childViewOptions = new DialogPage(this);
//   childViewOptions->setPageTitle(tr("Child View Options"));
//   childViewOptions->addDialogTab(_childViewOptionsTab);
//   addDialogPage(childViewOptions);

  setupFill();
  setupGrid();
//   setupChildViewOptions();

  selectDialogPage(grid);
}


ViewDialog::~ViewDialog() {
}


void ViewDialog::setupFill() {
  Q_ASSERT(_view);
  QBrush b = _view->backgroundBrush();

  _fillTab->setColor(b.color());
  _fillTab->setStyle(b.style());

  if (const QGradient *gradient = b.gradient()) {
    _fillTab->setGradient(*gradient);
  }
}


void ViewDialog::setupGrid() {
  _gridTab->setShowGrid(_view->showGrid());
  _gridTab->setSnapToGrid(_view->snapToGrid());
  _gridTab->setGridHorizontalSpacing(_view->gridSpacing().width());
  _gridTab->setGridVerticalSpacing(_view->gridSpacing().height());
}


void ViewDialog::setupChildViewOptions() {
}


void ViewDialog::fillChanged() {
  Q_ASSERT(_view);

  QBrush b = _view->backgroundBrush();

  b.setColor(_fillTab->color());
  b.setStyle(_fillTab->style());

  QGradient gradient = _fillTab->gradient();
  if (gradient.type() != QGradient::NoGradient) {
    b = QBrush(gradient);
  }
  kstApp->mainWindow()->document()->setChanged(true);
  _view->setBackgroundBrush(b);
}


void ViewDialog::gridChanged() {
  _view->setShowGrid(_gridTab->showGrid());
  _view->setSnapToGrid(_gridTab->snapToGrid());
  _view->setGridSpacing(QSizeF(_gridTab->gridHorizontalSpacing(), _gridTab->gridVerticalSpacing()));
}


void ViewDialog::childViewOptionsChanged() {
}

}

// vim: ts=2 sw=2 et
