/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2016 C. Barth Netterfield                             *
 *                   netterfield@astro.utoronto.ca                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "filtermultipledialog.h"
#include "ui_filtermultipledialog.h"

#include "objectstore.h"
#include "mainwindow.h"
#include "application.h"
#include "document.h"
#include "basicplugin.h"
#include "updatemanager.h"
#include "updateserver.h"
#include "curve.h"
#include "plotiteminterface.h"
#include "plotitem.h"
#include "datacollection.h"
#include "geticon.h"
#include "psd.h"
#include "histogram.h"

#include <QMessageBox>

namespace Kst {

FilterMultipleDialog::FilterMultipleDialog(QWidget *parent) :
  QDialog(parent), ui(new Ui::FilterMultipleDialog),
  _configWidget(0), _layout(0), _store(0)
{
  if (MainWindow *mw = qobject_cast<MainWindow*>(parent)) {
    _store = mw->document()->objectStore();
  } else {
     // FIXME: we need the object store
    qFatal("ERROR: can't construct a FilterMultipleDialog without the object store");
  }

  ui->setupUi(this);

  ui->_add->setIcon(KstGetIcon("kst_rightarrow"));
  ui->_addAll->setIcon(KstGetIcon("kst_rightarrow"));
  ui->_timestreams->setIcon(KstGetIcon("kst_rightarrow"));
  ui->_remove->setIcon(KstGetIcon("kst_leftarrow"));
  ui->_removeAll->setIcon(KstGetIcon("kst_leftarrow"));

  ui->_pluginCombo->addItems(DataObject::filterPluginList());

  connect(ui->_add, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
  connect(ui->_timestreams, SIGNAL(clicked()), this, SLOT(smartButtonClicked()));
  connect(ui->_remove, SIGNAL(clicked()), this, SLOT(removeButtonClicked()));
  connect(ui->_addAll, SIGNAL(clicked()), this, SLOT(addAll()));
  connect(ui->_removeAll, SIGNAL(clicked()), this, SLOT(removeAll()));

  connect(ui->_selectedVectors, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));
  connect(ui->_availableVectors, SIGNAL(itemSelectionChanged()), this, SLOT(updateButtons()));

  connect(ui->_availableVectors, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(availableDoubleClicked(QListWidgetItem*)));
  connect(ui->_selectedVectors, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(selectedDoubleClicked(QListWidgetItem*)));

  connect(ui->_pluginCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(pluginChanged(QString)));

  connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
  connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(OKClicked()));

  pluginChanged();
}

FilterMultipleDialog::~FilterMultipleDialog()
{
  delete ui;
}

void FilterMultipleDialog::show() {
  updateVectorLists();
  QDialog::show();
}

void FilterMultipleDialog::updateVectorLists() {
  VectorList vectors = _store->getObjects<Vector>();

  // make sure all items in ui->_availableVectors exist in the store; remove if they don't.
  for (int i_item = 0; i_item < ui->_availableVectors->count(); i_item++) {
    bool exists=false;
    for (int i_vector = 0; i_vector<vectors.count(); i_vector++) {
      if (vectors.at(i_vector)->Name() == ui->_availableVectors->item(i_item)->text()) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      QListWidgetItem *item = ui->_availableVectors->takeItem(i_item);
      delete item;
      i_item--;
    }
  }

  // make sure all items in ui->ui->_selectedVectors exist in the store; remove if they don't.
  for (int i_item = 0; i_item < ui->_selectedVectors->count(); i_item++) {
    bool exists=false;
    for (int i_primitive = 0; i_primitive<vectors.count(); i_primitive++) {
      if (vectors.at(i_primitive)->Name() == ui->_selectedVectors->item(i_item)->text()) {
        exists = true;
        break;
      }
    }
    if (!exists) {
      QListWidgetItem *item = ui->_selectedVectors->takeItem(i_item);
      delete item;
      i_item--;
    }
  }

  // insert into ui->_availableVectors all items in store not in one of the lists.
  for (int i_primitive = 0; i_primitive<vectors.count(); i_primitive++) {
    bool listed = false;
    for (int i_item = 0; i_item<ui->_availableVectors->count(); i_item++) {
      if (vectors.at(i_primitive)->Name() == ui->_availableVectors->item(i_item)->text()) {
        ui->_availableVectors->item(i_item)->setToolTip(vectors.at(i_primitive)->descriptionTip());
        listed = true;
        break;
      }
    }
    for (int i_item = 0; i_item<ui->_selectedVectors->count(); i_item++) {
      if (vectors.at(i_primitive)->Name() == ui->_selectedVectors->item(i_item)->text()) {
        ui->_selectedVectors->item(i_item)->setToolTip(vectors.at(i_primitive)->descriptionTip());
        listed = true;
        break;
      }
    }
    if (!listed) {
      QListWidgetItem *wi = new QListWidgetItem(vectors.at(i_primitive)->Name());
      ui->_availableVectors->addItem(wi);
      wi->setToolTip(vectors.at(i_primitive)->descriptionTip());
    }
  }

  updateButtons();
}


void FilterMultipleDialog::updateButtons() {
  bool valid = ui->_selectedVectors->count() > 0; // fixme: check valid plugin
  ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
  ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(valid);
  ui->_add->setEnabled(ui->_availableVectors->selectedItems().count() > 0);
  ui->_addAll->setEnabled(ui->_availableVectors->count() > 0);
  ui->_remove->setEnabled(ui->_selectedVectors->selectedItems().count() > 0);
  ui->_removeAll->setEnabled(ui->_selectedVectors->count() > 0);
}


