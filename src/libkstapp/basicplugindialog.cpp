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

#include "basicplugindialog.h"

#include "dialogpage.h"

#include "objectstore.h"
#include "datacollection.h"
#include "document.h"
#include "basicplugin.h"

#include <QMessageBox>

namespace Kst {

BasicPluginTab::BasicPluginTab(QString& pluginName, QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Basic Plugin"));

  QGridLayout *layout = new QGridLayout(_inputOutputBox);
  _configWidget = DataObject::pluginWidget(pluginName);
  _configWidget->setupSlots(this);
  layout->addWidget(_configWidget, 0, 0);
  layout->activate();
}


BasicPluginTab::~BasicPluginTab() {
}


void BasicPluginTab::setObjectStore(ObjectStore *store) {
   _configWidget->setObjectStore(store);
}


BasicPluginDialog::BasicPluginDialog(QString& pluginName, ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent), _pluginName(pluginName) {

  QString title;
  if (editMode() == Edit)
    title = tr("Edit ") + pluginName + tr(" Plugin");
  else
    title = tr("New ") + pluginName + tr(" Plugin");
  setWindowTitle(title);

  _basicPluginTab = new BasicPluginTab(pluginName, this);
  addDataTab(_basicPluginTab);

  if (dataObject) {
    _basicPluginTab->configWidget()->setupFromObject(dataObject);
  } else {
    _basicPluginTab->configWidget()->load();
  }
}


BasicPluginDialog::~BasicPluginDialog() {
}


QString BasicPluginDialog::tagString() const {
  return DataDialog::tagString();
}


ObjectPtr BasicPluginDialog::createNewDataObject() {
  BasicPluginPtr dataObject = kst_cast<BasicPlugin>(DataObject::createPlugin(_pluginName, _document->objectStore(), _basicPluginTab->configWidget()));

  _basicPluginTab->configWidget()->save();

  if (!dataObject->isValid()) {
    _document->objectStore()->removeObject(dataObject);
    QString msg(tr("Unable to create Plugin Object using provided parameters.\n\n"));
    msg += dataObject->errorMessage();
    QMessageBox::warning(this, tr("Kst"), msg);

    return 0;
  }

  return dataObject;
}


ObjectPtr BasicPluginDialog::editExistingDataObject() const {
  if (BasicPlugin* plugin = kst_cast<BasicPlugin>(dataObject())) {
    plugin->writeLock();
    plugin->change(_basicPluginTab->configWidget());
    plugin->registerChange();
    plugin->unlock();
  }
  return dataObject();
}

}

// vim: ts=2 sw=2 et
