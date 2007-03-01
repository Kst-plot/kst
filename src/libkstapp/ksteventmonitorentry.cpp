/***************************************************************************
                          ksteventmonitorentry.cpp  -  description
                             -------------------
    begin                : Tue Apr 6 2004
    copyright            : (C) 2004 The University of British Columbia
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *   Permission is granted to link with any opensource library             *
 *                                                                         *
 ***************************************************************************/

#include <config.h>

// include files for Qt
#include <q3stylesheet.h>
#include <qthread.h>
//Added by qt3to4:
#include <QEvent>

// include files for KDE
#include <dcopref.h>
#include <klocale.h>

// application specific includes
#include "enodes.h"
#include "emailthread.h"
#include "kst.h"
#include "kstdatacollection.h"
#include "ksteventmonitorentry.h"
#include "ksteventmonitor.h"

#include <assert.h>
#include <unistd.h>

extern "C" int yyparse();
extern "C" void *ParsedEquation;
extern "C" struct yy_buffer_state *yy_scan_string(const char*);

const QString EventMonitorEntry::OUTXVECTOR = "X";
const QString EventMonitorEntry::OUTYVECTOR = "Y";

EventMonitorEntry::EventMonitorEntry(const QString &in_tag) : KstDataObject() {
  _level = KstDebug::Warning;
  _logKstDebug = true;
  _logEMail = false;
  _logELOG = false;

  commonConstructor(in_tag);
  setDirty();
}


EventMonitorEntry::EventMonitorEntry(const QDomElement &e) {
  QString strTag;

  _level = KstDebug::Warning;
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
        _level = (KstDebug::LogLevel)e.text().toInt();
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

  _typeString = i18n("Event");
  _type = "Event";
  KstObject::setTagName(KstObjectTag::fromString(in_tag));

  KstVectorPtr xv = new KstVector(KstObjectTag("x", tag()), NS, this);
  _xVector = _outputVectors.insert(OUTXVECTOR, xv);

  KstVectorPtr yv = new KstVector(KstObjectTag("y", tag()), NS, this);
  _yVector = _outputVectors.insert(OUTYVECTOR, yv);
}


bool EventMonitorEntry::reparse() {
  _isValid = false;
  if (!_event.isEmpty()) {
    QMutexLocker ml(&Equation::mutex());
    yy_scan_string(_event.toLatin1());
    int rc = yyparse();
    if (rc == 0) {
      _pExpression = static_cast<Equation::Node*>(ParsedEquation);
      Equation::Context ctx;
      Equation::FoldVisitor vis(&ctx, &_pExpression);
      KstStringMap stm;
      _pExpression->collectObjects(_vectorsUsed, _inputScalars, stm);

      for (KstScalarMap::ConstIterator i = _inputScalars.begin(); i != _inputScalars.end(); ++i) {
        if ((*i)->myLockStatus() == KstRWLock::UNLOCKED) {
          (*i)->readLock();
        }
      }

      _isValid = true;
    } else {
      delete (Equation::Node*)ParsedEquation;
    }
    ParsedEquation = 0L;
  }
  return _isValid;
}


void EventMonitorEntry::save(Q3TextStream &ts, const QString& indent) {
  QString l2 = indent + "  ";
  ts << indent << "<event>" << endl;
  ts << l2 << "<tag>" << Q3StyleSheet::escape(tagName()) << "</tag>" << endl;
  ts << l2 << "<equation>" << Q3StyleSheet::escape(_event) << "</equation>" << endl;
  ts << l2 << "<description>" << Q3StyleSheet::escape(_description) << "</description>" << endl;
  ts << l2 << "<logdebug>" << QString::number(_logKstDebug) << "</logdebug>" << endl;
  ts << l2 << "<loglevel>" << QString::number(_level) << "</loglevel>" << endl;
  ts << l2 << "<logemail>" << QString::number(_logEMail) << "</logemail>" << endl;
  ts << l2 << "<logelog>" << QString::number(_logELOG) << "</logelog>" << endl;
  ts << l2 << "<emailRecipients>" << Q3StyleSheet::escape(_eMailRecipients) << "</emailRecipients>" << endl;
  ts << l2 << "<script>" << Q3StyleSheet::escape(_script) << "</script>" << endl;
  ts << indent << "</event>" << endl;
}


