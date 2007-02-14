// -*- c++ -*-

/*
 *  Copyright (C) 2003, Richard J. Moore <rich@kde.org>
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

#ifndef KJSEMBED_JSOBJECTEVENTPROXY_H
#define KJSEMBED_JSOBJECTEVENTPROXY_H

#include <qbitarray.h>
#include <qobject.h>
#include <kjs/object.h>
#include <global.h>

namespace KJSEmbed {

class JSObjectProxy;

/**
 * Filters events for a QObject and forwards them to a JS handler.
 *
 * @version $Id$
 * @author Richard Moore, rich@kde.org
 */
class KJSEMBED_EXPORT JSObjectEventProxy : public QObject
{
    Q_OBJECT

public:
    JSObjectEventProxy( JSObjectProxy *parent, const char *name=0 );
    virtual ~JSObjectEventProxy();

    /** Returns true iff we forward the event type to JS. */
    bool isFiltered( QEvent::Type t ) const;

    /** Adds an event type to those we forward to JS. */
    void addFilter( QEvent::Type t );

    /**
     * Removes an event type from those we forward to JS. If there are no
     * event types left to forward then we self-destruct.
     */
    void removeFilter( QEvent::Type t );

    /** Reimplemented to forward events to JS. */
    bool eventFilter ( QObject *watched, QEvent *e );

protected:
    KJS::Value callHandler( QEvent *e );

private:
    JSObjectProxy *proxy;
    QBitArray eventMask;
    uint refcount;
    class JSObjectEventProxyPrivate *d;
};

} // KJSEmbed

#endif // KJSEMBED_JSOBJECTEVENTPROXY_H

