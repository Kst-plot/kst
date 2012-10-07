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

#include "viewprimitivedialog.h"

#include "document.h"
#include "objectstore.h"
#include "scalarmodel.h"
#include "stringmodel.h"

#include <datacollection.h>
#include <QHeaderView>

#ifdef QT5
#define setResizeMode setSectionResizeMode
#endif

namespace Kst {

ViewPrimitiveDialog::ViewPrimitiveDialog(QWidget *parent, Document *doc)
  : QDialog(parent), _doc(doc) {
  _model = 0;
  setupUi(this);
  setAttribute(Qt::WA_DeleteOnClose);
  connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
}


ViewPrimitiveDialog::~ViewPrimitiveDialog() {
  deleteModel();
}

void ViewPrimitiveDialog::deleteModel() {
  if (_model) {
    _tree->setModel(0);
    delete _model;
    _model = 0;
  }
}


void ViewPrimitiveDialog::update() {
  deleteModel();
  _model = createModel(_doc->objectStore());
  _tree->setModel(_model);

  _tree->header()->setResizeMode(QHeaderView::ResizeToContents);
  _tree->header()->setStretchLastSection(false);
  QApplication::processEvents();
  _tree->header()->setResizeMode(QHeaderView::Interactive);
}



ViewStringDialog::ViewStringDialog(QWidget *parent, Document *doc) :
  ViewPrimitiveDialog(parent, doc)
{
  setWindowTitle(tr("View String Values"));
  update();
}


QAbstractItemModel* ViewStringDialog::createModel(ObjectStore *store)
{
  return new StringModel(store);
}


ViewScalarDialog::ViewScalarDialog(QWidget *parent, Document *doc) :
  ViewPrimitiveDialog(parent, doc)
{
  setWindowTitle(tr("View Scalar Values"));
  update();
}


QAbstractItemModel* ViewScalarDialog::createModel(ObjectStore *store)
{
  return new ScalarModel(store);
}


}

// vim: ts=2 sw=2 et
