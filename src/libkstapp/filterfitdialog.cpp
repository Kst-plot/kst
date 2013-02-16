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

#include "filterfitdialog.h"

#include "dialogpage.h"

#include "datacollection.h"
#include "document.h"
#include "basicplugin.h"
#include "objectstore.h"
#include "curve.h"
#include "labelitem.h"
#include "updatemanager.h"

#include <QMessageBox>

namespace Kst {

FilterFitTab::FilterFitTab(QString& pluginName, QWidget *parent)
  : DataTab(parent), _configWidget(0), _layout(0), _store(0), _vectorX(0), _vectorY(0), _lockVectors(false) {

  setupUi(this);
  setTabTitle(tr("Plugin"));

  _type = (DataObjectPluginInterface::PluginTypeID)DataObject::pluginType(pluginName);
  if (_type == DataObjectPluginInterface::Filter) {
    _pluginCombo->addItems(DataObject::filterPluginList());
  } else if (_type == DataObjectPluginInterface::Fit) {
    _pluginCombo->addItems(DataObject::fitsPluginList());
  }

  _curveAppearance->setVisible(false);
  _curvePlacement->setVisible(false);
  _ignoreAutoScale->setVisible(false);
  _curvePlacement->setPlace(CurvePlacement::NoPlot);

  pluginChanged(pluginName);
  connect(_pluginCombo, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(pluginChanged(const QString&)));
  connect(_curvePlacement->_noPlot, SIGNAL(toggled(bool)), _curveAppearance, SLOT(setDisabled(bool)));
}


FilterFitTab::~FilterFitTab() {
}


void FilterFitTab::setObjectStore(ObjectStore *store) {
  _store = store;
   _configWidget->setObjectStore(store);
}


void FilterFitTab::setVectorX(VectorPtr vector) {
  _vectorX = vector;
  if (_configWidget) {
    _configWidget->setVectorX(vector);
  }
  lockVectors();
}


void FilterFitTab::setVectorY(VectorPtr vector) {
  _vectorY = vector;
  if (_configWidget) {
    _configWidget->setVectorY(vector);
  }
  lockVectors();
}


void FilterFitTab::setPlotMode(PlotItem* plot) {

  _curvePlacement->setPlace(CurvePlacement::ExistingPlot);
  _curvePlacement->setExistingPlots(Data::self()->plotList());
  if (plot) {
    _curvePlacement->setCurrentPlot(plot);
  }
  _curveAppearance->setVisible(true);
  _curvePlacement->setVisible(true);
  _ignoreAutoScale->setVisible(true);
  lockVectors();
}


void FilterFitTab::lockVectors() {
  if (!_lockVectors) {
    _lockVectors = true;
    if (_configWidget) {
      _configWidget->setVectorsLocked(true);
    }
  }
}


CurveAppearance* FilterFitTab::curveAppearance() const {
  return _curveAppearance;
}

CurvePlacement* FilterFitTab::curvePlacement() const {
  return _curvePlacement;
}


QString FilterFitTab::pluginName() {
  return _pluginCombo->currentText();
}


void FilterFitTab::pluginChanged(const QString &plugin) {
  if (plugin != _pluginCombo->currentText()) {
    _pluginCombo->setCurrentIndex(_pluginCombo->findText(plugin));
  }

  _pluginLabel->setText(plugin);
  _descriptionLabel->setText(DataObject::pluginDescription(plugin));

  if (_layout) {
    delete _layout;
  }
  if (_configWidget) {
    delete _configWidget;
  }
  _layout = new QGridLayout(_inputOutputBox);
  _configWidget = DataObject::pluginWidget(plugin);
  _configWidget->setupSlots(this);
  if (_store) {
    _configWidget->setObjectStore(_store);
  }
  if (_vectorX) {
    _configWidget->setVectorX(_vectorX);
  }
  if (_vectorY) {
    _configWidget->setVectorY(_vectorY);
  }
  if (_lockVectors) {
    _configWidget->setVectorsLocked(true);
  }
  _layout->addWidget(_configWidget, 0, 0);
  _layout->activate();
}


FilterFitDialog::FilterFitDialog(QString& pluginName, ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent), _vectorX(0), _vectorY(0) {

  QString title;
  if (editMode() == Edit)
    title = tr("Edit ") + pluginName + tr(" Plugin");
  else
    title = tr("New ") + pluginName + tr(" Plugin");
  setWindowTitle(title);

  _filterFitTab = new FilterFitTab(pluginName, this);
  addDataTab(_filterFitTab);

  if (dataObject) {
    _filterFitTab->configWidget()->setupFromObject(dataObject);
  } else {
    _filterFitTab->configWidget()->load();
    configureTab();
  }
}


FilterFitDialog::~FilterFitDialog() {
}


void FilterFitDialog::configureTab() {
  _filterFitTab->curveAppearance()->loadWidgetDefaults();
}


QString FilterFitDialog::tagString() const {
  return DataDialog::tagString();
}


void FilterFitDialog::setVectorX(VectorPtr vector) {
  _vectorX = vector;
  _filterFitTab->setVectorX(vector);
}


void FilterFitDialog::setVectorY(VectorPtr vector) {
  _vectorY = vector;
  _filterFitTab->setVectorY(vector);
}


void FilterFitDialog::setPlotMode(PlotItem* plot) {
  _filterFitTab->setPlotMode(plot);
}


ObjectPtr FilterFitDialog::createNewDataObject() {
  BasicPluginPtr dataObject = kst_cast<BasicPlugin>(DataObject::createPlugin(_filterFitTab->pluginName(), _document->objectStore(), _filterFitTab->configWidget()));
  Q_ASSERT(dataObject);

  if (!dataObject->isValid()) {
    _document->objectStore()->removeObject(dataObject);
    QString msg(tr("Unable to create Plugin Object using provided parameters.\n\n"));
    msg += dataObject->errorMessage();
    QMessageBox::warning(this, tr("Kst"), msg);

    return 0;
  }

  if(editMode()==New) {
      PlotItem *plotItem = 0;
      switch (_filterFitTab->curvePlacement()->place()) {
      case CurvePlacement::NoPlot:
          break;
      case CurvePlacement::ExistingPlot:
      {
          plotItem = static_cast<PlotItem*>(_filterFitTab->curvePlacement()->existingPlot());
          break;
      }
      case CurvePlacement::NewPlotNewTab:
          _document->createView();
          // fall through to case NewPlot.
      case CurvePlacement::NewPlot:
      {
          CreatePlotForCurve *cmd = new CreatePlotForCurve();
          cmd->createItem();

          plotItem = static_cast<PlotItem*>(cmd->item());
          if (_filterFitTab->curvePlacement()->scaleFonts()) {
              plotItem->view()->resetPlotFontSizes(1);
              plotItem->view()->configurePlotFontDefaults(plotItem); // copy plots already in window
          }
          plotItem->view()->appendToLayout(_filterFitTab->curvePlacement()->layout(), plotItem,
                                           _filterFitTab->curvePlacement()->gridColumns());
          if (_filterFitTab->curvePlacement()->layout() == CurvePlacement::Custom) {
            plotItem->createCustomLayout(_filterFitTab->curvePlacement()->gridColumns());
          }
          break;
      }
      default:
          break;
      }

      if (plotItem) {
          CurvePtr curve = _document->objectStore()->createObject<Curve>();
          Q_ASSERT(curve);

          if (!_vectorX) {
              setVectorX(dataObject->inputVectors().value(dataObject->inputVectorList().first()));
          }
          Q_ASSERT(_vectorX);
          curve->setXVector(_vectorX);

          VectorPtr yVector = dataObject->outputVectors().value(dataObject->outputVectorList().first());
          Q_ASSERT(yVector);
          curve->setYVector(yVector);

          curve->setColor(_filterFitTab->curveAppearance()->color());
          curve->setHasPoints(_filterFitTab->curveAppearance()->showPoints());
          curve->setHasLines(_filterFitTab->curveAppearance()->showLines());
          curve->setHasBars(_filterFitTab->curveAppearance()->showBars());
          curve->setLineWidth(_filterFitTab->curveAppearance()->lineWidth());
          curve->setPointSize(_filterFitTab->curveAppearance()->pointSize());
          curve->setLineStyle(_filterFitTab->curveAppearance()->lineStyle());
          curve->setPointType(_filterFitTab->curveAppearance()->pointType());
          curve->setPointDensity(_filterFitTab->curveAppearance()->pointDensity());
          curve->setBarFillColor(_filterFitTab->curveAppearance()->barFillColor());

          curve->writeLock();
          curve->registerChange();
          curve->unlock();

          _filterFitTab->curveAppearance()->setWidgetDefaults();

          PlotRenderItem *renderItem = plotItem->renderItem(PlotRenderItem::Cartesian);
          renderItem->addRelation(kst_cast<Relation>(curve));

          dataObject->writeLock();
          dataObject->internalUpdate();
          dataObject->unlock();

          if (dataObject->hasParameterVector()) {
              CreateLabelCommand *cmd = new CreateLabelCommand;
              QString *tmpstring = new QString(dataObject->parameterVectorToString());

              cmd->createItem(tmpstring);
          }
          plotItem->update();

      }
  }

  _filterFitTab->configWidget()->save();
  UpdateManager::self()->doUpdates(true);

  return dataObject;
}


ObjectPtr FilterFitDialog::editExistingDataObject() const {
  if (BasicPlugin* plugin = kst_cast<BasicPlugin>(dataObject())) {
    plugin->writeLock();
    plugin->change(_filterFitTab->configWidget());
    if (DataDialog::tagStringAuto()) {
       plugin->setDescriptiveName(QString());
    } else {
       plugin->setDescriptiveName(DataDialog::tagString());
    }
    plugin->registerChange();
    plugin->unlock();
  }
  UpdateManager::self()->doUpdates(true);
  return dataObject();
}

}

// vim: ts=2 sw=2 et
