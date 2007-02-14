/***************************************************************************
                                 kstdebug.h
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

#ifndef KSTDEBUG_H
#define KSTDEBUG_H

#include <config.h>

#include <qdatetime.h>
#include <qguardedptr.h>
#include <qobject.h>
#include <qmutex.h>

#include <kstaticdeleter.h>
#include <ksttimers.h>

#include "kst_export.h"

// This class has to be threadsafe
class KST_EXPORT KstDebug : public QObject {
  Q_OBJECT
  friend class KStaticDeleter<KstDebug>;
  public:
    enum LogLevel { Unknown = 0, Notice = 1, Warning = 2, Error = 4, Debug = 8, None = 16384 };
    struct LogMessage {
      QDateTime date;
      QString msg;
      LogLevel level;
    };
    static KstDebug *self();

    void clear();
    void log(const QString& msg, LogLevel level = Notice);
    void setLimit(bool applyLimit, int limit);   
    QString text();
    void sendEmail();

    int logLength() const;
    QValueList<LogMessage> messages() const;
    KstDebug::LogMessage message(unsigned n) const;
    QStringList dataSourcePlugins() const;
    QString label(LogLevel level) const;
    const QString& kstRevision() const;

    int limit() const;

    bool hasNewError() const;
    void clearHasNewError();

#ifdef BENCHMARK
    QMap<QString,int>& drawCounter() { return _drawCounter; }
#endif

  protected:
    friend class KstApp;
    void setHandler(QObject *handler);

  private:
    KstDebug();
    ~KstDebug();

    static KstDebug *_self;
    QValueList<LogMessage> _messages;
    bool _applyLimit;
    bool _hasNewError;
    int _limit;
    mutable QMutex _lock;
#ifdef BENCHMARK
    // If this is ever public we can't do this
    QMap<QString,int> _drawCounter;
#endif
    QGuardedPtr<QObject> _handler;
    QString _kstRevision;
};


#endif

// vim: ts=2 sw=2 et
