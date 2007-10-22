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

#include "eventmonitordialog.h"

#include "dialogpage.h"

#include "datacollection.h"
#include "dataobjectcollection.h"
#include "defaultnames.h"

namespace Kst {

EventMonitorTab::EventMonitorTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Event Monitor"));
}


EventMonitorTab::~EventMonitorTab() {
}


EventMonitorDialog::EventMonitorDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Event Monitor"));
  else
    setWindowTitle(tr("New Event Monitor"));

  _eventMonitorTab = new EventMonitorTab(this);
  addDataTab(_eventMonitorTab);

  //FIXME need to do validation to enable/disable ok button...
}


EventMonitorDialog::~EventMonitorDialog() {
}


QString EventMonitorDialog::tagName() const {
  return DataDialog::tagName();
}


ObjectPtr EventMonitorDialog::createNewDataObject() const {
  qDebug() << "createNewDataObject" << endl;
  return 0;
}


ObjectPtr EventMonitorDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
