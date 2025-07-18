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
#include "plotaxis.h"
#include "labelitem.h"

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
#include "updateserver.h"

#include <QHeaderView>
#include <QToolBar>
#include <QMenu>
#include <QShortcut>
#include <QSortFilterProxyModel>

namespace Kst {

DataManager::DataManager(QWidget *parent, Document *doc)
  : QDialog(parent), _doc(doc), _currentObject(0) {

  setupUi(this);

  MainWindow::setWidgetFlags(this);

  // Setup proxy model for filtering / sorting
  _proxyModel = new QSortFilterProxyModel(this);
  _proxyModel->setSourceModel(doc->session());
  _proxyModel->setFilterKeyColumn(-1); // Filter on all columns by default
  _proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
  _session->setModel(_proxyModel);

  _filterText->setPlaceholderText(tr("Enter your filter here (wildcards allowed)"));

  connect(_filterText, SIGNAL(textChanged(QString)), _proxyModel, SLOT(setFilterWildcard(QString)));
  connect(_caseSensitive, SIGNAL(stateChanged(int)), this, SLOT(setCaseSensitivity(int)));
  connect(_filterColumn, SIGNAL(currentIndexChanged(int)), this, SLOT(setFilterColumn(int)));


  _session->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  _session->setContextMenuPolicy(Qt::CustomContextMenu);
  _session->setSortingEnabled(true);
  _session->sortByColumn(1, Qt::AscendingOrder); // Sort by type by default
  _session->setUniformRowHeights(true);
  connect(_session, SIGNAL(customContextMenuRequested(QPoint)),
          this, SLOT(showContextMenu(QPoint)));
  connect(_session, SIGNAL(doubleClicked(QModelIndex)),
          this, SLOT(showEditDialog(QModelIndex)));

  // Simple keyboard shortcut for fast object deletion
  _deleteShortcut = new QShortcut(Qt::Key_Delete, this);
  connect(_deleteShortcut, SIGNAL(activated()), this, SLOT(deleteObject()));

  _contextMenu = new QMenu(this);

  connect(_purge, SIGNAL(clicked()), this, SLOT(purge()));
  connect(_delete, SIGNAL(clicked()), this, SLOT(deleteObject()));
  connect(_edit, SIGNAL(clicked()), this, SLOT(showEditDialog()));
}

DataManager::~DataManager() {
  // the data manager is only destroyed at exit, so there is no real
  // need to clean up anything...  (though valgrind thinks we really
  // ought to delete all of our actions before we exit)
}


void DataManager::showEvent(QShowEvent*)
{
  _session->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
  _session->header()->setStretchLastSection(false);
  QApplication::processEvents();
  _session->header()->setSectionResizeMode(QHeaderView::Interactive);
}


void DataManager::showContextMenu(const QPoint &position) {
  QList<QAction *> actions;
  if (_session->indexAt(position).isValid()) {
    SessionModel *model = static_cast<SessionModel*>(_doc->session());
    if (!model->parent(_proxyModel->mapToSource(_session->indexAt(position))).isValid()) {
      _currentObject = model->objectList()->at(_proxyModel->mapToSource(_session->indexAt(position)).row());
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
            action->setData(QVariant::fromValue(plot));
            addMenu->addAction(action);

            PlotItem* plotItem = static_cast<PlotItem*>(plot);
            if (plotItem) {
              foreach (PlotRenderItem* renderItem, plotItem->renderItems()) {
                if (renderItem->relationList().contains(r)) {
                  action = new QAction(plot->plotName(), this);
                  action->setData(QVariant::fromValue(plot));
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

        // Also add delete action in the menu
        action = new QAction(tr("Delete"), this);
        connect(action, SIGNAL(triggered()), this, SLOT(deleteObject()));
        actions.append(action);
      }
    } else {
      DataObjectPtr dataObject = kst_cast<DataObject>(model->objectList()->at(_proxyModel->mapToSource(_session->indexAt(position)).parent().row()));
      if (dataObject) {
        if (dataObject->outputVectors().count() > _proxyModel->mapToSource(_session->indexAt(position)).row()) {
          _currentObject = dataObject->outputVectors().values()[_proxyModel->mapToSource(_session->indexAt(position)).row()];
        } else {
          _currentObject = dataObject->outputMatrices().values()[_proxyModel->mapToSource(_session->indexAt(position)).row() - dataObject->outputVectors().count()];
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
    //SessionModel *model = static_cast<SessionModel*>(_doc->session());

    //_currentObject = model->objectList()->at(_proxyModel->mapToSource(qml).row());

    showEditDialog(_proxyModel->mapToSource(qml).row());
  }
}


void DataManager::showEditDialog() {

  if (_session->selectionModel()->selectedIndexes().size()<1) {
    return;
  }

  QModelIndex qml = _session->selectionModel()->selectedIndexes()[0];

  if (qml.parent().isValid()) { // don't edit slave objects.
    return;
  }

  int row = _proxyModel->mapToSource(qml).row(); // Single selection mode => only one selected index

  showEditDialog(row);
}


void DataManager::showEditDialog(int row) {
  SessionModel *model = static_cast<SessionModel*>(_doc->session());
  if ((row < 0) || (row >=model->objectList()->size())) {
    return;
  }
  DialogLauncher::self()->showObjectDialog(model->objectList()->at(row));
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
  SessionModel *model = static_cast<SessionModel*>(_doc->session());

  if (_session->selectionModel()->selectedIndexes().size()<1) {
    return;
  }

  int row = _proxyModel->mapToSource(_session->selectionModel()->selectedIndexes()[0]).row(); // Single selection mode => only one selected index
  _currentObject = model->objectList()->at(row);
  if (RelationPtr relation = kst_cast<Relation>(_currentObject)) {
    Data::self()->removeCurveFromPlots(relation);
    _doc->objectStore()->removeObject(relation);
  } else if (DataObjectPtr dataObject = kst_cast<DataObject>(_currentObject)) {
    _doc->objectStore()->removeObject(dataObject);
  } else if (PrimitivePtr primitive = kst_cast<Primitive>(_currentObject)) {
    _doc->objectStore()->removeObject(primitive);
  } else if (DataSourcePtr datasource = kst_cast<DataSource>(_currentObject)) {
    _doc->objectStore()->removeObject(datasource);
  }
  _currentObject = 0;
  UpdateServer::self()->requestUpdateSignal();

  // Now select the next item
  _session->selectionModel()->select(_proxyModel->mapFromSource(model->index(row,0)), QItemSelectionModel::Select);
  // Cleanup and return
  _doc->objectStore()->cleanUpDataSourceList();
}

void DataManager::addToPlot(QAction* action) {
  PlotItem* plotItem = static_cast<PlotItem*>(action->data().value<PlotItemInterface*>());
  RelationPtr relation = kst_cast<Relation>(_currentObject);
  if (plotItem && relation) {
    PlotRenderItem *renderItem = plotItem->renderItem(PlotRenderItem::Cartesian);
    renderItem->addRelation(kst_cast<Relation>(relation));
    plotItem->update();
  }
}


void DataManager::removeFromPlot(QAction* action) {
  bool plotUpdated = false;

  PlotItem* plotItem = static_cast<PlotItem*>(action->data().value<PlotItemInterface*>());
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

// search through all the objects to see what is a dependency of anything shown.
// FIXME: this is very fragile - objects use objects in all sorts of ways,
// all which have to be listed here.
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
      if (plot->xAxis()->axisPlotMarkers().isCurveSource()) {
        plot->xAxis()->axisPlotMarkers().curve()->setUsed(true);
      }
      if (plot->yAxis()->axisPlotMarkers().isCurveSource()) {
        plot->yAxis()->axisPlotMarkers().curve()->setUsed(true);
      }
      if (plot->xAxis()->axisPlotMarkers().isVectorSource()) {
        plot->xAxis()->axisPlotMarkers().vector()->setUsed(true);
      }
      if (plot->yAxis()->axisPlotMarkers().isVectorSource()) {
        plot->yAxis()->axisPlotMarkers().vector()->setUsed(true);
      }
    }
  }

  QList<LabelItem*> labels = ViewItem::getItems<LabelItem>();
  foreach (LabelItem * label, labels) {
    if (label->_labelRc) {
      foreach (Primitive* primitive, label->_labelRc->_refObjects) {
        primitive->setUsed(true);
      }
    }
  }

  // for each primitive used by a relation mark 'used' - O(N)
  ObjectList<Relation> relationList = _doc->objectStore()->getObjects<Relation>();
  foreach (RelationPtr object, relationList) {
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


  ObjectList<DataObject> dataObjectList = _doc->objectStore()->getObjects<DataObject>();
  foreach (DataObjectPtr object, dataObjectList) {
    object->readLock();
    //set used all input and output primitives
    foreach (PrimitivePtr p, object->inputPrimitives()) {
      p->setUsed(true);
    }
    object->unlock();
  }
}

void DataManager::purge() {
  do {
    setUsedFlags();
  } while (_doc->objectStore()->deleteUnsetUsedFlags());
  _doc->objectStore()->cleanUpDataSourceList();
  UpdateServer::self()->requestUpdateSignal();
  _session->reset();
}

void DataManager::setFilterColumn(int column) {
  _proxyModel->setFilterKeyColumn(column-1);
}

void DataManager::setCaseSensitivity(int state) {
  if (state) {
    _proxyModel->setFilterCaseSensitivity(Qt::CaseSensitive);
  } else {
    _proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
  }
}

}
// vim: ts=2 sw=2 et
