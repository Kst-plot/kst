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

#include "viewprimitivedialog.h"

#include "document.h"
#include "objectstore.h"
#include "scalarmodel.h"
#include "stringmodel.h"

#include <datacollection.h>
#include <QHeaderView>

namespace Kst {

ViewPrimitiveDialog::ViewPrimitiveDialog(QWidget *parent, Document *doc, PrimitiveType type)
  : QDialog(parent), _doc(doc), _type(type) {
  _model = 0;
  setupUi(this);
  refresh();
}


ViewPrimitiveDialog::~ViewPrimitiveDialog() {
  _tree->setModel(0);
  delete _model;
  _model = 0;
}


void ViewPrimitiveDialog::refresh() {
  if (_model) {
    delete _model;
  }
  if (_type == Scalar) {
    _model = (QAbstractItemModel*)new ScalarModel(_doc->objectStore());
    _tree->header()->setResizeMode(QHeaderView::ResizeToContents);
    _tree->setModel(_model);
  } else if (_type == String) {
    _model = (QAbstractItemModel*)new StringModel(_doc->objectStore());
    _tree->header()->setResizeMode(QHeaderView::ResizeToContents);
    _tree->setModel(_model);
  }
}


}

// vim: ts=2 sw=2 et
