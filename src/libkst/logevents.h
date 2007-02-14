/***************************************************************************
                                logevents.h
                              ----------------
    begin                : Apr 01 2005
    copyright            : (C) 2005 The University of Toronto
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

#ifndef LOGEVENTS_H
#define LOGEVENTS_H

#include "kstdebug.h"
#include "kstevents.h"

class LogEvent : public QEvent {
  public:
    enum LogEventType { Unknown = 0, LogAdded, LogCleared };

    LogEvent(LogEventType et) : QEvent(QEvent::Type(KstEventTypeLog)), _eventType(et) {}
    virtual ~LogEvent() {}

    LogEventType _eventType;
    KstDebug::LogMessage _msg;
};


#endif

// vim: ts=2 sw=2 et
