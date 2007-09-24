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

#include "vectordialog.h"

#include "dialogpage.h"

namespace Kst {

VectorTab::VectorTab(QWidget *parent)
  : DialogTab(parent) {

  setupUi(this);
  setTabTitle(tr("Vector"));
}


VectorTab::~VectorTab() {
}


VectorDialog::VectorDialog(QWidget *parent)
  : DataDialog(parent) {

  setWindowTitle(tr("New Vector"));

  _vectorTab = new VectorTab(this);
//  connect(_vectorTab, SIGNAL(apply()), this, SLOT(vectorChanged()));
  addDataTab(_vectorTab);
}


VectorDialog::~VectorDialog() {
}

}

// vim: ts=2 sw=2 et
