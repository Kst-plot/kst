/***************************************************************************
                               threadevents.h
                              ----------------
    begin                : Jan 06 2004
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

#ifndef THREADEVENTS_H
#define THREADEVENTS_H

#include "kstevents.h"
//Added by qt3to4:
#include <QEvent>
#include <Q3ValueList>

class ThreadEvent : public QEvent {
  public:
    enum ThreadEventType { Unknown = 0, UpdateDataDialogs, UpdateAllDialogs, Repaint, Done, NoUpdate };

    ThreadEvent(ThreadEventType et) : QEvent(QEvent::Type(KstEventTypeThread)), _eventType(et), _counter(-1) {}
    virtual ~ThreadEvent() {}

    ThreadEventType _eventType;
    Q3ValueList<KstBaseCurve*> _curves; // HACK: for temporary use in update reworking
    int _counter;
};


#endif

// vim: ts=2 sw=2 et
