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

#include "datamanager.h"
#include "databuttonaction.h"
#include "databutton.h"

#include "dialoglauncher.h"

#include "document.h"
#include "sessionmodel.h"
#include "datacollection.h"
#include "plotitem.h"

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
#include "basicplugin.h"

#include <QHeaderView>
#include <QToolBar>
#include <QMenu>

namespace Kst {

DataManager::DataManager(QWidget *parent, Document *doc)
  : QDialog(parent), _doc(doc), _currentObject(0) {

  setupUi(this);

  // Make sure the dialog gets maximize and minimize buttons under Windows
  QWidget::setWindowFlags((Qt::WindowFlags) Qt::Dialog | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

  _session->header()->setResizeMode(QHeaderView::ResizeToContents);
  _session->setModel(doc->session());
  _session->setContextMenuPolicy(Qt::CustomContextMenu);
  _session->setUniformRowHeights(true);
  connect(_session, SIGNAL(customContextMenuRequested(const QPoint &)),
          this, SLOT(showContextMenu(const QPoint &)));
  connect(_session, SIGNAL(doubleClicked(const QModelIndex &)),
          this, SLOT(showEditDialog(QModelIndex)));

  _contextMenu = new QMenu(this);

  connect(_purge, SIGNAL(clicked()), this, SLOT(purge()));

}

DataManager::~DataManager() {
  // the data manager is only destroyed at exit, so there is no real
  // need to clean up anything...  (though valgrind thinks we really
  // ought to delete all of our actions before we exit)
}

void DataManager::showContextMenu(const QPoint &position) {
  QList<QAction *> actions;
  if (_session->indexAt(position).isValid()) {
    SessionModel *model = static_cast<SessionModel*>(_session->model());
    if (!model->parent(_session->indexAt(position)).isValid()) {
      _currentObject = model->objectList()->at(_session->indexAt(position).row());
      if (_currentObject) {
        QAction *action = new QAction(_currentObject->Name(), this);
        action->setEnabled(false);
        actions.append(action);

        action = new QAction(tr("Edit"), this);
        connect(action, SIGNAL(triggered()), this, SLOT(showEditDialog()));
        actions.append(action);

        if (VectorPtr v = kst_cast<Vector>(_currentObject)) {

          action = new QAction(tr("Make Curve"), this);
          connect(action, SIGNAL(triggered()), this, SLOT(showCurveDialog()));
          actions.append(action);

          action = new QAction(tr("Make Power Spectrum"), this);
          connect(action, SIGNAL(triggered()), this, SLOT(showPowerSpectrumDialog()));
          actions.append(action);

          action = new QAction(tr("Make Spectrogram"), this);
          connect(action, SIGNAL(triggered()), this, SLOT(showCSDDialog()));
          actions.append(action);

          action = new QAction(tr("Make Histogram"), this);
          connect(action, SIGNAL(triggered()), this, SLOT(showHistogramDialog()));
          actions.append(action);

          if (!DataObject::filterPluginList().empty()) {
            action = new QAction(tr("Apply Filter"), this);
            connect(action, SIGNAL(triggered()), this, SLOT(showFilterDialog()));
            actions.append(action);
          }
        } else if (MatrixPtr m = kst_cast<Matrix>(_currentObject)) {
          action = new QAction(tr("Make Image"), this);
          connect(action, SIGNAL(triggered()), this, SLOT(showImageDialog()));
          actions.append(action);
        } else if (RelationPtr r = kst_cast<Relation>(_currentObject)) {

          QMenu *addMenu = new QMenu(this);
          QMenu *removeMenu = new QMenu(this);

          foreach (PlotItemInterface *plot, Data::self()->plotList()) {
            action = new QAction(plot->plotName(), this);
            action->setData(qVariantFromValue(plot));
            addMenu->addAction(action);

            PlotItem* plotItem = static_cast<PlotItem*>(plot);
            if (plotItem) {
              foreach (PlotRenderItem* renderItem, plotItem->renderItems()) {
                if (renderItem->relationList().contains(r)) {
                  action = new QAction(plot->plotName(), this);
                  action->setData(qVariantFromValue(plot));
                  removeMenu->addAction(action);
                  break;
                }
              }
            }
          }

          connect(addMenu, SIGNAL(triggered(QAction*)), this, SLOT(addToPlot(QAction*)));
          action = new QAction(tr("Add to Plot"), this);

          action->setMenu(addMenu);
          actions.append(action);

          connect(removeMenu, SIGNAL(triggered(QAction*)), this, SLOT(removeFromPlot(QAction*)));
          action = new QAction(tr("Remove From Plot"), this);
          connect(action, SIGNAL(triggered()), this, SLOT(showImageDialog()));

          action->setMenu(removeMenu);
          actions.append(action);

          if (!DataObject::fitsPluginList().empty()) {
            action = new QAction(tr("Apply Fit"), this);
            connect(action, SIGNAL(triggered()), this, SLOT(showFitDialog()));
            actions.append(action);
          }

          if (!DataObject::filterPluginList().empty()) {
            action = new QAction(tr("Apply Filter"), this);
            connect(action, SIGNAL(triggered()), this, SLOT(showFilterDialog()));
            actions.append(action);
          }
        }

        action = new QAction(tr("Delete"), this);
        connect(action, SIGNAL(triggered()), this, SLOT(deleteObject()));
        actions.append(action);
      }
    } else {
      DataObjectPtr dataObject = kst_cast<DataObject>(model->objectList()->at(_session->indexAt(position).parent().row()));
      if (dataObject) {
        if (dataObject->outputVectors().count() > _session->indexAt(position).row()) {
          _currentObject = dataObject->outputVectors().values()[_session->indexAt(position).row()];
        } else {
          _currentObject = dataObject->outputMatrices().values()[_session->indexAt(position).row() - dataObject->outputVectors().count()];
        }
        if (_currentObject) {
          QAction *action = new QAction(_currentObject->Name(), this);
          action->setEnabled(false);
          actions.append(action);

          if (VectorPtr v = kst_cast<Vector>(_currentObject)) {
            action = new QAction(tr("Make Curve"), this);
            connect(action, SIGNAL(triggered()), this, SLOT(showCurveDialog()));
            actions.append(action);

            action = new QAction(tr("Make Power Spectrum"), this);
            connect(action, SIGNAL(triggered()), this, SLOT(showPowerSpectrumDialog()));
            actions.append(action);

            action = new QAction(tr("Make Spectrogram"), this);
            connect(action, SIGNAL(triggered()), this, SLOT(showCSDDialog()));
            actions.append(action);

            action = new QAction(tr("Make Histogram"), this);
            connect(action, SIGNAL(triggered()), this, SLOT(showHistogramDialog()));
            actions.append(action);

            if (!DataObject::filterPluginList().empty()) {
              action = new QAction(tr("Apply Filter"), this);
              connect(action, SIGNAL(triggered()), this, SLOT(showFilterDialog()));
              actions.append(action);
            }

          } else if (MatrixPtr m = kst_cast<Matrix>(_currentObject)) {
            action = new QAction(tr("Make Image"), this);
            connect(action, SIGNAL(triggered()), this, SLOT(showImageDialog()));
            actions.append(action);
          }
        }
      }
    }
  }
  if (actions.count() > 0)
      QMenu::exec(actions, _session->mapToGlobal(position));
}

void DataManager::showEditDialog(QModelIndex qml) {
  if (!qml.parent().isValid()) { // don't edit slave objects
    SessionModel *model = static_cast<SessionModel*>(_session->model());

    _currentObject = model->objectList()->at(qml.row());

    showEditDialog();
  }
}

void DataManager::showEditDialog() {
  DialogLauncher::self()->showObjectDialog(_currentObject);
}


bool DataManager::event(QEvent * event) {
  if ((event->type() == QEvent::WindowActivate) || (event->type() == QEvent::Show)) {
    _doc->session()->triggerReset();
  }
  return QDialog::event(event);
}


void DataManager::showVectorDialog() {
  QString tmp;
  DialogLauncher::self()->showVectorDialog(tmp);
}


void DataManager::showMatrixDialog() {
  QString tmp;
  DialogLauncher::self()->showMatrixDialog(tmp);
}


void DataManager::showScalarDialog() {
  QString scalarName;
  DialogLauncher::self()->showScalarDialog(scalarName);
}


void DataManager::showStringDialog() {
  QString stringName;
  DialogLauncher::self()->showStringDialog(stringName);
}


void DataManager::showEventMonitorDialog() {
  DialogLauncher::self()->showEventMonitorDialog();
}


void DataManager::showEquationDialog() {
  DialogLauncher::self()->showEquationDialog();
}


void DataManager::showCurveDialog() {
  if (VectorPtr vector = kst_cast<Vector>(_currentObject)) {
    DialogLauncher::self()->showCurveDialog(0, vector);
  } else {
    DialogLauncher::self()->showCurveDialog();
  }
}


void DataManager::showCSDDialog() {
  if (VectorPtr vector = kst_cast<Vector>(_currentObject)) {
    DialogLauncher::self()->showCSDDialog(0, vector);
  } else {
    DialogLauncher::self()->showCSDDialog();
  }
}


void DataManager::showPowerSpectrumDialog() {
  if (VectorPtr vector = kst_cast<Vector>(_currentObject)) {
    DialogLauncher::self()->showPowerSpectrumDialog(0, vector);
  } else {
    DialogLauncher::self()->showPowerSpectrumDialog();
  }
}


void DataManager::showHistogramDialog() {
  if (VectorPtr vector = kst_cast<Vector>(_currentObject)) {
    DialogLauncher::self()->showHistogramDialog(0, vector);
  } else {
    DialogLauncher::self()->showHistogramDialog();
  }
}


void DataManager::showImageDialog() {
  if (MatrixPtr matrix = kst_cast<Matrix>(_currentObject)) {
    DialogLauncher::self()->showImageDialog(0, matrix);
  } else {
    DialogLauncher::self()->showImageDialog();
  }
}


void DataManager::showPluginDialog(QString &pluginName) {
  if (VectorPtr vector = kst_cast<Vector>(_currentObject)) {
    DialogLauncher::self()->showBasicPluginDialog(pluginName, 0, vector);
  } else if (CurvePtr curve = kst_cast<Curve>(_currentObject)) {
    DialogLauncher::self()->showBasicPluginDialog(pluginName, 0, curve->xVector(), curve->yVector());
  } else {
    DialogLauncher::self()->showBasicPluginDialog(pluginName);
  }
}


void DataManager::showFilterDialog() {
  showPluginDialog(DataObject::filterPluginList().first());
}


void DataManager::showFitDialog() {
  showPluginDialog(DataObject::fitsPluginList().first());
}


void DataManager::deleteObject() {
  if (RelationPtr relation = kst_cast<Relation>(_currentObject)) {
    Data::self()->removeCurveFromPlots(relation);
    _doc->objectStore()->removeObject(relation);
  } else if (DataObjectPtr dataObject = kst_cast<DataObject>(_currentObject)) {
    _doc->objectStore()->removeObject(dataObject);
  } else if (PrimitivePtr primitive = kst_cast<Primitive>(_currentObject)) {
    _doc->objectStore()->removeObject(primitive);
  }
  _currentObject = 0;
  _doc->session()->triggerReset();
  _doc->objectStore()->cleanUpDataSourceList();
}


void DataManager::addToPlot(QAction* action) {
  PlotItem* plotItem = static_cast<PlotItem*>(qVariantValue<PlotItemInterface*>(action->data()));
  RelationPtr relation = kst_cast<Relation>(_currentObject);
  if (plotItem && relation) {
    PlotRenderItem *renderItem = plotItem->renderItem(PlotRenderItem::Cartesian);
    renderItem->addRelation(kst_cast<Relation>(relation));
    plotItem->update();
  }
}


void DataManager::removeFromPlot(QAction* action) {
  bool plotUpdated = false;

  PlotItem* plotItem = static_cast<PlotItem*>(qVariantValue<PlotItemInterface*>(action->data()));
  RelationPtr relation = kst_cast<Relation>(_currentObject);
  if (plotItem && relation) {
    foreach (PlotRenderItem* renderItem, plotItem->renderItems()) {
      if (renderItem->relationList().contains(relation)) {
        renderItem->removeRelation(relation);
        plotUpdated = true;
      }
    }
    if (plotUpdated) {
      plotItem->update();
    }
  }
}

void DataManager::setUsedFlags() {
  _doc->objectStore()->clearUsedFlags();

  // for each relation used in an unhidden plot mark 'used' - O(N)
  QList<PlotItem*> plotlist = ViewItem::getItems<PlotItem>();
  foreach (PlotItem *plot, plotlist) {
    if (plot->isVisible()) {
      foreach (PlotRenderItem *renderer, plot->renderItems()) {
        foreach (RelationPtr relation, renderer->relationList()) {
          relation->setUsed(true);
        }
      }
    }
  }

  // TODO: for each primitive used in an unhidden label mark 'used' - O(N)

  // for each primitive used by a relation mark 'used' - O(N)
  ObjectList<Relation> relationList = _doc->objectStore()->getObjects<Relation>();
  foreach (RelationPtr object, relationList) {
    object->readLock();
    //set used all input and output primitives
    foreach (VectorPtr v, object->inputVectors()) {
      v->setUsed(true);
      if (v->provider()) {
        v->provider()->setUsed(true);
      }
    }
    foreach (VectorPtr v, object->outputVectors()) {
      v->setUsed(true);
    }
    foreach (ScalarPtr s, object->inputScalars()) {
      s->setUsed(true);
      if (s->provider()) {
        s->provider()->setUsed(true);
      }
    }
    foreach (ScalarPtr s, object->outputScalars()) {
      s->setUsed(true);
    }
    foreach (StringPtr s, object->inputStrings()) {
      s->setUsed(true);
      if (s->provider()) {
        s->provider()->setUsed(true);
      }
    }
    foreach (StringPtr s, object->outputStrings()) {
      s->setUsed(true);
    }
    foreach (MatrixPtr m, object->inputMatrices()) {
      m->setUsed(true);
      if (m->provider()) {
        m->provider()->setUsed(true);
      }
    }
    foreach (MatrixPtr m, object->outputMatrices()) {
      m->setUsed(true);
    }
    object->unlock();
  }

  ObjectList<DataObject> dataObjectList = _doc->objectStore()->getObjects<DataObject>();
  foreach (DataObjectPtr object, dataObjectList) {
    object->readLock();
    //set used all input and output primitives
    foreach (VectorPtr v, object->inputVectors()) {
      v->setUsed(true);
    }
    foreach (VectorPtr v, object->outputVectors()) {
      v->setUsed(true);
    }
    foreach (ScalarPtr s, object->inputScalars()) {
      s->setUsed(true);
    }
    foreach (ScalarPtr s, object->outputScalars()) {
      s->setUsed(true);
    }
    foreach (StringPtr s, object->inputStrings()) {
      s->setUsed(true);
    }
    foreach (StringPtr s, object->outputStrings()) {
      s->setUsed(true);
    }
    foreach (MatrixPtr m, object->inputMatrices()) {
      m->setUsed(true);
    }
    foreach (MatrixPtr m, object->outputMatrices()) {
      m->setUsed(true);
    }
    object->unlock();
  }
}

void DataManager::purge() {
  do {
    setUsedFlags();
  } while (_doc->objectStore()->deleteUnsetUsedFlags());
  _doc->session()->triggerReset();
  _session->reset();
}

}

// vim: ts=2 sw=2 et
