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
#include "document.h"
#include "defaultnames.h"
#include "objectstore.h"

#include "eventmonitorentry.h"

namespace Kst {

EventMonitorTab::EventMonitorTab(QWidget *parent)
  : DataTab(parent) {

  setupUi(this);
  setTabTitle(tr("Event Monitor"));
}


EventMonitorTab::~EventMonitorTab() {
}


QString EventMonitorTab::script() const {
  return _script->toPlainText();
}


QString EventMonitorTab::event() const {
  return _equation->text();
}


QString EventMonitorTab::description() const {
  return _description->text();
}


QString EventMonitorTab::emailRecipients() const {
  return _emailRecipients->text();
}


Debug::LogLevel EventMonitorTab::logLevel() const {
  if (_debugLogNotice->isChecked()) {
    return Debug::Notice;
  } else if (_debugLogWarning->isChecked()) {
    return Debug::Warning;
  } else {
    return Debug::Error;
  }
}


bool EventMonitorTab::logKstDebug() const {
  return _debugLog->isChecked();
}


bool EventMonitorTab::logEMail() const {
  return _emailNotify->isChecked();
}


bool EventMonitorTab::logELOG() const {
  return _ELOGNotify->isChecked();
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


QString EventMonitorDialog::tagString() const {
  return DataDialog::tagString();
}


ObjectPtr EventMonitorDialog::createNewDataObject() const {
  Q_ASSERT(_document && _document->objectStore());
  EventMonitorEntryPtr eventMonitor = _document->objectStore()->createObject<EventMonitorEntry>(ObjectTag::fromString(tagString()));

  eventMonitor->setScriptCode(_eventMonitorTab->script());
  eventMonitor->setEvent(_eventMonitorTab->event());
  eventMonitor->setDescription(_eventMonitorTab->description());
  eventMonitor->setLevel(_eventMonitorTab->logLevel());
  eventMonitor->setLogKstDebug(_eventMonitorTab->logKstDebug());
  eventMonitor->setLogEMail(_eventMonitorTab->logEMail());
  eventMonitor->setLogELOG(_eventMonitorTab->logELOG());
  eventMonitor->setEMailRecipients(_eventMonitorTab->emailRecipients());

  eventMonitor->reparse();

  eventMonitor->writeLock();
  eventMonitor->update(0);
  eventMonitor->unlock();

  return ObjectPtr(eventMonitor.data());
}


ObjectPtr EventMonitorDialog::editExistingDataObject() const {
  qDebug() << "editExistingDataObject" << endl;
  return 0;
}

}

// vim: ts=2 sw=2 et
