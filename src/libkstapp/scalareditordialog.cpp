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

#include "scalareditordialog.h"

#include "document.h"
#include "objectstore.h"
#include "scalartablemodel.h"

#include <datacollection.h>

namespace Kst {

ScalarEditorDialog::ScalarEditorDialog(QWidget *parent, Document *doc)
  : QDialog(parent), _doc(doc) {
  _model = 0;
  setupUi(this);
  refreshScalars();
}


ScalarEditorDialog::~ScalarEditorDialog() {
  _scalars->setModel(0);
  qDeleteAll(_model->scalars());
  _model->scalars().clear();
  delete _model;
  _model = 0;
}


void ScalarEditorDialog::show() {
  refreshScalars();
  QDialog::show();
}


void ScalarEditorDialog::refreshScalars() {
  if (_model) {
    delete _model;
  }
  _model = new ScalarTableModel;
  // TODO: Extract this model so the dialog can be reused, and make a new model
  // or modification to the model so that it tracks all the scalar creates and
  // destroys
  Q_ASSERT(_doc && _doc->objectStore());
  ScalarList scalarList = _doc->objectStore()->getObjects<Scalar>();
  foreach (ScalarPtr v, scalarList) {
    ScalarModel *vm = new ScalarModel(v);
    _model->scalars().append(vm);
  }
  _scalars->setModel(_model);
}


}

// vim: ts=2 sw=2 et
