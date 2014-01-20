/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2004 The University of Toronto                        *
 *                   netterfield@astro.utoronto.ca                         *
*                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "debug.h"

#include <config.h>

#ifdef KST_HAVE_REVISION_H
#include "kstrevision.h"
#endif

#include "datasource.h"
#include "logevents.h"

#include <qlocale.h>
#include <qapplication.h>
#include <qdebug.h>

#include "datasourcepluginmanager.h"

#include "ksttimers.h"

namespace Kst {

Debug *Debug::_self = 0L;
void Debug::cleanup() {
    delete _self;
    _self = 0;
}


static QMutex soLock;
Debug *Debug::self() {
  QMutexLocker ml(&soLock);
  if (!_self) {
    _self = new Debug;
    qAddPostRoutine(Debug::cleanup);
  }

  return _self;
}


Debug::Debug()
: QObject() {
  _applyLimit = false;
  _limit = 10000;
#ifdef KST_REVISION
  _kstRevision = QString::fromLatin1(KST_REVISION);
#else
  _kstRevision = -1;
#endif
  _hasNewError = false;
}


Debug::~Debug() {
#ifdef BENCHMARK
  qDebug() << "DRAW COUNTS ---------------------------------------" << endl;
  for (QMap<QString,int>::ConstIterator i = _drawCounter.begin(); i != _drawCounter.end(); ++i) {
    qDebug() << i.key() << ": " << i.value() << endl;
  }
#endif
}


int Debug::limit() const {
  QMutexLocker ml(&_lock);
  return _limit;
}


QStringList Debug::dataSourcePlugins() const {
  return DataSourcePluginManager::pluginList();
}


void Debug::setHandler(QObject *handler) {
  _handler = handler;
}


void Debug::log(const QString& msg, LogLevel level) {
  QMutexLocker ml(&_lock);
  LogMessage message;

  message.date  = QDateTime::currentDateTime();
  message.msg   = msg;
  message.level = level;

  _messages.append(message);
  if (_applyLimit && int(_messages.size()) > _limit) {
    QList<LogMessage>::Iterator first = _messages.begin();
    QList<LogMessage>::Iterator last = first;
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


void Debug::clear() {
  clearHasNewError(); // has to be before the lock is acquired
  QMutexLocker ml(&_lock);
  _messages.clear(); 
  LogEvent *e = new LogEvent(LogEvent::LogCleared);
  QApplication::postEvent(_handler, e);
}


QString Debug::label(LogLevel level) const {
  switch (level) {
    case Notice:
      return tr("Notice");
    case Warning:
      return tr("Warning");
    case Error:
      return tr("Error");
    case DebugLog:
      return tr("Debug");
    default:
      return tr("Other");
  }    
}


QString Debug::text() {
  QMutexLocker ml(&_lock);
  QString body = tr("Kst version %1\n\n\nKst log:\n").arg(KSTVERSION);

  QLocale locale;
  for (int i = 0; i < _messages.count(); i++ ) {
    body += QString("%1 %2: %3\n").arg(_messages[i].date.toString(locale.dateFormat())).arg(label(_messages[i].level)).arg(_messages[i].msg);
  }

  body += tr("\n\nData-source plugins:");
  QStringList dsp = dataSourcePlugins();
  for (QStringList::ConstIterator it = dsp.constBegin(); it != dsp.constEnd(); ++it) {
    body += '\n';
    body += *it;
  }
  body += "\n\n";
  return body;
}


void Debug::setLimit(bool applyLimit, int limit) {
  QMutexLocker ml(&_lock);
  _applyLimit = applyLimit;
  _limit = limit;
}


QList<Debug::LogMessage> Debug::messages() const {
  QMutexLocker ml(&_lock);
  return _messages;
}


Debug::LogMessage Debug::message(unsigned n) const {
  QMutexLocker ml(&_lock);
  if (_messages.size() > int(n)) {
    return _messages[n];
  }
  return Debug::LogMessage();
}


int Debug::logLength() const {
  QMutexLocker ml(&_lock);
  return _messages.size();
}


const QString& Debug::kstRevision() const {
  QMutexLocker ml(&_lock);
  return _kstRevision;
}


bool Debug::hasNewError() const {
  QMutexLocker ml(&_lock);
  return _hasNewError;
}


void Debug::clearHasNewError() {
  QMutexLocker ml(&_lock);
  _hasNewError = false;
}

}
// vim: ts=2 sw=2 et
