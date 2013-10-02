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
  // Set icon size to be the same as other buttons
  int size = style()->pixelMetric(QStyle::PM_SmallIconSize);
  _addVectorToView->setIcon(QPixmap(":magnifying_glass.png"));
  _addVectorToView->setFixedSize(size + 8, size + 8);

  // TODO  ResizeToContents is too expensive
  //_vectors->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  _vectors->horizontalHeader()->setResizeMode(QHeaderView::Stretch);

//  _vectors->verticalHeader()->hide();

//  connect(_vectorSelector, SIGNAL(selectionChanged(const QString&)), this, SLOT(vectorSelected()));
  connect(_addVectorToView, SIGNAL(clicked()), this, SLOT(vectorSelected()));
  connect(_resetButton, SIGNAL(clicked()), this, SLOT(reset()));

  _vectorSelector->setObjectStore(doc->objectStore());

//  setAttribute(Qt::WA_DeleteOnClose);
}


ViewVectorDialog::~ViewVectorDialog() {
//  delete _model;
//  _model = 0;
}


void ViewVectorDialog::show() {
  vectorSelected();
  QDialog::show();
}


void ViewVectorDialog::vectorSelected() {
  if (_model == 0) {
      _model = new VectorModel(_vectorSelector->selectedVector());
      _vectors->setModel(_model);
  } else {
    _model->addVector(_vectorSelector->selectedVector());
  }
}

void ViewVectorDialog::reset() {
  delete _model;
  _model = 0;

}

}

// vim: ts=2 sw=2 et
