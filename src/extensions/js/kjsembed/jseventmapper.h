// -*- c++ -*-

/*
 *  Copyright (C) 2001-2003, Richard J. Moore <rich@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef JSEVENTMAPPER_H
#define JSEVENTMAPPER_H

#include <qdict.h>
#include <qevent.h>
#include <qintdict.h>

#include <kjs/identifier.h>
#include <kjsembed/jsbinding.h>

namespace KJSEmbed {

/**
 * Maintains a map between the types of QEvent and the names of their event
 * handlers.
 *
 * @author Richard Moore, rich@kde.org
 */
class JSEventMapper
{
public:
    JSEventMapper();
    virtual ~JSEventMapper();

    /** 
     * Adds an event to the map. The event handler has the specified name, and
     * the event has the specified type.
     */
    void addEvent( const KJS::Identifier &name, QEvent::Type t );

    /** Returns true iff the specified name is the identifier for an event handler. */
    bool isEventHandler( const KJS::Identifier &name ) const {
	return handlerToEvent.find( name.qstring() ) ? true : false; 
    }

    /** Returns the type of the events handled by the specified handler. */
    QEvent::Type findEventType( const KJS::Identifier &name ) const;

    KJS::Identifier *findEventHandler( QEvent::Type t ) const { return eventToHandler.find(t); }

private:
    QDict<uint> handlerToEvent;
    QIntDict<KJS::Identifier> eventToHandler;
    class JSEventMapperPrivate *d;
};

} // namespace KJSEmbed

#endif // JSEVENTMAPPER_H

