/*
 *  Copyright (C) 2003-2004, Richard J. Moore <rich@kde.org>
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

#include <qvariant.h>
#include "global.h"

#include "jsfactory.h"
#include "jseventmapper.h"
#include "jsobjectproxy.h"
#include "kjsembedpart.h"
#include "jsbinding.h"

#include "jsobjecteventproxy.h"
#ifndef QT_ONLY
#include "jsobjecteventproxy.moc"
#endif // QT_ONLY

namespace KJSEmbed {

// We'll need to increase this if we need to support any custom events
static const QEvent::Type QEVENT_TYPE_MAX = QEvent::TabletRelease;

JSObjectEventProxy::JSObjectEventProxy( JSObjectProxy *parent, const char *name )
    : QObject( parent->object(), name ? name : "jsobjecteventproxy" ),
      proxy( parent ), eventMask( QEVENT_TYPE_MAX ), refcount(0)
{
    eventMask.fill( false );
}

JSObjectEventProxy::~JSObjectEventProxy()
{
}

bool JSObjectEventProxy::isFiltered( QEvent::Type t ) const
{
    return eventMask.testBit( t );
}

void JSObjectEventProxy::addFilter( QEvent::Type t )
{
    if ( !refcount )
	proxy->object()->installEventFilter( this );

    if ( !eventMask.testBit(t) ) {
	refcount++;
	eventMask.setBit( t );
    }

    kdDebug(80001) << "JSObjectEventProxy::addFilter done" << endl;
}

void JSObjectEventProxy::removeFilter( QEvent::Type t )
{
    eventMask.clearBit( t );
    refcount--;
    if ( !refcount ) {
	proxy->object()->removeEventFilter( this );
	deleteLater();
    }

    kdDebug(80001) << "JSObjectEventProxy::removeFilter done" << endl;
}

bool JSObjectEventProxy::eventFilter( QObject * /*watched*/, QEvent *e )
{
    if ( isFiltered(e->type()) )
	callHandler( e );

    return false;
}

KJS::Value JSObjectEventProxy::callHandler( QEvent *e )
{
// Be careful enabling this as if there are a lot of events then the event loop times
// out and the app crashes with 'Alarm Clock'.
//    kdDebug(80001) << "JSObjectEventProxy::callHandler() event type " << e->type() << endl;

    KJS::ExecState *exec = proxy->interpreter()->globalExec();
    KJS::Identifier *id = proxy->part()->factory()->eventMapper()->findEventHandler( e->type() );
   
    KJS::Object jsobj( proxy );
    KJS::Object fun = jsobj.get(exec, *id ).toObject( exec );

    if ( !fun.implementsCall() ) {
      QString msg = i18n( "Bad event handler: Object %1 Identifier %2 Method %3 Type: %4." )
	  .arg(jsobj.className().ascii()).arg(id->ascii()).arg(fun.className().ascii()).arg(e->type());
      return throwError(exec, msg, KJS::TypeError );
    }

    // Process args
    KJS::List args;
    KJS::Object obj = proxy->part()->factory()->createProxy( exec, e, proxy );
    args.append( obj );

    // Call handler
    KJS::Value result = fun.call( exec, jsobj, args );

    if ( exec->hadException() ) {
	kdWarning( 80001 ) << "Exception in event handler '" << id->qstring() << "', "
			   << exec->exception().toString(exec).qstring() << endl;

	exec->clearException();
    }

    return result;
}

} // KJSEmbed


