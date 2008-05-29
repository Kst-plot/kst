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
#include "editmultiplewidget.h"

#include "datacollection.h"
#include "dataobjectcollection.h"
#include "document.h"
#include "defaultnames.h"
#include "objectstore.h"

#include "eventmonitorentry.h"

namespace Kst {

EventMonitorTab::EventMonitorTab(QWidget *parent)
  : DataTab(parent), _logLevelDirty(false) {

  setupUi(this);
  setTabTitle(tr("Event Monitor"));

  connect(_equation, SIGNAL(textChanged(const QString &)), this, SLOT(selectionChanged()));

  connect(_debugLog, SIGNAL(toggled(const bool&)), this, SIGNAL(modified()));
  connect(_emailNotify, SIGNAL(toggled(const bool&)), this, SIGNAL(modified()));
  connect(_ELOGNotify, SIGNAL(toggled(const bool&)), this, SIGNAL(modified()));
  connect(_executeScript, SIGNAL(toggled(const bool&)), this, SIGNAL(modified()));

  connect(_equation, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_description, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_emailRecipients, SIGNAL(textChanged(const QString&)), this, SIGNAL(modified()));
  connect(_script, SIGNAL(textChanged()), this, SIGNAL(modified()));

  connect(_debugLogNotice, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_debugLogWarning, SIGNAL(clicked()), this, SIGNAL(modified()));
  connect(_debugLogError, SIGNAL(clicked()), this, SIGNAL(modified()));

  connect(_debugLogNotice, SIGNAL(clicked()), this, SLOT(logLevelChanged()));
  connect(_debugLogWarning, SIGNAL(clicked()), this, SLOT(logLevelChanged()));
  connect(_debugLogError, SIGNAL(clicked()), this, SLOT(logLevelChanged()));
}


EventMonitorTab::~EventMonitorTab() {
}


void EventMonitorTab::selectionChanged() {
  emit optionsChanged();
}


void EventMonitorTab::logLevelChanged() {
  _logLevelDirty = true;
}


void EventMonitorTab::resetLogLevelDirty() {
  _logLevelDirty = false;
}


QString EventMonitorTab::script() const {
  return _script->toPlainText();
}


bool EventMonitorTab::scriptDirty() const {
  return !_script->toPlainText().isEmpty();
}


void EventMonitorTab::setScript(const QString script) {
  return _script->setText(script);
}


QString EventMonitorTab::event() const {
  return _equation->text();
}


bool EventMonitorTab::eventDirty() const {
  return !_equation->text().isEmpty();
}


void EventMonitorTab::setEvent(const QString event) {
  return _equation->setText(event);
}


QString EventMonitorTab::description() const {
  return _description->text();
}


bool EventMonitorTab::descriptionDirty() const {
  return !_description->text().isEmpty();
}


void EventMonitorTab::setDescription(const QString description) {
  return _description->setText(description);
}


QString EventMonitorTab::emailRecipients() const {
  return _emailRecipients->text();
}


bool EventMonitorTab::emailRecipientsDirty() const {
  return !_emailRecipients->text().isEmpty();
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


bool EventMonitorTab::logLevelDirty() const {
  return _logLevelDirty;
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
    default:
      break;
  }
  resetLogLevelDirty();
}


bool EventMonitorTab::logDebug() const {
  return _debugLog->isChecked();
}


bool EventMonitorTab::logDebugDirty() const {
  return _debugLog->checkState() != Qt::PartiallyChecked;
}


void EventMonitorTab::setLogDebug(const bool logDebug) {
  return _debugLog->setChecked(logDebug);
}


bool EventMonitorTab::logEMail() const {
  return _emailNotify->isChecked();
}


bool EventMonitorTab::logEMailDirty() const {
  return _emailNotify->checkState() != Qt::PartiallyChecked;
}


void EventMonitorTab::setLogEMail(const bool logEMail) {
  return _emailNotify->setChecked(logEMail);
}


bool EventMonitorTab::logELOG() const {
  return _ELOGNotify->isChecked();
}


bool EventMonitorTab::logELOGDirty() const {
  return _ELOGNotify->checkState() != Qt::PartiallyChecked;
}


void EventMonitorTab::setLogELOG(const bool logELOG) {
  return _ELOGNotify->setChecked(logELOG);
}


void EventMonitorTab::setObjectStore(ObjectStore *store) {
  _vectorSelector->setObjectStore(store);
  _scalarSelector->setObjectStore(store);
}


void EventMonitorTab::clearTabValues() {
  _equation->clear();
  _emailRecipients->clear();
  _description->clear();
  _script->clear();
  _executeScript->setCheckState(Qt::PartiallyChecked);
  _ELOGNotify->setCheckState(Qt::PartiallyChecked);
  _emailNotify->setCheckState(Qt::PartiallyChecked);
  _debugLog->setCheckState(Qt::PartiallyChecked);
  resetLogLevelDirty();
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
  connect(this, SIGNAL(editMultipleMode()), this, SLOT(editMultipleMode()));
  connect(this, SIGNAL(editSingleMode()), this, SLOT(editSingleMode()));

  connect(_eventMonitorTab, SIGNAL(modified()), this, SLOT(modified()));
  updateButtons();
}


EventMonitorDialog::~EventMonitorDialog() {
}


// QString EventMonitorDialog::tagString() const {
//   return DataDialog::tagString();
// }


void EventMonitorDialog::editMultipleMode() {
  _eventMonitorTab->clearTabValues();
}


void EventMonitorDialog::editSingleMode() {
   configureTab(dataObject());
}


void EventMonitorDialog::configureTab(ObjectPtr object) {
  if (EventMonitorEntryPtr eventMonitorEntry = kst_cast<EventMonitorEntry>(object)) {
    _eventMonitorTab->setScript(eventMonitorEntry->scriptCode());
    _eventMonitorTab->setEvent(eventMonitorEntry->event());
    _eventMonitorTab->setDescription(eventMonitorEntry->description());
    _eventMonitorTab->setLogLevel(eventMonitorEntry->level());
    _eventMonitorTab->setLogDebug(eventMonitorEntry->logDebug());
    _eventMonitorTab->setLogEMail(eventMonitorEntry->logEMail());
    _eventMonitorTab->setLogELOG(eventMonitorEntry->logELOG());
    _eventMonitorTab->setEmailRecipients(eventMonitorEntry->eMailRecipients());
    if (_editMultipleWidget) {
      QStringList objectList;
      EventMonitorEntryList objects = _document->objectStore()->getObjects<EventMonitorEntry>();
      foreach(EventMonitorEntryPtr object, objects) {
        objectList.append(object->Name());
      }
      _editMultipleWidget->addObjects(objectList);
    }
  }
}


void EventMonitorDialog::updateButtons() {
  _buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!_eventMonitorTab->event().isEmpty() || (editMode() == EditMultiple));
}


