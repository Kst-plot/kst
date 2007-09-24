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

#include "datamanager.h"

#include "vectordialog.h"

#include "document.h"
#include "sessionmodel.h"

#include <QToolBar>

namespace Kst {

DataManager::DataManager(QWidget *parent, Document *doc)
  : QDialog(parent), _doc(doc) {

  setupUi(this);
  _session->setModel(doc->session());

  _primitives = new QToolBar(_objects);
  _primitives->setOrientation(Qt::Vertical);
  _primitives->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  _dataObjects = new QToolBar(_objects);
  _dataObjects->setOrientation(Qt::Vertical);
  _dataObjects->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  _fits = new QToolBar(_objects);
  _fits->setOrientation(Qt::Vertical);
  _fits->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  _filters = new QToolBar(_objects);
  _filters->setOrientation(Qt::Vertical);
  _filters->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  _objects->addItem(_primitives, tr("Create Primitive"));
  _objects->addItem(_dataObjects, tr("Create Data Object"));
  _objects->addItem(_fits, tr("Create Fit"));
  _objects->addItem(_filters, tr("Create Filter"));

//   Create canonical items...

//FIXME "Scalar"

  QAction *action = _primitives->addAction(tr("Vector"));
  connect(action, SIGNAL(triggered()), this, SLOT(showVectorDialog()));
  QWidget *widget = _primitives->widgetForAction(action);
  widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  action = _primitives->addAction(tr("Matrix"));
  widget = _primitives->widgetForAction(action);
  widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

//FIXME "String"

  action = _dataObjects->addAction(tr("Curve"));
  widget = _dataObjects->widgetForAction(action);
  widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  action = _dataObjects->addAction(tr("Equation"));
  widget = _dataObjects->widgetForAction(action);
  widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  action = _dataObjects->addAction(tr("Histogram"));
  widget = _dataObjects->widgetForAction(action);
  widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  action = _dataObjects->addAction(tr("Power Spectrum"));
  widget = _dataObjects->widgetForAction(action);
  widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  action = _dataObjects->addAction(tr("Event Monitor"));
  widget = _dataObjects->widgetForAction(action);
  widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  action = _dataObjects->addAction(tr("Image"));
  widget = _dataObjects->widgetForAction(action);
  widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  action = _dataObjects->addAction(tr("CSD"));
  widget = _dataObjects->widgetForAction(action);
  widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}


DataManager::~DataManager() {
}


void DataManager::showVectorDialog() {
  VectorDialog dialog(this);
  dialog.exec();
}

}

// vim: ts=2 sw=2 et
