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
#include "dialogpage.h"
#include "document.h"
#include "mainwindow.h"
#include "application.h"
#include "dialogdefaults.h"

#include <QBrush>
#include <QDebug>

namespace Kst {

ViewDialog::ViewDialog(View *view, QWidget *parent)
    : Dialog(parent), _view(view) {

  setWindowTitle(tr("Edit View"));

  _fillTab = new FillTab(this);

  connect(_fillTab, SIGNAL(apply()), this, SLOT(fillChanged()));

  _saveAsDefault->show();

  DialogPage *fill = new DialogPage(this);
  fill->setPageTitle(tr("Fill"));
  fill->addDialogTab(_fillTab);
  addDialogPage(fill);

  setupFill();

  selectDialogPage(fill);

}


ViewDialog::~ViewDialog() {
}


void ViewDialog::setupFill() {
  Q_ASSERT(_view);
  QBrush b = _view->backgroundBrush();

  _fillTab->initialize(&b);
}

void ViewDialog::fillChanged() {
  Q_ASSERT(_view);

  QBrush b = _fillTab->brush(_view->backgroundBrush());

  kstApp->mainWindow()->document()->setChanged(true);
  _view->setBackgroundBrush(b);

  if (_saveAsDefault->isChecked()) {
    saveDialogDefaultsBrush(View::staticDefaultsGroupName(), _view->backgroundBrush());
  }
}

}

// vim: ts=2 sw=2 et
