/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "viewvectordialog.h"

#include "document.h"
#include "vectormodel.h"

#include <datacollection.h>
#include <objectstore.h>
#include <QHeaderView>

#ifdef QT5
#define setResizeMode setSectionResizeMode
#endif

namespace Kst {

ViewVectorDialog::ViewVectorDialog(QWidget *parent, Document *doc)
  : QDialog(parent), _doc(doc) {
  _model = 0;

  Q_ASSERT(_doc && _doc->objectStore());
  setupUi(this);
  // TODO  ResizeToContents is too expensive
  //_vectors->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  _vectors->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

  _vectors->verticalHeader()->hide();

  connect(_vectorSelector, SIGNAL(selectionChanged(const QString&)), this, SLOT(vectorSelected()));
  _vectorSelector->setObjectStore(doc->objectStore());

  setAttribute(Qt::WA_DeleteOnClose);
}


ViewVectorDialog::~ViewVectorDialog() {
  delete _model;
  _model = 0;
}


void ViewVectorDialog::show() {
  vectorSelected();
  QDialog::show();
}


void ViewVectorDialog::vectorSelected() {
  if (_model) {
    delete _model;
  }

  VectorPtr vector = _vectorSelector->selectedVector();
  if (vector) {
    _model = new VectorModel(vector);
    _vectors->setModel(_model);
  }
}

}

// vim: ts=2 sw=2 et
