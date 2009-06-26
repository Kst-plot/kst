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

#include "viewmatrixdialog.h"

#include "document.h"
#include "matrixmodel.h"

#include <datacollection.h>

namespace Kst {

ViewMatrixDialog::ViewMatrixDialog(QWidget *parent, Document *doc)
  : QDialog(parent), _doc(doc) {
  _model = 0;
  setupUi(this);

  connect(matrixSelector, SIGNAL(selectionChanged()), this, SLOT(matrixSelected()));
  matrixSelector->setObjectStore(doc->objectStore());

  setAttribute(Qt::WA_DeleteOnClose);
}


ViewMatrixDialog::~ViewMatrixDialog() {
  delete _model;
  _model = 0;
}


void ViewMatrixDialog::show() {
  matrixSelector->updateMatrices();
  matrixSelected();
  QDialog::show();
}


void ViewMatrixDialog::matrixSelected() {
  if (_model) {
    delete _model;
  }

  MatrixPtr m = matrixSelector->selectedMatrix();
  if (m) {
    _model = new MatrixModel(matrixSelector->selectedMatrix());
    _matrices->setModel(_model);
  }
}



}

// vim: ts=2 sw=2 et
