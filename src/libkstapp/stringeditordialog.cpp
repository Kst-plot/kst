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

#include "stringeditordialog.h"

#include <datacollection.h>
#include "stringtablemodel.h"

namespace Kst {

StringEditorDialog::StringEditorDialog(QWidget *parent)
  : QDialog(parent) {
  _model = 0;
  setupUi(this);
  refreshStrings();

}


StringEditorDialog::~StringEditorDialog() {
  _strings->setModel(0);
  qDeleteAll(_model->strings());
  _model->strings().clear();
  delete _model;
  _model = 0;
}


void StringEditorDialog::show() {
  refreshStrings();
  QDialog::show();
}


void StringEditorDialog::refreshStrings() {
  if (_model) {
    delete _model;
  }
  _model = new StringTableModel;
  // TODO: Extract this model so the dialog can be reused, and make a new model
  // or modification to the model so that it tracks all the string creates and
  // destroys
  StringList stringList;  // FIXME
  foreach (StringPtr v, stringList) {
    StringModel *vm = new StringModel(v);
    _model->strings().append(vm);
  }
  _strings->setModel(_model);
}

}

// vim: ts=2 sw=2 et
