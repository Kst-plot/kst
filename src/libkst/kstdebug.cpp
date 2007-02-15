/***************************************************************************
                                kstdebug.cpp
                             -------------------
    begin                : Mar 07 2004
    copyright            : (C) 2004 The University of Toronto
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

#include "kstdatasource.h"
#include "kstdebug.h"
#include "kstrevision.h"
#include "logevents.h"

#include <kapplication.h>
#include "ksdebug.h"
#include <kglobal.h>
#include <klocale.h>

#include <q3deepcopy.h>
//Added by qt3to4:
#include <Q3ValueList>

static KStaticDeleter<KstDebug> sd;

KstDebug *KstDebug::_self = 0L;

static QMutex soLock;
KstDebug *KstDebug::self() {
  QMutexLocker ml(&soLock);
  if (!_self) {
    sd.setObject(_self, new KstDebug);
  }

  return _self;
}


KstDebug::KstDebug()
: QObject() {
  _applyLimit = false;
  _limit = 10000;
  _kstRevision = QString::fromLatin1(KSTREVISION);
  _hasNewError = false;
}


KstDebug::~KstDebug() {
#ifdef BENCHMARK
  kstdDebug() << "DRAW COUNTS ---------------------------------------" << endl;
  for (QMap<QString,int>::ConstIterator i = _drawCounter.begin(); i != _drawCounter.end(); ++i) {
    kstdDebug() << i.key() << ": " << i.data() << endl;
  }
#endif
}


int KstDebug::limit() const {
  QMutexLocker ml(&_lock);
  return _limit;
}


QStringList KstDebug::dataSourcePlugins() const {
  return KstDataSource::pluginList();
}


void KstDebug::setHandler(QObject *handler) {
  _handler = handler;
}


void KstDebug::log(const QString& msg, LogLevel level) {
  QMutexLocker ml(&_lock);
  LogMessage message;

  message.date  = QDateTime::currentDateTime();
  message.msg   = msg;
  message.level = level;

  _messages.append(message);
  if (_applyLimit && int(_messages.size()) > _limit) {
    Q3ValueListIterator<LogMessage> first = _messages.begin();
    Q3ValueListIterator<LogMessage> last = first;
    last += _messages.size() - _limit;
    _messages.erase(first, last);
  }

  if (level == Error) {
    _hasNewError = true;
  }

  if (_handler) {
    LogEvent *e = new LogEvent(LogEvent::LogAdded);
    e->_msg = message;
    QApplication::postEvent(_handler, e);
  }
}


void KstDebug::clear() {
  clearHasNewError(); // has to be before the lock is acquired
  QMutexLocker ml(&_lock);
  _messages.clear(); 
  LogEvent *e = new LogEvent(LogEvent::LogCleared);
  QApplication::postEvent(_handler, e);
}


QString KstDebug::label(LogLevel level) const {
  switch (level) {
    case Notice:
      return i18n("log level notice", "Notice");
    case Warning:
      return i18n("log level warning", "Warning");
    case Error:
      return i18n("log level error", "Error");
    case Debug:
      return i18n("log level debug", "Debug");
    default:
      return i18n("log level other", "Other");
  }    
}


QString KstDebug::text() {
  QMutexLocker ml(&_lock);
  QString body = i18n("Kst version %1\n\n\nKst log:\n").arg(KSTVERSION);

  for (unsigned i = 0; i < _messages.count(); i++ ) {
    body += i18n("date leveltext: message", "%1 %2: %3\n").arg(KGlobal::locale()->formatDateTime(_messages[i].date)).arg(label(_messages[i].level)).arg(_messages[i].msg);
  }

  body += i18n("\n\nData-source plugins:");
  QStringList dsp = dataSourcePlugins();
  for (QStringList::ConstIterator it = dsp.begin(); it != dsp.end(); ++it) {
    body += '\n';
    body += *it;
  }
  body += "\n\n";
  return body;
}


void KstDebug::setLimit(bool applyLimit, int limit) {
  QMutexLocker ml(&_lock);
  _applyLimit = applyLimit;
  _limit = limit;
}


void KstDebug::sendEmail() {
  kapp->invokeMailer(QString::null, QString::null, QString::null, i18n("Kst Debugging Information"), text());
}


Q3ValueList<KstDebug::LogMessage> KstDebug::messages() const {
  QMutexLocker ml(&_lock);
  return Q3DeepCopy<Q3ValueList<LogMessage> >(_messages);
}


KstDebug::LogMessage KstDebug::message(unsigned n) const {
  QMutexLocker ml(&_lock);
  if (_messages.size() > n) {
    return _messages[n];
  }
  return KstDebug::LogMessage();
}


int KstDebug::logLength() const {
  QMutexLocker ml(&_lock);
  return _messages.size();
}


const QString& KstDebug::kstRevision() const {
  QMutexLocker ml(&_lock);
  return _kstRevision;
}


bool KstDebug::hasNewError() const {
  QMutexLocker ml(&_lock);
  return _hasNewError;
}


void KstDebug::clearHasNewError() {
  QMutexLocker ml(&_lock);
  _hasNewError = false;
}


#include "kstdebug.moc"
// vim: ts=2 sw=2 et
