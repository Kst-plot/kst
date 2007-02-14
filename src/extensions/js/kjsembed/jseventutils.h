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

#ifndef KJSEMBED_JSEVENTUTILS_H
#define KJSEMBED_JSEVENTUTILS_H
#include "global.h"
#include <qevent.h>
#include <kjs/object.h>

namespace KJSEmbed {

class JSObjectProxy;
class JSFactory;

/**
 * Utility class that contains the methods for converting event types to JS.
 */
class KJSEMBED_EXPORT JSEventUtils
{
public:
    static KJS::Object convertEvent( KJS::ExecState *exec,
				     const QMouseEvent *ev, const JSObjectProxy *context );

    static KJS::Object convertEvent( KJS::ExecState *exec,
				     const QPaintEvent *ev, const JSObjectProxy *context );

    static KJS::Object convertEvent( KJS::ExecState *exec,
				     const QKeyEvent *ev, const JSObjectProxy *context );

    static KJS::Object convertEvent( KJS::ExecState *exec,
				     const QIMEvent *ev, const JSObjectProxy *context );

    static KJS::Object convertEvent( KJS::ExecState *exec,
				     const QResizeEvent *ev, const JSObjectProxy *context );

    static KJS::Object convertEvent( KJS::ExecState *exec,
				     const QFocusEvent *ev, const JSObjectProxy *context );

    static KJS::Object convertEvent( KJS::ExecState *exec,
				     const QCloseEvent *ev, const JSObjectProxy *context );

#ifdef ENABLE_CHILDEVENTS
    static KJS::Object convertEvent( KJS::ExecState *exec,
				     const QChildEvent *ev, const JSObjectProxy *context );
#endif

    static KJS::Object convertEvent( KJS::ExecState *exec,
				     const QMoveEvent *ev, const JSObjectProxy *context );

    static KJS::Object convertEvent( KJS::ExecState *exec,
				     const QWheelEvent *ev, const JSObjectProxy *context );

    static KJS::Object convertEvent( KJS::ExecState *exec,
				     const QEvent *ev, const JSObjectProxy *context );

    static KJS::Object convertEvent( KJS::ExecState *exec,
				     const QDropEvent *ev, const JSObjectProxy *context );

    static KJS::Object convertEvent( KJS::ExecState *exec,
				     const QDragMoveEvent *ev, const JSObjectProxy *context );

    static KJS::Object convertEvent( KJS::ExecState *exec,
				     const QTimerEvent *ev, const JSObjectProxy *context );

   static KJS::Object convertEvent( KJS::ExecState *exec,
				     const QContextMenuEvent *ev, const JSObjectProxy *context );







private:
    JSEventUtils() {}
    ~JSEventUtils() {}

    static JSFactory *factory( const JSObjectProxy *prx );
};

} // KJSEmbed

#endif // KJSEMBED_JSEVENTUTILS_H
