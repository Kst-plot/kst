/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2007 The University of Toronto                        *
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

#include "debug.h"

#include <assert.h>
#include <unistd.h>

namespace Kst {

namespace {
  const int EventMonitorEventType = int(QEvent::User) + 2931;
  class EventMonitorEvent : public QEvent {
    public:
      EventMonitorEvent(const QString& msg) : QEvent(QEvent::Type(EventMonitorEventType)), logMessage(msg) {}
      QString logMessage;
  };
}

extern "C" int yyparse();
extern "C" void *ParsedEquation;
extern "C" struct yy_buffer_state *yy_scan_string(const char*);

const QString EventMonitorEntry::OUTXVECTOR = "X";
const QString EventMonitorEntry::OUTYVECTOR = "Y";

EventMonitorEntry::EventMonitorEntry(const QString &in_tag) : DataObject() {
  _level = Debug::Warning;
  _logKstDebug = true;
  _logEMail = false;
  _logELOG = false;

  commonConstructor(in_tag);
}


EventMonitorEntry::EventMonitorEntry(const QString &tag, const QString &script, const QString &event, const QString &description, const Debug::LogLevel level, const bool logKstDebug, const bool logEMail, const bool logELOG, const QString& emailRecipients) {

  _event = event;
  _description = description;
  _eMailRecipients = emailRecipients;
  _logKstDebug = logKstDebug;
  _logEMail = logEMail;
  _logELOG = logELOG;
  _level = level;
  _script = script;

  commonConstructor(tag);

}

EventMonitorEntry::EventMonitorEntry(const QDomElement &e) {
  QString strTag;

  _level = Debug::Warning;
  _logKstDebug = true;
  _logEMail = false;
  _logELOG = false;

  QDomNode n = e.firstChild();
  while (!n.isNull()) {
    QDomElement e = n.toElement(); // try to convert the node to an element.
    if (!e.isNull()) { // the node was really an element.
      if (e.tagName() == "tag") {
        strTag = e.text();
      } else if (e.tagName() == "equation") {
        _event = e.text();
      } else if (e.tagName() == "description") {
        _description = e.text();
      } else if (e.tagName() == "logdebug") {
        _logKstDebug = e.text().toInt();
      } else if (e.tagName() == "loglevel") {
        _level = (Debug::LogLevel)e.text().toInt();
      } else if (e.tagName() == "logemail") {
        _logEMail = e.text().toInt();
      } else if (e.tagName() == "logelog") {
        _logELOG = e.text().toInt();
      } else if (e.tagName() == "emailRecipients") {
        _eMailRecipients = e.text();
      } else if (e.tagName() == "script") {
        _script = e.text();
      }
    }
    n = n.nextSibling();
  }

  commonConstructor(strTag);

  // wait for the initial update, as we don't want to trigger elog entries
  // until we are sure the document is open.
  //QTimer::singleShot(500, this, SLOT(slotUpdate()));
}


void EventMonitorEntry::commonConstructor(const QString &in_tag) {
  const int NS = 1;

  _numDone = 0;
  _isValid = false;
  _pExpression = 0L;

  _typeString = "Event";
  _type = "Event";
  Object::setTagName(ObjectTag::fromString(in_tag));

  VectorPtr xv = new Vector(ObjectTag("x", tag()), NS, this);
  _xVector = _outputVectors.insert(OUTXVECTOR, xv);

  VectorPtr yv = new Vector(ObjectTag("y", tag()), NS, this);
  _yVector = _outputVectors.insert(OUTYVECTOR, yv);
}


bool EventMonitorEntry::reparse() {
  _isValid = false;
  if (!_event.isEmpty()) {
    Equations::mutex().lock();
    yy_scan_string(_event.toLatin1());
    int rc = yyparse();
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
  xml.writeStartElement("event");
  xml.writeAttribute("tag", tag().tagString());
  xml.writeAttribute("equation", _event);
  xml.writeAttribute("description", _description);
  xml.writeAttribute("logdebug", _logKstDebug);
  xml.writeAttribute("loglevel", _level);
  xml.writeAttribute("logemail", _logEMail);
  xml.writeAttribute("logelog", _logELOG);
  xml.writeAttribute("emailRecipients", _eMailRecipients);
  xml.writeAttribute("script", _script);
  xml.writeEndElement();
}


EventMonitorEntry::~EventMonitorEntry() {
  logImmediately(false);

  delete _pExpression;
  _pExpression = 0L;
}


void EventMonitorEntry::slotUpdate() {
  update();
}


Object::UpdateType EventMonitorEntry::update(int updateCounter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  if (Object::checkUpdateCounter(updateCounter) && !force) {
    return lastUpdateResult();
  }

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

  if (xv) {
    xv->setDirty();
    xv->update(updateCounter);
  }

  if (yv) {
    yv->setDirty();
    yv->update(updateCounter);
  }

  unlockInputsAndOutputs();

  return setLastUpdateResult(NO_CHANGE);
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
  return _logKstDebug || _logEMail || _logELOG || !_script.isEmpty();
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
        rangeString = rangeString + " - ";
        rangeString += idxOld + ", " + idx;
        makeRange = false;
      } else if (idx != idxOld + 1) {
        rangeString = rangeString + ", ";
        rangeString += idx;
      }
      idxOld = idx;
    }