void FilterMultipleDialog::addButtonClicked() {
  foreach (QListWidgetItem* item, ui->_availableVectors->selectedItems()) {
    ui->_selectedVectors->addItem(ui->_availableVectors->takeItem(ui->_availableVectors->row(item)));
  }
  ui->_availableVectors->clearSelection();
  updateButtons();
}


void FilterMultipleDialog::smartButtonClicked() {
  QList<CurvePtr> curves = _store->getObjects<Curve>();
  foreach (CurvePtr curve, curves) {
    bool is_timestream = true;
    if (kst_cast<PSD>(curve->yVector()->provider())) {
      is_timestream = false;
    }
    if (kst_cast<Histogram>(curve->yVector()->provider())) {
      is_timestream = false;
    }
    if (is_timestream) {
      QList<QListWidgetItem *> vec_items = ui->_availableVectors->findItems(curve->yVector()->Name(), Qt::MatchExactly);
      if (vec_items.size()>0) {
        ui->_selectedVectors->addItem(ui->_availableVectors->takeItem(ui->_availableVectors->row(vec_items[0])));
      }
    }
  }
  updateButtons();
}


void FilterMultipleDialog::removeButtonClicked() {
  foreach (QListWidgetItem* item, ui->_selectedVectors->selectedItems()) {
    ui->_availableVectors->addItem(ui->_selectedVectors->takeItem(ui->_selectedVectors->row(item)));
  }

  ui->_availableVectors->clearSelection();
  updateButtons();
}


void FilterMultipleDialog::addAll() {
  ui->_availableVectors->selectAll();
  addButtonClicked();
}


void FilterMultipleDialog::removeAll() {
  ui->_selectedVectors->selectAll();
  removeButtonClicked();
}


void FilterMultipleDialog::availableDoubleClicked(QListWidgetItem * item) {
  if (item) {
    ui->_selectedVectors->addItem(ui->_availableVectors->takeItem(ui->_availableVectors->row(item)));
    ui->_selectedVectors->clearSelection();
    updateButtons();
  }
}

void FilterMultipleDialog::selectedDoubleClicked(QListWidgetItem * item) {
  if (item) {
    ui->_availableVectors->addItem(ui->_selectedVectors->takeItem(ui->_selectedVectors->row(item)));
    ui->_availableVectors->clearSelection();
    updateButtons();
  }
}

void FilterMultipleDialog::OKClicked() {
  apply();
  accept();
}


void FilterMultipleDialog::apply() {
  ui->_selectedVectors->selectAll();
  QList<QListWidgetItem*> selectedItems = ui->_selectedVectors->selectedItems();

  QList<CurvePtr> curves = _store->getObjects<Curve>();

  foreach (QListWidgetItem *vec_item, selectedItems) {
    VectorPtr vector = kst_cast<Vector>(_store->retrieveObject(vec_item->text()));
    if (vector) {
      _configWidget->setVectorY(vector);
      BasicPluginPtr dataObject = kst_cast<BasicPlugin>(
            DataObject::createPlugin(ui->_pluginCombo->currentText(),
                                     _store,
                                     _configWidget));
      Q_ASSERT(dataObject);

      if (!dataObject->isValid()) {
        _store->removeObject(dataObject);
        QString msg(tr("Unable to create Plugin Object using provided parameters.\n\n"));
        msg += dataObject->errorMessage();
        QMessageBox::warning(this, tr("Kst"), msg);

        return;
      }
      VectorPtr yVector = dataObject->outputVectors().value(dataObject->outputVectorList().first());
      if (ui->_replaceCurves->isChecked()) {
        foreach (CurvePtr curve, curves) {
          if (curve->xVector()->shortName() == vector->shortName()) {
            curve->setXVector(yVector);
          }
          if (curve->yVector()->shortName() == vector->shortName()) {
            curve->setYVector(yVector);
          }
        }
      } else if (ui->_copyCurves->isChecked()) {
        foreach (CurvePtr curve, curves) {
          if (curve->yVector()->shortName() == vector->shortName()) {
            CurvePtr new_curve = _store->createObject<Curve>();
            new_curve->setXVector(curve->xVector());
            new_curve->setYVector(yVector);

            // plot it
            QList<PlotItem *> plots = ViewItem::getItems<PlotItem>();
            foreach (PlotItemInterface *plot, plots) {
              PlotItem* plotItem = static_cast<PlotItem*>(plot);
              foreach (PlotRenderItem* renderItem, plotItem->renderItems()) {
                if (renderItem->relationList().contains(curve)) {
                  renderItem->addRelation(new_curve);
                }
              }
            }
          }
        }
      }
    }
  }
  UpdateManager::self()->doUpdates();
  UpdateServer::self()->requestUpdateSignal();

  kstApp->mainWindow()->document()->setChanged(true);
  _configWidget->save();

}


void FilterMultipleDialog::pluginChanged(QString plugin) {
  if (plugin.isEmpty()) {
    int index = ui->_pluginCombo->currentIndex();
    if (index < 0) {
      ui->_pluginCombo->setCurrentIndex(0);
    }
    plugin = ui->_pluginCombo->currentText();
  }

  if (plugin != ui->_pluginCombo->currentText()) {
    ui->_pluginCombo->setCurrentIndex(ui->_pluginCombo->findText(plugin));
  }

  ui->_descriptionLabel->setText(DataObject::pluginDescription(plugin));

  if (_layout) {
    delete _layout;
  }

  if (_configWidget) {
    delete _configWidget;
  }
  _layout = new QGridLayout(ui->_inputOutputBox);
  _configWidget = DataObject::pluginWidget(plugin);

  _configWidget->setupSlots(this);
  if (_store) {
    _configWidget->setObjectStore(_store);
  }
  _configWidget->load();
  _configWidget->setVectorsLocked();
  _layout->addWidget(_configWidget, 0, 0);
  _layout->activate();

}




}
