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
#include "editmultiplewidget.h"

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

  connect(_resetButton, SIGNAL(clicked()), this, SLOT(reset()));

  // Add vector list, reusing the editmultiplewidget class + some tweaking
  _showMultipleWidget = new EditMultipleWidget();
  QPushButton *addButton = new QPushButton();
  addButton->setIcon(QPixmap(":kst_rightarrow.png"));
  QPushButton *removeButton = new QPushButton();
  removeButton->setIcon(QPixmap(":kst_leftarrow.png"));
  if (_showMultipleWidget) {
    // Set header
    _showMultipleWidget->setHeader(i18n("Select Vectors to View"));
    // Populate the list
    update();
    // Finish setting up the layout
    _listLayout->addWidget(_showMultipleWidget,0,0,Qt::AlignLeft);
    QVBoxLayout *addRemoveButtons = new QVBoxLayout();
    addRemoveButtons->addStretch();
    addRemoveButtons->addWidget(addButton);
    addRemoveButtons->addWidget(removeButton);
    addRemoveButtons->addStretch();
    _listLayout->addLayout(addRemoveButtons,0,1);
  }
  _splitter->setStretchFactor(0,0);
  _splitter->setStretchFactor(1,1);
  connect(addButton, SIGNAL(clicked()), this, SLOT(addSelected()));
  connect(removeButton, SIGNAL(clicked()), this, SLOT(removeSelected()));
//  setAttribute(Qt::WA_DeleteOnClose);
}


ViewVectorDialog::~ViewVectorDialog() {
  delete _model;
  delete _showMultipleWidget;
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
    removeSelected();
  }
}

void ViewVectorDialog::update()
{
  VectorList objects = _doc->objectStore()->getObjects<Vector>();
  _showMultipleWidget->clearObjects();
  foreach(VectorPtr object, objects) {
    _showMultipleWidget->addObject(object->Name(), object->descriptionTip());
  }
}

void ViewVectorDialog::addSelected() {
  if (_model == 0) {
      _model = new VectorModel();
      _vectors->setModel(_model);
  }
  // Retrieve list of selected objects by name
  QStringList objects = _showMultipleWidget->selectedObjects();
  // Get to the pointers and add them to the model
  foreach (const QString &objectName, objects) {
    VectorPtr vector = kst_cast<Vector>(_doc->objectStore()->retrieveObject(objectName));
    if (vector) {
      _model->addVector(vector);
    }
  }
}

void ViewVectorDialog::removeSelected() {
  if (!_model) {
    return;
  }
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

void ViewVectorDialog::reset() {
  delete _model;
  _model = 0;
}


}

// vim: ts=2 sw=2 et
