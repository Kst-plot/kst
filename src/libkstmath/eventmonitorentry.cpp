/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
 *   copyright : (C) 2004 University of British Columbia                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <config.h>

// include files for Qt
#include <qthread.h>
#include <QEvent>
#include <QApplication>

// application specific includes
#include "enodes.h"
#include "emailthread.h"
#include "dialoglauncher.h"
#include "datacollection.h"
#include "eventmonitorentry.h"

#include <QXmlStreamWriter>

#include "debug.h"

#include <assert.h>
#ifndef Q_WS_WIN32
#include <unistd.h>
#endif

extern int yyparse(Kst::ObjectStore *store);
extern void *ParsedEquation;
extern struct yy_buffer_state *yy_scan_string(const char*);

namespace Kst {

const QString EventMonitorEntry::staticTypeString = I18N_NOOP("Event Monitor");
const QString EventMonitorEntry::staticTypeTag = I18N_NOOP("eventmonitor");

namespace {
  const int EventMonitorEventType = int(QEvent::User) + 2931;
  class EventMonitorEvent : public QEvent {
    public:
      EventMonitorEvent(const QString& msg) : QEvent(QEvent::Type(EventMonitorEventType)), logMessage(msg) {}
      QString logMessage;
  };
}

//extern "C" int yyparse();
//extern "C" void *ParsedEquation;
//extern "C" struct yy_buffer_state *yy_scan_string(const char*);

const QString EventMonitorEntry::OUTXVECTOR = "X";
const QString EventMonitorEntry::OUTYVECTOR = "Y";

EventMonitorEntry::EventMonitorEntry(ObjectStore *store) : DataObject(store) {
  _level = Debug::Warning;
  _logDebug = true;
  _logEMail = false;
  _logELOG = false;

  const int NS = 1;

  _numDone = 0;
  _isValid = false;
  _pExpression = 0L;

  _typeString = staticTypeString;
  _type = "Event";
  _initializeShortName();

  VectorPtr xv = store->createObject<Vector>();
  xv->resize(NS);
  xv->setProvider(this);
  _xVector = _outputVectors.insert(OUTXVECTOR, xv);

  VectorPtr yv = store->createObject<Vector>();
  yv->resize(NS);
  yv->setProvider(this);
  _yVector = _outputVectors.insert(OUTYVECTOR, yv);
}

void EventMonitorEntry::_initializeShortName() {
}

bool EventMonitorEntry::reparse() {
  _isValid = false;
  if (!_event.isEmpty()) {
    Equations::mutex().lock();
    yy_scan_string(_event.toLatin1());
    int rc = yyparse(store());
    if (rc == 0) {
      _pExpression = static_cast<Equations::Node*>(ParsedEquation);
      Equations::Context ctx;
      Equations::FoldVisitor vis(&ctx, &_pExpression);
      StringMap stm;
      _pExpression->collectObjects(_vectorsUsed, _inputScalars, stm);

      for (ScalarMap::ConstIterator i = _inputScalars.begin(); i != _inputScalars.end(); ++i) {
        if ((*i)->myLockStatus() == KstRWLock::UNLOCKED) {
          (*i)->readLock();
        }
      }
      Equations::mutex().unlock();
      _isValid = true;
    } else {
      delete (Equations::Node*)ParsedEquation;
      Equations::mutex().unlock();
    }
    ParsedEquation = 0L;
  }
  return _isValid;
}


void EventMonitorEntry::save(QXmlStreamWriter &xml) {
  xml.writeStartElement(staticTypeTag);
  xml.writeAttribute("equation", _event);
  xml.writeAttribute("description", _description);
  xml.writeAttribute("logdebug", QVariant(_logDebug).toString());
  xml.writeAttribute("loglevel", QVariant(_level).toString());
  xml.writeAttribute("logemail", QVariant(_logEMail).toString());
  xml.writeAttribute("logelog", QVariant(_logELOG).toString());
  xml.writeAttribute("emailrecipients", _eMailRecipients);
  xml.writeAttribute("script", _script);
  xml.writeEndElement();
}


EventMonitorEntry::~EventMonitorEntry() {
  logImmediately(false);

  delete _pExpression;
  _pExpression = 0L;
}


void EventMonitorEntry::internalUpdate() {
  writeLockInputsAndOutputs();

  if (!_pExpression) {
    reparse();
  }

  VectorPtr xv = *_xVector;
  VectorPtr yv = *_yVector;
  int ns = 1;

  for (VectorMap::ConstIterator i = _vectorsUsed.begin(); i != _vectorsUsed.end(); ++i) {
    ns = qMax(ns, i.value()->length());
  }

  double *rawValuesX = 0L;
  double *rawValuesY = 0L;
  if (xv && yv) {
    if (xv->resize(ns)) {
      rawValuesX = xv->value();
    }

    if (yv->resize(ns)) {
      rawValuesY = yv->value();
    }
  }

  Equations::Context ctx;
  ctx.sampleCount = ns;
  ctx.x = 0.0;

  if (needToEvaluate()) {
    if (_pExpression) {
      for (ctx.i = _numDone; ctx.i < ns; ++ctx.i) {
        const double value = _pExpression->value(&ctx);
        if (value != 0.0) { // The expression evaluates to true
          log(ctx.i);
          if (rawValuesX && rawValuesY) {
            rawValuesX[ctx.i] = ctx.i;
            rawValuesY[ctx.i] = 1.0;
          }
        } else {
          if (rawValuesX && rawValuesY) {
            rawValuesX[ctx.i] = ctx.i;
            rawValuesY[ctx.i] = 0.0;
          }
        }
      }
      _numDone = ns;
      logImmediately();
    }
  } else {
    _numDone = ns;
  }

  unlockInputsAndOutputs();

  return;
}


void EventMonitorEntry::setEvent(const QString& strEvent) {
  if (_event != strEvent) {
    _event = strEvent;
    _vectorsUsed.clear();
    _inputScalars.clear();

    _numDone = 0;
    _isValid = false;

    delete _pExpression;
    _pExpression = 0L;
  }
}


bool EventMonitorEntry::needToEvaluate() {
  return _logDebug || _logEMail || _logELOG || !_script.isEmpty();
}


void EventMonitorEntry::logImmediately(bool sendEvent) {
  const int arraySize = _indexArray.size();

  if (arraySize > 0) {
    QString logMessage;
    QString rangeString;
    bool makeRange = false;
    int idx = 0;
    int idxOld = 0;

    for (int i = 0; i < arraySize; ++i) {
      idx = _indexArray.at(i);
      if (i == 0) {
        rangeString.setNum(idx);
      } else if (!makeRange && idx == idxOld + 1) {
        makeRange = true;
      } else if (makeRange && idx != idxOld + 1) {
        rangeString = rangeString + QString(" - %1, %2").arg(idxOld).arg(idx);
        makeRange = false;
      } else if (idx != idxOld + 1) {
        rangeString = rangeString + QString(", %1").arg(idx);
      }
      idxOld = idx;
    }

    if (makeRange) {
        rangeString = rangeString + QString(", %1").arg(idx);
    }

    if (_description.isEmpty()) {
      logMessage = "Event Monitor: " + _event + ": " + rangeString;
    } else {
      logMessage = "Event Monitor: " + _description + ": " + rangeString;
    }

    _indexArray.clear();

    if (sendEvent) { // update thread
      QApplication::postEvent(this, new EventMonitorEvent(logMessage));
    } else { // GUI thread
      doLog(logMessage);
    }
  }
}


bool EventMonitorEntry::event(QEvent *e) {
    if (e->type() == EventMonitorEventType) {
      readLock();
      doLog(static_cast<EventMonitorEvent*>(e)->logMessage);
      unlock();
      return true;
    }
    return false;
}


void EventMonitorEntry::doLog(const QString& logMessage) const {
  if (_logDebug) {
    Debug::self()->log(logMessage, _level);
  }

  if (_logEMail && !_eMailRecipients.isEmpty()) {
    EMailThread* thread = new EMailThread(_eMailRecipients, "Kst Event Monitoring Notification", logMessage);
    thread->send();
  }

//FIXME ELOG need to be implemented.
//   if (_logELOG) {
//     KstApp::inst()->EventELOGSubmitEntry(logMessage);
//   }

//FIXME PORT!
//   if (!_script.isEmpty()) {
//     DCOPRef ref(QString("kst-%1").arg(getpid()).toLatin1(), "KstScript");
//     ref.send("evaluate", _script);
//   }
}


void EventMonitorEntry::log(int idx) {
  _indexArray.append(idx);
  if (_indexArray.size() > 1000) {
    logImmediately();
  }
}


QString EventMonitorEntry::propertyString() const {
  return _event;
}


void EventMonitorEntry::showNewDialog() {
  DialogLauncher::self()->showEventMonitorDialog();
}


void EventMonitorEntry::showEditDialog() {
  DialogLauncher::self()->showEventMonitorDialog();
}


const QString& EventMonitorEntry::scriptCode() const {
  return _script;
}


void EventMonitorEntry::setScriptCode(const QString& script) {
  if (_script != script) {
    _script = script;
  }
}


void EventMonitorEntry::setDescription(const QString& str) {
  if (_description != str) {
    _description = str;
  }
}


void EventMonitorEntry::setLevel(Debug::LogLevel level) {
  if (_level != level) {
    _level = level;
  }
}


void EventMonitorEntry::setExpression(Equations::Node* pExpression) {
  if (_pExpression != pExpression) {
    _pExpression = pExpression;
  }
}


void EventMonitorEntry::setLogDebug(bool logDebug) {
  if (_logDebug != logDebug) {
    _logDebug = logDebug;
  }
}


void EventMonitorEntry::setLogEMail(bool logEMail) {
  if (logEMail != _logEMail) {
    _logEMail = logEMail;
  }
}


void EventMonitorEntry::setLogELOG(bool logELOG) {
  if (logELOG != _logELOG) {
    _logELOG = logELOG;
  }
}


void EventMonitorEntry::setEMailRecipients(const QString& str) {
  if (str != _eMailRecipients) {
    _eMailRecipients = str;
  }
}


DataObjectPtr EventMonitorEntry::makeDuplicate() {
  EventMonitorEntryPtr eventMonitor = store()->createObject<EventMonitorEntry>();

  eventMonitor->setScriptCode(_script);
  eventMonitor->setEvent(_event);
  eventMonitor->setDescription(_description);
  eventMonitor->setLevel(_level);
  eventMonitor->setLogDebug(_logDebug);
  eventMonitor->setLogEMail(_logEMail);
  eventMonitor->setLogELOG(_logELOG);
  eventMonitor->setEMailRecipients(_eMailRecipients);

  if (descriptiveNameIsManual()) {
    eventMonitor->setDescriptiveName(descriptiveName());
  }
  eventMonitor->reparse();

  eventMonitor->writeLock();
  eventMonitor->registerChange();
  eventMonitor->unlock();

  return DataObjectPtr(eventMonitor);
}


void EventMonitorEntry::replaceDependency(DataObjectPtr oldObject, DataObjectPtr newObject) {
  QString newExp = _event;

  // replace all occurences of outputVectors, outputScalars from oldObject
  for (VectorMap::ConstIterator j = oldObject->outputVectors().begin(); j != oldObject->outputVectors().end(); ++j) {
    const QString oldName = j.value()->Name();
    const QString newName = newObject->outputVectors()[j.key()]->Name();
    newExp = newExp.replace("[" + oldName + "]", "[" + newName + "]");
  }

  for (ScalarMap::ConstIterator j = oldObject->outputScalars().begin(); j != oldObject->outputScalars().end(); ++j) {
    const QString oldName = j.value()->Name();
    const QString newName = newObject->outputScalars()[j.key()]->Name();
    newExp = newExp.replace("[" + oldName + "]", "[" + newName + "]");
  }

  // and dependencies on vector stats
  for (VectorMap::ConstIterator j = oldObject->outputVectors().begin(); j != oldObject->outputVectors().end(); ++j) {
    const QHash<QString, ScalarPtr>& scalarMap(newObject->outputVectors()[j.key()]->scalars());
    QHashIterator<QString, ScalarPtr> scalarDictIter(j.value()->scalars());
    while (scalarDictIter.hasNext()) {
      const QString oldName = scalarDictIter.next().value()->Name();
      const QString newName = scalarMap[scalarDictIter.key()]->Name();
      newExp = newExp.replace("[" + oldName + "]", "[" + newName + "]");
    }
  }

  // and dependencies on matrix stats
  for (MatrixMap::ConstIterator j = oldObject->outputMatrices().begin(); j != oldObject->outputMatrices().end(); ++j) {
    const QHash<QString, ScalarPtr>& scalarMap(newObject->outputMatrices()[j.key()]->scalars());
    QHashIterator<QString, ScalarPtr> scalarDictIter(j.value()->scalars());
    while (scalarDictIter.hasNext()) {
      const QString oldName = scalarDictIter.next().value()->Name();
      const QString newName = scalarMap[scalarDictIter.key()]->Name();
      newExp = newExp.replace("[" + oldName + "]", "[" + newName + "]");
    }
  }

  setEvent(newExp);

  // events have no _inputVectors
}


void EventMonitorEntry::replaceDependency(VectorPtr oldVector, VectorPtr newVector) {
  // replace all occurences of oldName with newName
  QString newExp = _event.replace("[" + oldVector->Name() + "]", "[" + newVector->Name() + "]");

  // also replace all occurences of vector stats for the oldVector
  QHashIterator<QString, ScalarPtr> scalarDictIter(oldVector->scalars());
  while (scalarDictIter.hasNext()) {
    const QString oldName = scalarDictIter.next().value()->Name();
    const QString newName = newVector->scalars()[scalarDictIter.key()]->Name();
    newExp = newExp.replace("[" + oldName + "]", "[" + newName + "]");
  }

  setEvent(newExp);

  // events have no _inputVectors
}


void EventMonitorEntry::replaceDependency(MatrixPtr oldMatrix, MatrixPtr newMatrix) {
  QString newExp = _event;

  // also replace all occurences of scalar stats for the oldMatrix
  QHashIterator<QString, ScalarPtr> scalarDictIter(oldMatrix->scalars());
  while (scalarDictIter.hasNext()) {
    const QString oldName = scalarDictIter.next().value()->Name();
    const QString newName = newMatrix->scalars()[scalarDictIter.key()]->Name();
    newExp = newExp.replace("[" + oldName + "]", "[" + newName + "]");
  }

  setEvent(newExp);
}


bool EventMonitorEntry::uses(ObjectPtr p) const {
  // check VectorsUsed in addition to _input*'s
  if (VectorPtr vect = kst_cast<Vector>(p)) {
    for (VectorMap::ConstIterator j = _vectorsUsed.begin(); j != _vectorsUsed.end(); ++j) {
      if (j.value() == vect) {
        return true;
      }
    }
  } else if (DataObjectPtr obj = kst_cast<DataObject>(p) ) {
    // check all connections from this expression to p
    for (VectorMap::ConstIterator j = obj->outputVectors().begin(); j != obj->outputVectors().end(); ++j) {
      for (VectorMap::ConstIterator k = _vectorsUsed.begin(); k != _vectorsUsed.end(); ++k) {
        if (j.value() == k.value()) {
          return true;
        }
      }
    }
  }
  return DataObject::uses(p);
}

QString EventMonitorEntry::_automaticDescriptiveName() const {
  return i18n("event");
}

QString EventMonitorEntry::descriptionTip() const {
  return i18n("Event: %1").arg(Name());
}

}

// vim: ts=2 sw=2 et
