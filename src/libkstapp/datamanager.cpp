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

#include "dialoglauncher.h"

#include "document.h"
#include "sessionmodel.h"

#include "objectstore.h"
#include "dataobject.h"
#include "curve.h"
#include "equation.h"
#include "vector.h"
#include "matrix.h"
#include "histogram.h"
#include "psd.h"
#include "eventmonitorentry.h"
#include "image.h"
#include "csd.h"

#include <QHeaderView>
#include <QToolBar>
#include <QMenu>

namespace Kst {

DataManager::DataManager(QWidget *parent, Document *doc)
  : QDialog(parent), _doc(doc), _currentObject(0) {

  setupUi(this);
  _session->header()->setResizeMode(QHeaderView::ResizeToContents);
  _session->setModel(doc->session());
  _session->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(_session, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(showContextMenu(const QPoint &)));

  _contextMenu = new QMenu(this);

  _objects->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
  _objects->setStyleSheet("background-color: white;");


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

  QAction *action = new DataButtonAction(tr("Vector"));
  connect(action, SIGNAL(triggered()), DialogLauncher::self(), SLOT(showVectorDialog()));
  _primitives->addAction(action);

   action = new DataButtonAction(tr("Matrix"));
   connect(action, SIGNAL(triggered()), DialogLauncher::self(), SLOT(showMatrixDialog()));
   _primitives->addAction(action);
 
   action = new DataButtonAction(tr("Scalar"));
   connect(action, SIGNAL(triggered()), DialogLauncher::self(), SLOT(showScalarDialog()));
   _primitives->addAction(action);
// 
//   action = new DataButtonAction(tr("String"));
//   connect(action, SIGNAL(triggered()), this, SLOT(showStringDialog()));
//   _primitives->addAction(action);

  action = new DataButtonAction(tr("Curve"));
  connect(action, SIGNAL(triggered()), DialogLauncher::self(), SLOT(showCurveDialog()));
  _dataObjects->addAction(action);

  action = new DataButtonAction(tr("Equation"));
  connect(action, SIGNAL(triggered()), DialogLauncher::self(), SLOT(showEquationDialog()));
  _dataObjects->addAction(action);

   action = new DataButtonAction(tr("Histogram"));
   connect(action, SIGNAL(triggered()), DialogLauncher::self(), SLOT(showHistogramDialog()));
   _dataObjects->addAction(action);
 
   action = new DataButtonAction(tr("Power Spectrum"));
   connect(action, SIGNAL(triggered()), DialogLauncher::self(), SLOT(showPowerSpectrumDialog()));
   _dataObjects->addAction(action);
 
   action = new DataButtonAction(tr("Event Monitor"));
   connect(action, SIGNAL(triggered()), DialogLauncher::self(), SLOT(showEventMonitorDialog()));
   _dataObjects->addAction(action);
 
   action = new DataButtonAction(tr("Image"));
   connect(action, SIGNAL(triggered()), DialogLauncher::self(), SLOT(showImageDialog()));
   _dataObjects->addAction(action);
 
   action = new DataButtonAction(tr("Spectrogram"));
   connect(action, SIGNAL(triggered()), DialogLauncher::self(), SLOT(showCSDDialog()));
   _dataObjects->addAction(action);
}


DataManager::~DataManager() {
}


void DataManager::showContextMenu(const QPoint &position) {
  QList<QAction *> actions;
  if (_session->indexAt(position).isValid()) {
    SessionModel *model = static_cast<SessionModel*>(_session->model());
    if (!model->parent(_session->indexAt(position)).isValid()) {
      _currentObject = model->generateObjectList().at(_session->indexAt(position).row());
      if (_currentObject) {
        QAction *action = new QAction(_currentObject->tag().displayString(), this);
        actions.append(action);

        action = new DataButtonAction(tr("Edit"));
        connect(action, SIGNAL(triggered()), this, SLOT(showEditDialog()));
        actions.append(action);

        if (VectorPtr v = kst_cast<Vector>(_currentObject)) {

          action = new DataButtonAction(tr("Make Curve"));
          connect(action, SIGNAL(triggered()), this, SLOT(showCurveDialog()));
          actions.append(action);

          action = new DataButtonAction(tr("Make Power Spectrum"));
          connect(action, SIGNAL(triggered()), this, SLOT(showPowerSpectrumDialog()));
          actions.append(action);

          action = new DataButtonAction(tr("Make Spectrogram"));
          connect(action, SIGNAL(triggered()), this, SLOT(showCSDDialog()));
          actions.append(action);

          action = new DataButtonAction(tr("Make Histogram"));
          connect(action, SIGNAL(triggered()), this, SLOT(showHistogramDialog()));
          actions.append(action);

        } else if (MatrixPtr m = kst_cast<Matrix>(_currentObject)) {
          action = new DataButtonAction(tr("Make Image"));
          connect(action, SIGNAL(triggered()), this, SLOT(showImageDialog()));
          actions.append(action);
        } else if (RelationPtr r = kst_cast<Relation>(_currentObject)) {
        }

        action = new DataButtonAction(tr("Delete"));
        connect(action, SIGNAL(triggered()), this, SLOT(deleteObject()));
        actions.append(action);
      }
    } else {
      // TODO what options do output vectors get?
    }
  }
  if (actions.count() > 0)
      QMenu::exec(actions, _session->mapToGlobal(position));
}


void DataManager::showEditDialog() {
  if (_currentObject) {
    if (CurvePtr curve = kst_cast<Curve>(_currentObject)) {
      DialogLauncher::self()->showCurveDialog(curve);
    } else if (EquationPtr equation = kst_cast<Equation>(_currentObject)) {
      DialogLauncher::self()->showEquationDialog(equation);
    } else if (HistogramPtr histogram = kst_cast<Histogram>(_currentObject)) {
      DialogLauncher::self()->showHistogramDialog(histogram);
    } else if (PSDPtr psd = kst_cast<PSD>(_currentObject)) {
      DialogLauncher::self()->showPowerSpectrumDialog(psd);
    } else if (EventMonitorEntryPtr eventMonitorEntry = kst_cast<EventMonitorEntry>(_currentObject)) {
      DialogLauncher::self()->showEventMonitorDialog(eventMonitorEntry);
    } else if (ImagePtr image = kst_cast<Image>(_currentObject)) {
      DialogLauncher::self()->showImageDialog(image);
    } else if (CSDPtr csd = kst_cast<CSD>(_currentObject)) {
      DialogLauncher::self()->showCSDDialog(csd);
    } else if (VectorPtr vector = kst_cast<Vector>(_currentObject)) {
      DialogLauncher::self()->showVectorDialog(vector);
    } else if (MatrixPtr matrix = kst_cast<Matrix>(_currentObject)) {
      DialogLauncher::self()->showMatrixDialog(matrix);
    }
  }
}


void DataManager::showCurveDialog() {
  if (_currentObject) {
    if (VectorPtr vector = kst_cast<Vector>(_currentObject)) {
      DialogLauncher::self()->showCurveDialog(0, vector);
    }
  }
}


void DataManager::showCSDDialog() {
  if (_currentObject) {
    if (VectorPtr vector = kst_cast<Vector>(_currentObject)) {
      DialogLauncher::self()->showCSDDialog(0, vector);
    }
  }
}


void DataManager::showPowerSpectrumDialog() {
  if (_currentObject) {
    if (VectorPtr vector = kst_cast<Vector>(_currentObject)) {
      DialogLauncher::self()->showPowerSpectrumDialog(0, vector);
    }
  }
}


void DataManager::showHistogramDialog() {
  if (_currentObject) {
    if (VectorPtr vector = kst_cast<Vector>(_currentObject)) {
      DialogLauncher::self()->showHistogramDialog(0, vector);
    }
  }
}


void DataManager::showImageDialog() {
  if (_currentObject) {
    if (MatrixPtr matrix = kst_cast<Matrix>(_currentObject)) {
      DialogLauncher::self()->showImageDialog(0, matrix);
    }
  }
}


void DataManager::deleteObject() {
  qDebug() << "deleteObject" << endl;
}

}

// vim: ts=2 sw=2 et
