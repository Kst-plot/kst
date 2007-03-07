/***************************************************************************
                    ksteventmonitor.cpp  -  Part of KST
                             -------------------
    begin                :
    copyright            : (C) 2004 The University of British Columbia
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include files for Qt
#include <qcheckbox.h>
#include <qlineedit.h>
#include <q3listbox.h>
#include <qradiobutton.h>
#include <q3textedit.h>
#include <q3vbox.h>

// include files for KDE
#include <klocale.h>
#include <kmessagebox.h>

// application specific includes
#include "editmultiplewidget.h"
#include "kst.h"
#include "kstdataobjectcollection.h"
#include "ksteventmonitor.h"
#include "ksteventmonitorentry.h"
#include "scalarselector.h"
#include "vectorselector.h"

QPointer<KstEventMonitorI> KstEventMonitorI::_inst;

KstEventMonitorI* KstEventMonitorI::globalInstance() {
  if (!_inst) {
    _inst = new KstEventMonitorI(KstApp::inst());
  }

  return _inst;
}


KstEventMonitorI::KstEventMonitorI(QWidget* parent, Qt::WindowFlags fl)
: KstDataDialog(parent, fl) {

  _w = new Ui::KstEventMonitor;
  _w->setupUi(_contents);

  setMultiple(true);
  connect(_w->_vectorSelectorEq, SIGNAL(newVectorCreated(const QString&)), this, SIGNAL(modified()));
  connect(_w->_scalarSelectorEq, SIGNAL(newScalarCreated()), this, SIGNAL(modified()));
  connect(_w->_vectorSelectorEq, SIGNAL(selectionChangedLabel(const QString&)), _w->lineEditEquation, SLOT(insert(const QString&)));
  connect(_w->_vectorSelectorEq, SIGNAL(selectionChangedLabel(const QString&)), _w->lineEditEquation, SLOT(setFocus()));
  connect(_w->_scalarSelectorEq, SIGNAL(selectionChangedLabel(const QString&)), _w->lineEditEquation, SLOT(insert(const QString&)));
  connect(_w->_scalarSelectorEq, SIGNAL(selectionChangedLabel(const QString&)), _w->lineEditEquation, SLOT(setFocus()));
  connect(_w->_pushButtonELOGConfigure, SIGNAL(clicked()), KstApp::inst(), SLOT(EventELOGConfigure()));
  
  // more multiple edit mode
  connect(_w->checkBoxDebug, SIGNAL(clicked()), this, SLOT(setcheckBoxDebugDirty()));
  connect(_w->checkBoxEMailNotify, SIGNAL(clicked()), this, SLOT(setcheckBoxEMailNotifyDirty()));
  connect(_w->checkBoxELOGNotify, SIGNAL(clicked()), this, SLOT(setcheckBoxELOGNotifyDirty()));
  connect(_w->_useScript, SIGNAL(clicked()), this, SLOT(setScriptDirty()));
  connect(_w->_script, SIGNAL(textChanged()), this, SLOT(setScriptDirty()));

  setFixedHeight(height());
}


KstEventMonitorI::~KstEventMonitorI() {
  delete _w;
}


void KstEventMonitorI::fillFieldsForEdit() {
  EventMonitorEntryPtr ep = kst_cast<EventMonitorEntry>(_dp);
  if (!ep) {
    return; // shouldn't be needed
  }
  ep->readLock();
  _tagName->setText(ep->tagName());

  _w->lineEditEquation->setText(ep->event());
  _w->lineEditDescription->setText(ep->description());
  _w->checkBoxDebug->setChecked(ep->logKstDebug());
  _w->checkBoxEMailNotify->setChecked(ep->logEMail());
  _w->checkBoxELOGNotify->setChecked(ep->logELOG());
  _w->lineEditEMailRecipients->setText(ep->eMailRecipients());
  _w->_useScript->setEnabled(!ep->scriptCode().isEmpty());
  _w->_script->setText(ep->scriptCode());

  switch (ep->level()) {
    case KstDebug::Notice:
      _w->radioButtonLogNotice->setChecked(true);
      break;
    case KstDebug::Warning:
      _w->radioButtonLogWarning->setChecked(true);
      break;
    case KstDebug::Error:
      _w->radioButtonLogError->setChecked(true);
      break;
    default:
      _w->radioButtonLogWarning->setChecked(true);
      break;
  }

  ep->unlock();
  adjustSize();
  resize(minimumSizeHint());
  setFixedHeight(height());
}


void KstEventMonitorI::fillFieldsForNew() {
  KstEventMonitorEntryList events = kstObjectSubList<KstDataObject, EventMonitorEntry>(KST::dataObjectList);

  QString new_label = QString("E%1-").arg(events.count() + 1) + "<New_Event>";
  _tagName->setText(new_label);

  _w->radioButtonLogWarning->setChecked(true);
  _w->lineEditEquation->setText(QString::null);
  _w->lineEditDescription->setText(QString::null);
  _w->checkBoxDebug->setChecked(true);
  _w->checkBoxEMailNotify->setChecked(false);
  _w->checkBoxELOGNotify->setChecked(false);
  _w->lineEditEMailRecipients->setText(QString::null);
  _w->_useScript->setChecked(false);
  _w->_script->setText(QString::null);
  adjustSize();
  resize(minimumSizeHint());
  setFixedHeight(height());
}


void KstEventMonitorI::update() {
  _w->_vectorSelectorEq->update();
  _w->_scalarSelectorEq->update();
}


void KstEventMonitorI::fillEvent(EventMonitorEntryPtr& event) {
  event->setEvent(_w->lineEditEquation->text());
  event->setDescription(_w->lineEditDescription->text());
  event->setLogKstDebug(_w->checkBoxDebug->isChecked());
  event->setLogEMail(_w->checkBoxEMailNotify->isChecked());
  event->setLogELOG(_w->checkBoxELOGNotify->isChecked());
  event->setEMailRecipients(_w->lineEditEMailRecipients->text());
  event->setScriptCode(_w->_useScript->isChecked() ? _w->_script->text() : QString::null);

  if (_w->radioButtonLogNotice->isChecked()) {
    event->setLevel(KstDebug::Notice);
  } else if (_w->radioButtonLogWarning->isChecked()) {
    event->setLevel(KstDebug::Warning);
  } else if (_w->radioButtonLogError->isChecked()) {
    event->setLevel(KstDebug::Error);
  }

  event->reparse();
}


void KstEventMonitorI::enableELOG() {
  _w->checkBoxELOGNotify->setEnabled(true);
  _w->_pushButtonELOGConfigure->setEnabled(true);
}


void KstEventMonitorI::disableELOG() {
  _w->checkBoxELOGNotify->setEnabled(false);
  _w->_pushButtonELOGConfigure->setEnabled(false);
}


bool KstEventMonitorI::newObject() {
  QString tag_name = _tagName->text();
  tag_name.replace("<New_Event>", _w->lineEditEquation->text());
  tag_name.replace(KstObjectTag::tagSeparator, KstObjectTag::tagSeparatorReplacement);

  // verify that the event name is unique
  if (KstData::self()->dataTagNameNotUnique(tag_name)) {
    _tagName->setFocus();
    return false;
  }

  EventMonitorEntryPtr event = new EventMonitorEntry(tag_name);
  fillEvent(event);

  if (!event->isValid()) {
    event = 0L;

    KMessageBox::sorry(this, i18n("There is a syntax error in the equation you entered."));
    return false;
  }

  KST::dataObjectList.lock().writeLock();
  KST::dataObjectList.append(event.data());
  KST::dataObjectList.lock().unlock();

  event = 0L; // drop the reference before we update
  emit modified();
  return true;
}


bool KstEventMonitorI::editSingleObject(EventMonitorEntryPtr emPtr) {
  emPtr->writeLock();
  
  if (_lineEditEquationDirty) {
    emPtr->setEvent(_w->lineEditEquation->text());
  }
  
  if (_lineEditDescriptionDirty) {
    emPtr->setDescription(_w->lineEditDescription->text());
  }
  
  if (_checkBoxDebugDirty) {
    if (!(_w->radioButtonLogNotice->isChecked() ||
          _w->radioButtonLogWarning->isChecked() ||
          _w->radioButtonLogError->isChecked()) && _w->checkBoxDebug->isChecked()) {
      KMessageBox::sorry(this, i18n("Select a Debug Log type."));
      emPtr->unlock();
      return false;
    }
    emPtr->setLogKstDebug(_w->checkBoxDebug->isChecked());
  }
  
  if (_checkBoxEMailNotifyDirty) {
    emPtr->setLogEMail(_w->checkBoxEMailNotify->isChecked());
  }
  
  if (_checkBoxELOGNotifyDirty) {
    emPtr->setLogELOG(_w->checkBoxELOGNotify->isChecked());
  }
  
  if (_lineEditEMailRecipientsDirty) {
    emPtr->setEMailRecipients(_w->lineEditEMailRecipients->text());
  }

  if (_scriptDirty) {
    if (_w->_useScript->isChecked()) {
      emPtr->setScriptCode(_w->_script->text());
    } else {
      emPtr->setScriptCode(QString::null);
    }
  }

  if (_w->radioButtonLogNotice->isChecked()) {
    emPtr->setLevel(KstDebug::Notice);
  } else if (_w->radioButtonLogWarning->isChecked()) {
    emPtr->setLevel(KstDebug::Warning);
  } else if (_w->radioButtonLogError->isChecked()) {
    emPtr->setLevel(KstDebug::Error);
  }
  
  emPtr->reparse();
  emPtr->unlock();
  
  return true;
}


bool KstEventMonitorI::editObject() {
  KstEventMonitorEntryList emList = kstObjectSubList<KstDataObject,EventMonitorEntry>(KST::dataObjectList);
  
  // if editing multiple objects, edit each one
  if (_editMultipleMode) { 
    // if text fields are empty, treat as non-dirty
    _lineEditEquationDirty = !_w->lineEditEquation->text().isEmpty();
    _lineEditDescriptionDirty = !_w->lineEditDescription->text().isEmpty();
    _lineEditEMailRecipientsDirty = !_w->lineEditEMailRecipients->text().isEmpty();
    
    bool didEdit = false;
    
    for (uint i = 0; i < _editMultipleWidget->_objectList->count(); i++) {
      if (_editMultipleWidget->_objectList->isSelected(i)) {
        // get the pointer to the object
        KstEventMonitorEntryList::Iterator emIter = emList.findTag(_editMultipleWidget->_objectList->text(i));
        if (emIter == emList.end()) {
          return false;
        }
          
        EventMonitorEntryPtr emPtr = *emIter;

        if (!editSingleObject(emPtr)) {
          return false;
        }
        
        didEdit = true;
      }
    } 
    if (!didEdit) {
      KMessageBox::sorry(this, i18n("Select one or more objects to edit."));
      return false;  
    }
  } else {
    EventMonitorEntryPtr ep = kst_cast<EventMonitorEntry>(_dp);
    // verify that the curve name is unique
    QString tag_name = _tagName->text();
    if (!ep || (tag_name != ep->tagName() && KstData::self()->dataTagNameNotUnique(tag_name))) {
      _tagName->setFocus();
      return false;
    }
    
    ep->writeLock();
    ep->setTagName(KstObjectTag(tag_name, ep->tag().context())); // FIXME: doesn't allow changing tag context
    ep->unlock();
    
    // then edit the object
    _lineEditEquationDirty = true;
    _lineEditDescriptionDirty = true;
    _checkBoxDebugDirty = true;
    _radioButtonLogNoticeDirty = true;
    _radioButtonLogWarningDirty = true;
    _radioButtonLogErrorDirty = true;
    _checkBoxEMailNotifyDirty = true;
    _lineEditEMailRecipientsDirty = true;
    _checkBoxELOGNotifyDirty = true;
    _scriptDirty = true;
    if (!editSingleObject(ep)) {
      return false;
    }
  }
  emit modified();
  return true;
}


void KstEventMonitorI::populateEditMultiple() {
  KstEventMonitorEntryList emlist = kstObjectSubList<KstDataObject,EventMonitorEntry>(KST::dataObjectList);
  _editMultipleWidget->_objectList->insertStringList(emlist.tagNames());

  // also intermediate state for multiple edit
  _w->lineEditEquation->setText("");
  _w->lineEditDescription->setText("");

  _w->checkBoxDebug->setTristate(true);
  _w->checkBoxDebug->setNoChange();
  _w->radioButtonLogNotice->setChecked(false);
  _w->radioButtonLogWarning->setChecked(false);
  _w->radioButtonLogError->setChecked(false);

  _w->checkBoxEMailNotify->setTristate(true);
  _w->checkBoxEMailNotify->setNoChange();
  _w->lineEditEMailRecipients->setText("");

  _w->checkBoxELOGNotify->setTristate(true);
  _w->checkBoxELOGNotify->setNoChange();
  
  _tagName->setText("");
  _tagName->setEnabled(false);
  
  _w->lineEditEMailRecipients->setEnabled(true); 
  _w->radioButtonLogNotice->setEnabled(true);
  _w->radioButtonLogWarning->setEnabled(true);
  _w->radioButtonLogError->setEnabled(true);

  _w->_useScript->setTristate(true);
  _w->_useScript->setNoChange();
  _w->_useScript->setChecked(false);
  _w->_script->setEnabled(false);
  _w->_script->setText("");
  
  // and clean all the fields
  _lineEditEquationDirty = false;
  _lineEditDescriptionDirty = false;
  _checkBoxDebugDirty = false;
  _radioButtonLogNoticeDirty = false;
  _radioButtonLogWarningDirty = false;
  _radioButtonLogErrorDirty = false;
  _checkBoxEMailNotifyDirty = false;
  _lineEditEMailRecipientsDirty = false;
  _checkBoxELOGNotifyDirty = false;
  _scriptDirty = false;
}


void KstEventMonitorI::setScriptDirty() {
  _w->_useScript->setTristate(false);
  _scriptDirty = true; 
}


void KstEventMonitorI::setcheckBoxDebugDirty() {
  _w->checkBoxDebug->setTristate(false);
  _checkBoxDebugDirty = true; 
}


void KstEventMonitorI::setcheckBoxEMailNotifyDirty() {
  _w->checkBoxEMailNotify->setTristate(false);
  _checkBoxEMailNotifyDirty = true;
}


void KstEventMonitorI::setcheckBoxELOGNotifyDirty() {
  _w->checkBoxELOGNotify->setTristate(false);
  _checkBoxELOGNotifyDirty = true;
}

#include "ksteventmonitor.moc"
// vim: ts=2 sw=2 et
