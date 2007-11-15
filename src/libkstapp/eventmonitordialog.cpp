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
  connect(_equation, SIGNAL(textChanged(const QString &)), this, SLOT(selectionChanged()));
}


EventMonitorTab::~EventMonitorTab() {
}


void EventMonitorTab::selectionChanged() {
  emit optionsChanged();
}


QString EventMonitorTab::script() const {
  return _script->toPlainText();
}


void EventMonitorTab::setScript(const QString script) {
  return _script->setText(script);
}


QString EventMonitorTab::event() const {
  return _equation->text();
}


void EventMonitorTab::setEvent(const QString event) {
  return _equation->setText(event);
}


QString EventMonitorTab::description() const {
  return _description->text();
}


void EventMonitorTab::setDescription(const QString description) {
  return _description->setText(description);
}


QString EventMonitorTab::emailRecipients() const {
  return _emailRecipients->text();
}


void EventMonitorTab::setEmailRecipients(const QString emailRecipients) {
  return _emailRecipients->setText(emailRecipients);
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


void EventMonitorTab::setLogLevel(const Debug::LogLevel logLevel) {
  switch (logLevel) {
    case Debug::Notice:
      _debugLogNotice->setChecked(true);
      break;
    case Debug::Warning:
      _debugLogWarning->setChecked(true);
      break;
    case Debug::Error:
      _debugLogError->setChecked(true);
      break;
  }
}


bool EventMonitorTab::logKstDebug() const {
  return _debugLog->isChecked();
}


void EventMonitorTab::setLogKstDebug(const bool logKstDebug) {
  return _debugLog->setChecked(logKstDebug);
}


bool EventMonitorTab::logEMail() const {
  return _emailNotify->isChecked();
}


void EventMonitorTab::setLogEMail(const bool logEMail) {
  return _emailNotify->setChecked(logEMail);
}


bool EventMonitorTab::logELOG() const {
  return _ELOGNotify->isChecked();
}


void EventMonitorTab::setLogELOG(const bool logELOG) {
  return _ELOGNotify->setChecked(logELOG);
}


void EventMonitorTab::setObjectStore(ObjectStore *store) {
  _vectorSelector->setObjectStore(store);
  _scalarSelector->setObjectStore(store);
}


EventMonitorDialog::EventMonitorDialog(ObjectPtr dataObject, QWidget *parent)
  : DataDialog(dataObject, parent) {

  if (editMode() == Edit)
    setWindowTitle(tr("Edit Event Monitor"));
  else
    setWindowTitle(tr("New Event Monitor"));

  _eventMonitorTab = new EventMonitorTab(this);
  addDataTab(_eventMonitorTab);

  if (editMode() == Edit) {
    configureTab(dataObject);
  }

  connect(_eventMonitorTab, SIGNAL(optionsChanged()), this, SLOT(updateButtons()));
  updateButtons();
}


EventMonitorDialog::~EventMonitorDialog() {
}


QString EventMonitorDialog::tagString() const {
  return DataDialog::tagString();
}


void EventMonitorDialog::configureTab(ObjectPtr object) {
  if (EventMonitorEntryPtr eventMonitorEntry = kst_cast<EventMonitorEntry>(object)) {
    _eventMonitorTab->setScript(eventMonitorEntry->scriptCode());
    _eventMonitorTab->setEvent(eventMonitorEntry->event());
    _eventMonitorTab->setDescription(eventMonitorEntry->description());
    _eventMonitorTab->setLogLevel(eventMonitorEntry->level());
    _eventMonitorTab->setLogKstDebug(eventMonitorEntry->logKstDebug());
    _eventMonitorTab->setLogEMail(eventMonitorEntry->logEMail());
    _eventMonitorTab->setLogELOG(eventMonitorEntry->logELOG());
    _eventMonitorTab->setEmailRecipients(eventMonitorEntry->eMailRecipients());
  }
}


void EventMonitorDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!_eventMonitorTab->event().isEmpty());
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
  if (EventMonitorEntryPtr eventMonitor = kst_cast<EventMonitorEntry>(dataObject())) {
    eventMonitor->writeLock();
    eventMonitor->setScriptCode(_eventMonitorTab->script());
    eventMonitor->setEvent(_eventMonitorTab->event());
    eventMonitor->setDescription(_eventMonitorTab->description());
    eventMonitor->setLevel(_eventMonitorTab->logLevel());
    eventMonitor->setLogKstDebug(_eventMonitorTab->logKstDebug());
    eventMonitor->setLogEMail(_eventMonitorTab->logEMail());
    eventMonitor->setLogELOG(_eventMonitorTab->logELOG());
    eventMonitor->setEMailRecipients(_eventMonitorTab->emailRecipients());

    eventMonitor->reparse();

    eventMonitor->update(0);
    eventMonitor->unlock();
  }
  return dataObject();}

}

// vim: ts=2 sw=2 et
