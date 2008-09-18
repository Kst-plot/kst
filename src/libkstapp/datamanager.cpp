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
  connect(action, SIGNAL(triggered()), this, SLOT(showVectorDialog()));
  _primitives->addAction(action);

   action = new DataButtonAction(tr("Matrix"));
   connect(action, SIGNAL(triggered()), this, SLOT(showMatrixDialog()));
   _primitives->addAction(action);
 
   action = new DataButtonAction(tr("Scalar"));
   connect(action, SIGNAL(triggered()), this, SLOT(showScalarDialog()));
   _primitives->addAction(action);
// 
//   action = new DataButtonAction(tr("String"));
//   connect(action, SIGNAL(triggered()), this, SLOT(showStringDialog()));
//   _primitives->addAction(action);

  action = new DataButtonAction(tr("Curve"));
  connect(action, SIGNAL(triggered()), this, SLOT(showCurveDialog()));
  _dataObjects->addAction(action);

  action = new DataButtonAction(tr("Equation"));
  connect(action, SIGNAL(triggered()), this, SLOT(showEquationDialog()));
  _dataObjects->addAction(action);

  action = new DataButtonAction(tr("Histogram"));
  connect(action, SIGNAL(triggered()), this, SLOT(showHistogramDialog()));
  _dataObjects->addAction(action);

  action = new DataButtonAction(tr("Power Spectrum"));
  connect(action, SIGNAL(triggered()), this, SLOT(showPowerSpectrumDialog()));
  _dataObjects->addAction(action);

  action = new DataButtonAction(tr("Event Monitor"));
  connect(action, SIGNAL(triggered()), this, SLOT(showEventMonitorDialog()));
  _dataObjects->addAction(action);

  action = new DataButtonAction(tr("Image"));
  connect(action, SIGNAL(triggered()), this, SLOT(showImageDialog()));
  _dataObjects->addAction(action);

  action = new DataButtonAction(tr("Spectrogram"));
  connect(action, SIGNAL(triggered()), this, SLOT(showCSDDialog()));
  _dataObjects->addAction(action);

  foreach (QString pluginName, DataObject::dataObjectPluginList()) {
    action = new DataButtonAction(tr(pluginName));
    connect(action, SIGNAL(triggered(QString&)), this, SLOT(showPluginDialog(QString&)));
    _dataObjects->addAction(action);
  }

  foreach (QString pluginName, DataObject::fitsPluginList()) {
    action = new DataButtonAction(tr(pluginName));
    connect(action, SIGNAL(triggered(QString&)), this, SLOT(showPluginDialog(QString&)));
    _fits->addAction(action);
  }

  foreach (QString pluginName, DataObject::filterPluginList()) {
    action = new DataButtonAction(tr(pluginName));
    connect(action, SIGNAL(triggered(QString&)), this, SLOT(showPluginDialog(QString&)));
    _filters->addAction(action);
  }
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
        }

        action = new QAction(tr("Delete"), this);
        connect(action, SIGNAL(triggered()), this, SLOT(deleteObject()));
        actions.append(action);
      }
    } else {
      DataObjectPtr dataObject = kst_cast<DataObject>(model->generateObjectList().at(_session->indexAt(position).parent().row()));
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


void DataManager::showEditDialog() {
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
    QString tmp;
    DialogLauncher::self()->showVectorDialog(tmp, vector);
  } else if (MatrixPtr matrix = kst_cast<Matrix>(_currentObject)) {
    QString tmp;
    DialogLauncher::self()->showMatrixDialog(tmp, matrix);
  } else if (BasicPluginPtr plugin = kst_cast<BasicPlugin>(_currentObject)) {
    DialogLauncher::self()->showBasicPluginDialog(plugin->pluginName(), plugin);
  }
  _doc->session()->triggerReset();
}


void DataManager::show() {
  _doc->session()->triggerReset();
  QDialog::show();
}


void DataManager::showVectorDialog() {
  QString tmp;
  DialogLauncher::self()->showVectorDialog(tmp);
  _doc->session()->triggerReset();
}


void DataManager::showMatrixDialog() {
  QString tmp;
  DialogLauncher::self()->showMatrixDialog(tmp);
  _doc->session()->triggerReset();
}


void DataManager::showScalarDialog() {
  QString scalarName;
  DialogLauncher::self()->showScalarDialog(scalarName);
  _doc->session()->triggerReset();
}


void DataManager::showEventMonitorDialog() {
  DialogLauncher::self()->showEventMonitorDialog();
  _doc->session()->triggerReset();
}


void DataManager::showEquationDialog() {
  DialogLauncher::self()->showEquationDialog();
  _doc->session()->triggerReset();
}


void DataManager::showCurveDialog() {
  if (VectorPtr vector = kst_cast<Vector>(_currentObject)) {
    DialogLauncher::self()->showCurveDialog(0, vector);
  } else {
    DialogLauncher::self()->showCurveDialog();
  }
  _doc->session()->triggerReset();
}


void DataManager::showCSDDialog() {
  if (VectorPtr vector = kst_cast<Vector>(_currentObject)) {
    DialogLauncher::self()->showCSDDialog(0, vector);
  } else {
    DialogLauncher::self()->showCSDDialog();
  }
  _doc->session()->triggerReset();
}


void DataManager::showPowerSpectrumDialog() {
  if (VectorPtr vector = kst_cast<Vector>(_currentObject)) {
    DialogLauncher::self()->showPowerSpectrumDialog(0, vector);
  } else {
    DialogLauncher::self()->showPowerSpectrumDialog();
  }
  _doc->session()->triggerReset();
}


void DataManager::showHistogramDialog() {
  if (VectorPtr vector = kst_cast<Vector>(_currentObject)) {
    DialogLauncher::self()->showHistogramDialog(0, vector);
  } else {
    DialogLauncher::self()->showHistogramDialog();
  }
  _doc->session()->triggerReset();
}


void DataManager::showImageDialog() {
  if (MatrixPtr matrix = kst_cast<Matrix>(_currentObject)) {
    DialogLauncher::self()->showImageDialog(0, matrix);
  } else {
    DialogLauncher::self()->showImageDialog();
  }
  _doc->session()->triggerReset();
}


void DataManager::showPluginDialog(QString &pluginName) {
  DialogLauncher::self()->showBasicPluginDialog(pluginName);

  _doc->session()->triggerReset();
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

}

// vim: ts=2 sw=2 et
