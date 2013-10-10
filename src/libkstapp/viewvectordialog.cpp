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
#include <QMenu>

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
  _vectors->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
  // Allow reorganizing the columns per drag&drop
  _vectors->horizontalHeader()->setMovable(true);

  // Custom context menu for the remove action
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(contextMenu(const QPoint&)));

  connect(_vectorSelector, SIGNAL(selectionChanged(const QString&)), this, SLOT(vectorSelected()));
  connect(_resetButton, SIGNAL(clicked()), this, SLOT(reset()));

  _vectorSelector->setObjectStore(doc->objectStore());

//  setAttribute(Qt::WA_DeleteOnClose);
}


ViewVectorDialog::~ViewVectorDialog() {
  delete _model;
}


void ViewVectorDialog::show() {
  // vectorSelected();
  QDialog::show();
}

void ViewVectorDialog::contextMenu(const QPoint& position)
{
  QMenu menu;
  QPoint cursor = QCursor::pos();
  QAction* removeAction = menu.addAction(tr("Remove"));
  QAction* selectedItem = menu.exec(cursor);
  if (selectedItem == removeAction) {
    // Get current selection
    QModelIndexList sel = _vectors->selectionModel()->selectedIndexes();
    // Now go through the list to see how may columns it spans
    QList<int> columns;
    QModelIndex index;
    foreach (index, sel) {
      if (!columns.contains(index.column())) {
        columns << index.column();
      }
    }
    // Sort the columns in descending order
    qSort(columns.begin(), columns.end(), qGreater<int>());
    // Remove columns starting from the highest index to avoid shifting them
    int column;
    foreach (column, columns) {
      _model->removeVector(column);
    }
  }
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