EventMonitorEntry::~EventMonitorEntry() {
  logImmediately(false);

  delete _pExpression;
  _pExpression = 0L;
}


void EventMonitorEntry::slotUpdate() {
  // possible deadlock?  bad idea.  updates should only happen in update thread
  KstWriteLocker wl(this);
  update();
}


KstObject::UpdateType EventMonitorEntry::update(int updateCounter) {
  Q_ASSERT(myLockStatus() == KstRWLock::WRITELOCKED);

  bool force = dirty();
  setDirty(false);

  if (KstObject::checkUpdateCounter(updateCounter) && !force) {
    return lastUpdateResult();
  }

  writeLockInputsAndOutputs();

  if (!_pExpression) {
    reparse();
  }

  KstVectorPtr xv = *_xVector;
  KstVectorPtr yv = *_yVector;
  int ns = 1;

  for (KstVectorMap::ConstIterator i = _vectorsUsed.begin(); i != _vectorsUsed.end(); ++i) {
    ns = kMax(ns, i.data()->length());
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

  Equation::Context ctx;
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


namespace {
  const int EventMonitorEventType = int(QEvent::User) + 2931;
  class EventMonitorEvent : public QEvent {
    public:
      EventMonitorEvent(const QString& msg) : QEvent(QEvent::Type(EventMonitorEventType)), logMessage(msg) {}
      QString logMessage;
  };
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
      idx = *_indexArray.at(i);
      if (i == 0) {
        rangeString.setNum(idx);
      } else if (!makeRange && idx == idxOld + 1) {
        makeRange = true;
      } else if (makeRange && idx != idxOld + 1) {
        rangeString = i18n("%1-%2,%3").arg(rangeString).arg(idxOld).arg(idx);
        makeRange = false;
      } else if (idx != idxOld + 1) {
        rangeString = i18n("%1,%2").arg(rangeString).arg(idx);
      }
      idxOld = idx;
    }

    if (makeRange) {
      rangeString = i18n("%1-%2").arg(rangeString).arg(idx);
    }

    if (_description.isEmpty()) {
      logMessage = i18n("Event Monitor: %1: %2").arg(_event).arg(rangeString);
    } else {
      logMessage = i18n("Event Monitor: %1: %2").arg(_description).arg(rangeString);
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
    KstDebug::self()->log(logMessage, _level);
  }

  if (_logEMail && !_eMailRecipients.isEmpty()) {
    EMailThread* thread = new EMailThread(_eMailRecipients, i18n("Kst Event Monitoring Notification"), logMessage);
    thread->send();
  }

  if (_logELOG) {
    KstApp::inst()->EventELOGSubmitEntry(logMessage);
  }

  if (!_script.isEmpty()) {
    DCOPRef ref(QString("kst-%1").arg(getpid()).toLatin1(), "KstScript");
    ref.send("evaluate", _script);
  }
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
  KstEventMonitorI::globalInstance()->show();
}


void EventMonitorEntry::showEditDialog() {
  KstEventMonitorI::globalInstance()->showEdit(tagName());
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


void EventMonitorEntry::setLevel(KstDebug::LogLevel level) {
  if (_level != level) {
    setDirty();
    _level = level;
  }
}


void EventMonitorEntry::setExpression(Equation::Node* pExpression) {
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


KstDataObjectPtr EventMonitorEntry::makeDuplicate(KstDataObjectDataObjectMap& duplicatedMap) {
  QString name(tagName() + '\'');
  while (KstData::self()->dataTagNameNotUnique(name, false)) {
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

  duplicatedMap.insert(this, KstDataObjectPtr(event));
  return KstDataObjectPtr(event);
}


void EventMonitorEntry::replaceDependency(KstDataObjectPtr oldObject, KstDataObjectPtr newObject) {
  QString newExp = _event;

  // replace all occurences of outputVectors, outputScalars from oldObject
  for (KstVectorMap::ConstIterator j = oldObject->outputVectors().begin(); j != oldObject->outputVectors().end(); ++j) {
    const QString oldTag = j.data()->tagName();
    const QString newTag = newObject->outputVectors()[j.key()]->tagName();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
  }

  for (KstScalarMap::ConstIterator j = oldObject->outputScalars().begin(); j != oldObject->outputScalars().end(); ++j) {
    const QString oldTag = j.data()->tagName();
    const QString newTag = newObject->outputScalars()[j.key()]->tagName();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
  }

  // and dependencies on vector stats
  for (KstVectorMap::ConstIterator j = oldObject->outputVectors().begin(); j != oldObject->outputVectors().end(); ++j) {
    const Q3Dict<KstScalar>& scalarMap(newObject->outputVectors()[j.key()]->scalars());
    Q3DictIterator<KstScalar> scalarDictIter(j.data()->scalars());
    for (; scalarDictIter.current(); ++scalarDictIter) {
      const QString oldTag = scalarDictIter.current()->tagName();
      const QString newTag = scalarMap[scalarDictIter.currentKey()]->tagName();
      newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
    }
  }

  // and dependencies on matrix stats
  for (KstMatrixMap::ConstIterator j = oldObject->outputMatrices().begin(); j != oldObject->outputMatrices().end(); ++j) {
    const Q3Dict<KstScalar>& scalarMap(newObject->outputMatrices()[j.key()]->scalars());
    Q3DictIterator<KstScalar> scalarDictIter(j.data()->scalars());
    for (; scalarDictIter.current(); ++scalarDictIter) {
      const QString oldTag = scalarDictIter.current()->tagName();
      const QString newTag = scalarMap[scalarDictIter.currentKey()]->tagName();
      newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
    }
  }

  setEvent(newExp);
  setDirty();

  // events have no _inputVectors
}


void EventMonitorEntry::replaceDependency(KstVectorPtr oldVector, KstVectorPtr newVector) {
  // replace all occurences of oldTag with newTag
  QString newExp = _event.replace("[" + oldVector->tagName() + "]", "[" + newVector->tagName() + "]");

  // also replace all occurences of vector stats for the oldVector
  Q3DictIterator<KstScalar> scalarDictIter(oldVector->scalars());
  for (; scalarDictIter.current(); ++scalarDictIter) {
    const QString oldTag = scalarDictIter.current()->tagName();
    const QString newTag = newVector->scalars()[scalarDictIter.currentKey()]->tagName();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
  }

  setEvent(newExp);
  setDirty();

  // events have no _inputVectors
}


void EventMonitorEntry::replaceDependency(KstMatrixPtr oldMatrix, KstMatrixPtr newMatrix) {
  QString newExp = _event;

  // also replace all occurences of scalar stats for the oldMatrix
  Q3DictIterator<KstScalar> scalarDictIter(oldMatrix->scalars());
  for (; scalarDictIter.current(); ++scalarDictIter) {
    const QString oldTag = scalarDictIter.current()->tagName();
    const QString newTag = newMatrix->scalars()[scalarDictIter.currentKey()]->tagName();
    newExp = newExp.replace("[" + oldTag + "]", "[" + newTag + "]");
  }

  setEvent(newExp);
  setDirty();
}


bool EventMonitorEntry::uses(KstObjectPtr p) const {
  // check VectorsUsed in addition to _input*'s
  if (KstVectorPtr vect = kst_cast<KstVector>(p)) {
    for (KstVectorMap::ConstIterator j = _vectorsUsed.begin(); j != _vectorsUsed.end(); ++j) {
      if (j.data() == vect) {
        return true;
      }
    }
  } else if (KstDataObjectPtr obj = kst_cast<KstDataObject>(p) ) {
    // check all connections from this expression to p
    for (KstVectorMap::ConstIterator j = obj->outputVectors().begin(); j != obj->outputVectors().end(); ++j) {
      for (KstVectorMap::ConstIterator k = _vectorsUsed.begin(); k != _vectorsUsed.end(); ++k) {
        if (j.data() == k.data()) {
          return true;
        }
      }
    }
  }
  return KstDataObject::uses(p);
}


#include "ksteventmonitorentry.moc"

// vim: ts=2 sw=2 et
