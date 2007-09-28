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
#include "databuttonaction.h"
#include "databutton.h"

#include "vectordialog.h"
#include "curvedialog.h"

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
  _primitives->setBackgroundRole(QPalette::Base);

  _dataObjects = new QToolBar(_objects);
  _dataObjects->setOrientation(Qt::Vertical);
  _dataObjects->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  _dataObjects->setBackgroundRole(QPalette::Base);

  _fits = new QToolBar(_objects);
  _fits->setOrientation(Qt::Vertical);
  _fits->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  _fits->setBackgroundRole(QPalette::Base);

  _filters = new QToolBar(_objects);
  _filters->setOrientation(Qt::Vertical);
  _filters->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  _filters->setBackgroundRole(QPalette::Base);

  _objects->addItem(_primitives, tr("Create Primitive"));
  _objects->addItem(_dataObjects, tr("Create Data Object"));
  _objects->addItem(_fits, tr("Create Fit"));
  _objects->addItem(_filters, tr("Create Filter"));

//   Create canonical items...

  QAction *action = new DataButtonAction(tr("Vector"));
  connect(action, SIGNAL(triggered()), this, SLOT(showVectorDialog()));
  _primitives->addAction(action);

//   action = new DataButtonAction(tr("Matrix"));
//   connect(action, SIGNAL(triggered()), this, SLOT(showMatrixDialog()));
//   _primitives->addAction(action);
// 
//   action = new DataButtonAction(tr("Scalar"));
//   connect(action, SIGNAL(triggered()), this, SLOT(showScalarDialog()));
//   _primitives->addAction(action);
// 
//   action = new DataButtonAction(tr("String"));
//   connect(action, SIGNAL(triggered()), this, SLOT(showStringDialog()));
//   _primitives->addAction(action);

  action = new DataButtonAction(tr("Curve"));
  connect(action, SIGNAL(triggered()), this, SLOT(showCurveDialog()));
  _dataObjects->addAction(action);

//   action = new DataButtonAction(tr("Equation"));
//   connect(action, SIGNAL(triggered()), this, SLOT(showEquationDialog()));
//   _dataObjects->addAction(action);
// 
//   action = new DataButtonAction(tr("Histogram"));
//   connect(action, SIGNAL(triggered()), this, SLOT(showHistogramDialog()));
//   _dataObjects->addAction(action);
// 
//   action = new DataButtonAction(tr("Power Spectrum"));
//   connect(action, SIGNAL(triggered()), this, SLOT(showPowerSpectrumDialog()));
//   _dataObjects->addAction(action);
// 
//   action = new DataButtonAction(tr("Event Monitor")));
//   connect(action, SIGNAL(triggered()), this, SLOT(showEventMonitorDialog()));
//   _dataObjects->addAction(action);
// 
//   action = new DataButtonAction(tr("Image"));
//   connect(action, SIGNAL(triggered()), this, SLOT(showImageDialog()));
//   _dataObjects->addAction(action);
// 
//   action = new DataButtonAction(tr("CSD"));
//   connect(action, SIGNAL(triggered()), this, SLOT(showCSDDialog()));
//   _dataObjects->addAction(action);
}


DataManager::~DataManager() {
}


void DataManager::showVectorDialog() {
  VectorDialog dialog(this);
  dialog.exec();
}


void DataManager::showCurveDialog() {
  CurveDialog dialog(this);
  dialog.exec();
}

}

// vim: ts=2 sw=2 et