ObjectPtr EventMonitorDialog::createNewDataObject() const {
  Q_ASSERT(_document && _document->objectStore());
  EventMonitorEntryPtr eventMonitor = _document->objectStore()->createObject<EventMonitorEntry>();

  eventMonitor->setScriptCode(_eventMonitorTab->script());
  eventMonitor->setEvent(_eventMonitorTab->event());
  eventMonitor->setDescription(_eventMonitorTab->description());
  eventMonitor->setLevel(_eventMonitorTab->logLevel());
  eventMonitor->setLogDebug(_eventMonitorTab->logDebug());
  eventMonitor->setLogEMail(_eventMonitorTab->logEMail());
  eventMonitor->setLogELOG(_eventMonitorTab->logELOG());
  eventMonitor->setEMailRecipients(_eventMonitorTab->emailRecipients());

  eventMonitor->reparse();

  eventMonitor->writeLock();
  eventMonitor->update();
  eventMonitor->unlock();

  return ObjectPtr(eventMonitor.data());
}


ObjectPtr EventMonitorDialog::editExistingDataObject() const {
  if (EventMonitorEntryPtr eventMonitor = kst_cast<EventMonitorEntry>(dataObject())) {
    if (editMode() == EditMultiple) {
      QStringList objects = _editMultipleWidget->selectedObjects();
      foreach (QString objectName, objects) {
        EventMonitorEntryPtr eventMonitor = kst_cast<EventMonitorEntry>(_document->objectStore()->retrieveObject(objectName));
        if (eventMonitor) {
          const QString script = _eventMonitorTab->scriptDirty() ? _eventMonitorTab->script() : eventMonitor->scriptCode();
          const QString event = _eventMonitorTab->eventDirty() ? _eventMonitorTab->event() : eventMonitor->event();
          const QString description = _eventMonitorTab->descriptionDirty() ? _eventMonitorTab->description() : eventMonitor->description();
          const QString emailRecipients = _eventMonitorTab->emailRecipientsDirty() ? _eventMonitorTab->emailRecipients() : eventMonitor->eMailRecipients();
          const Debug::LogLevel logLevel = _eventMonitorTab->logLevelDirty() ?  _eventMonitorTab->logLevel() : eventMonitor->level();
          const bool logDebug = _eventMonitorTab->logDebugDirty() ?  _eventMonitorTab->logDebug() : eventMonitor->logDebug();
          const bool logEMail = _eventMonitorTab->logEMailDirty() ?  _eventMonitorTab->logEMail() : eventMonitor->logEMail();
          const bool logELOG = _eventMonitorTab->logELOGDirty() ?  _eventMonitorTab->logELOG() : eventMonitor->logELOG();

          eventMonitor->writeLock();
          eventMonitor->setScriptCode(script);
          eventMonitor->setEvent(event);
          eventMonitor->setDescription(description);
          eventMonitor->setLevel(logLevel);
          eventMonitor->setLogDebug(logDebug);
          eventMonitor->setLogEMail(logEMail);
          eventMonitor->setLogELOG(logELOG);
          eventMonitor->setEMailRecipients(emailRecipients);

          eventMonitor->reparse();
          eventMonitor->update();
          eventMonitor->unlock();
        }
      }
    } else {
      eventMonitor->writeLock();
      eventMonitor->setScriptCode(_eventMonitorTab->script());
      eventMonitor->setEvent(_eventMonitorTab->event());
      eventMonitor->setDescription(_eventMonitorTab->description());
      eventMonitor->setLevel(_eventMonitorTab->logLevel());
      eventMonitor->setLogDebug(_eventMonitorTab->logDebug());
      eventMonitor->setLogEMail(_eventMonitorTab->logEMail());
      eventMonitor->setLogELOG(_eventMonitorTab->logELOG());
      eventMonitor->setEMailRecipients(_eventMonitorTab->emailRecipients());

      eventMonitor->reparse();

      eventMonitor->update();
      eventMonitor->unlock();
    }
  }
  return dataObject();}

}

// vim: ts=2 sw=2 et