    if (makeRange) {
        rangeString = rangeString + ", ";
        rangeString += idx;
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
  if (_logKstDebug) {
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
    setDirty();
    _script = script;
  }
}


void EventMonitorEntry::setDescription(const QString& str) {
  if (_description != str) {
    setDirty();
    _description = str;
  }
}


void EventMonitorEntry::setLevel(Debug::LogLevel level) {
  if (_level != level) {
    setDirty();
    _level = level;
  }
}


void EventMonitorEntry::setExpression(Equations::Node* pExpression) {
  if (_pExpression != pExpression) {
    setDirty();
    _pExpression = pExpression;
  }
}


void EventMonitorEntry::setLogKstDebug(bool logKstDebug) {
  if (_logKstDebug != logKstDebug) {
    setDirty();
    _logKstDebug = logKstDebug;
  }
}


void EventMonitorEntry::setLogEMail(bool logEMail) {
  if (logEMail != _logEMail) {
    setDirty();
    _logEMail = logEMail;
  }
}


void EventMonitorEntry::setLogELOG(bool logELOG) {
  if (logELOG != _logELOG) {
    setDirty();
    _logELOG = logELOG;
  }
}


void EventMonitorEntry::setEMailRecipients(const QString& str) {
  if (str != _eMailRecipients) {
    setDirty();
    _eMailRecipients = str;
  }
}


DataObjectPtr EventMonitorEntry::makeDuplicate(DataObjectDataObjectMap& duplicatedMap) {
  QString name(tagName() + '\'');
  while (Data::self()->dataTagNameNotUnique(name, false)) {
    name += '\'';
  }
  EventMonitorEntryPtr event = new EventMonitorEntry(name);
  event->setEvent(_event);
  event->setDescription(_description);
  event->setLevel(_level);
  event->setLogKstDebug(_logKstDebug);
  event->setLogEMail(_logEMail);
  event->setLogELOG(_logELOG);
  event->setEMailRecipients(_eMailRecipients);

  duplicatedMap.insert(this, DataObjectPtr(event));
  return DataObjectPtr(event);
}


void EventMonitorEntry::replaceDependency(DataObjectPtr oldObject, DataObjectPtr newObject) {
  QString newExp = _event;

  // replace all occurences of outputVectors, outputScalars from oldObject
  for (VectorMap::ConstIterator j = oldObject->outputVectors().begin(); j != oldObject->outputVectors().end(); ++j) {
    const QString oldTag = j.value()->tagName();
    const QString newTag = newObject->outputVectors()[j.key()]->tagName();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
  }

  for (ScalarMap::ConstIterator j = oldObject->outputScalars().begin(); j != oldObject->outputScalars().end(); ++j) {
    const QString oldTag = j.value()->tagName();
    const QString newTag = newObject->outputScalars()[j.key()]->tagName();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
  }

  // and dependencies on vector stats
  for (VectorMap::ConstIterator j = oldObject->outputVectors().begin(); j != oldObject->outputVectors().end(); ++j) {
    const QHash<QString, Scalar*>& scalarMap(newObject->outputVectors()[j.key()]->scalars());
    QHashIterator<QString, Scalar*> scalarDictIter(j.value()->scalars());
    while (scalarDictIter.hasNext()) {
      const QString oldTag = scalarDictIter.next().value()->tagName();
      const QString newTag = scalarMap[scalarDictIter.key()]->tagName();
      newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
    }
  }

  // and dependencies on matrix stats
  for (MatrixMap::ConstIterator j = oldObject->outputMatrices().begin(); j != oldObject->outputMatrices().end(); ++j) {
    const QHash<QString, Scalar*>& scalarMap(newObject->outputMatrices()[j.key()]->scalars());
    QHashIterator<QString, Scalar*> scalarDictIter(j.value()->scalars());
    while (scalarDictIter.hasNext()) {
      const QString oldTag = scalarDictIter.next().value()->tagName();
      const QString newTag = scalarMap[scalarDictIter.key()]->tagName();
      newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
    }
  }

  setEvent(newExp);
  setDirty();

  // events have no _inputVectors
}


void EventMonitorEntry::replaceDependency(VectorPtr oldVector, VectorPtr newVector) {
  // replace all occurences of oldTag with newTag
  QString newExp = _event.replace("[" + oldVector->tagName() + "]", "[" + newVector->tagName() + "]");

  // also replace all occurences of vector stats for the oldVector
  QHashIterator<QString, Scalar*> scalarDictIter(oldVector->scalars());
  while (scalarDictIter.hasNext()) {
    const QString oldTag = scalarDictIter.next().value()->tagName();
    const QString newTag = newVector->scalars()[scalarDictIter.key()]->tagName();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
  }

  setEvent(newExp);
  setDirty();

  // events have no _inputVectors
}


void EventMonitorEntry::replaceDependency(MatrixPtr oldMatrix, MatrixPtr newMatrix) {
  QString newExp = _event;

  // also replace all occurences of scalar stats for the oldMatrix
  QHashIterator<QString, Scalar*> scalarDictIter(oldMatrix->scalars());
  while (scalarDictIter.hasNext()) {
    const QString oldTag = scalarDictIter.next().value()->tagName();
    const QString newTag = newMatrix->scalars()[scalarDictIter.key()]->tagName();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
  }

  setEvent(newExp);
  setDirty();
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

}

// vim: ts=2 sw=2 et
