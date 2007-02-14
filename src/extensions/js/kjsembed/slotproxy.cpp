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

#include <kjs/interpreter.h>

#ifndef QT_ONLY
#include <kurl.h>
#else
#include "qtstubs.h"
#endif // QT_ONLY

#include "jsopaqueproxy.h"
#include "global.h"
#include "jsbinding.h"
#include "jsfactory.h"
#include "jsobjectproxy.h"
#include "slotproxy.h"

#include "kjsembedpart.h"

#ifndef QT_ONLY
#include "slotproxy.moc"
#endif // QT_ONLY

namespace KJSEmbed {

JSSlotProxy::JSSlotProxy( QObject *parent, const char *name )
    : QObject( parent, name ? name : "slot_proxy" ),
      proxy(0), js(0)
{
}

JSSlotProxy::JSSlotProxy( QObject *parent, const char *name, JSObjectProxy *prx )
    : QObject( parent, name ? name : "slot_proxy" ),
      proxy(prx), js( prx ? prx->interpreter() : 0 )
{
}

JSSlotProxy::~JSSlotProxy()
{
}

void JSSlotProxy::slot_void()
{
    execute( KJS::List() );
}

void JSSlotProxy::slot_bool( bool b )
{
    KJS::List args;
    args.append( KJS::Boolean(b) );
    execute( args );
}

void JSSlotProxy::slot_string( const QString &s )
{
    KJS::List args;
    args.append( KJS::String(s) );
    execute( args );
}

void JSSlotProxy::slot_int( int i )
{
    KJS::List args;
    args.append( KJS::Number(i) );
    execute( args );
}

void JSSlotProxy::slot_uint( uint i )
{
    KJS::List args;
    args.append( KJS::Number(i) );
    execute( args );
}

void JSSlotProxy::slot_long( long i )
{
    KJS::List args;
    args.append( KJS::Number(i) );
    execute( args );
}

void JSSlotProxy::slot_ulong( ulong i )
{
    KJS::List args;
    args.append( KJS::Number(i) );
    execute( args );
}

void JSSlotProxy::slot_double( double d )
{
    KJS::List args;
    args.append( KJS::Number(d) );
    execute( args );
}

void JSSlotProxy::slot_color( const QColor &color )
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), color) );
    execute( args );
}

void JSSlotProxy::slot_font( const QFont &font )
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), font) );
    execute( args );
}
}// namespace KJSEmbed


void KJSEmbed::JSSlotProxy::slot_point( const QPoint & point )
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), point) );
    execute( args );
}

void KJSEmbed::JSSlotProxy::slot_rect( const QRect & rec )
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), rec) );
    execute( args );
}

void KJSEmbed::JSSlotProxy::slot_size( const QSize & size )
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), size) );
    execute( args );
}

void KJSEmbed::JSSlotProxy::slot_pixmap( const QPixmap & pix )
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), QVariant( pix ) ) );
    execute( args );
}

void KJSEmbed::JSSlotProxy::slot_url( const KURL & url )
{
    KJS::List args;
#ifndef QT_ONLY
    args.append( convertToValue(js->globalExec(), url.prettyURL() ));
#else // QT_ONLY
    args.append( convertToValue(js->globalExec(), url.toString() ));
#endif // QT_ONLY
    execute( args );
}

void KJSEmbed::JSSlotProxy::slot_intint( int arg, int arg1)
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), arg ));
    args.append( convertToValue(js->globalExec(), arg1 ));
    execute( args );
}
void KJSEmbed::JSSlotProxy::slot_intbool( int arg, bool arg1)
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), arg ));
    args.append( convertToValue(js->globalExec(), arg1 ));
    execute( args );
}

void KJSEmbed::JSSlotProxy::slot_intintint( int arg, int arg1, int arg2)
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), arg ));
    args.append( convertToValue(js->globalExec(), arg1 ));
    args.append( convertToValue(js->globalExec(), arg2 ));
    execute( args );
}

void KJSEmbed::JSSlotProxy::execute( const KJS::List &args )
{
    KJS::ExecState *exec = js->globalExec();
    KJS::Value val = obj.get( exec, KJS::Identifier(KJS::UString(method)) );
    KJS::Object fun = val.toObject( exec );

    if ( !fun.implementsCall() ) {
	kdDebug(80001) << "Target does not support call, class "
		       << fun.toString(exec).qstring() << " method " << method << endl;
	return;
    }

    fun.call( exec, obj, args );

    if ( exec->hadException() ) {
	kdWarning( 80001 ) << "Exception in slot function '" << method << "', "
			   << exec->exception().toString(exec).qstring() << endl;

	exec->clearException();
    }
}

void KJSEmbed::JSSlotProxy::slot_widget( QWidget * widget )
{
    KJS::List args;
    if ( widget )
	args.append( proxy->part()->factory()->createProxy( js->globalExec(), widget ) );
    else
	args.append( KJS::Null() );

    execute( args );
}

void KJSEmbed::JSSlotProxy::slot_datetime( const QDateTime & dateTime )
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), QVariant( dateTime ) ));
    execute( args );
}

void KJSEmbed::JSSlotProxy::slot_datedate( const QDate & date1, const QDate & date2 )
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), QVariant( date1 ) ));
    args.append( convertToValue(js->globalExec(), QVariant( date2 ) ));
    execute( args );
}

void KJSEmbed::JSSlotProxy::slot_colorstring( const QColor & color, const QString & string )
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), color ));
    args.append( convertToValue(js->globalExec(), string ));
    execute( args );
}

void KJSEmbed::JSSlotProxy::slot_image( const QImage & image )
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), QVariant( image ) ));
    execute( args );
}

void KJSEmbed::JSSlotProxy::slot_date( const QDate & date )
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), QVariant( date ) ));
    execute( args );
}

void KJSEmbed::JSSlotProxy::slot_time( const QTime &time )
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), QVariant( time ) ));
    execute( args );
}

void KJSEmbed::JSSlotProxy::slot_variant( const QVariant & variant )
{
    KJS::List args;
    args.append( convertToValue(js->globalExec(), variant ) );
    execute( args );
}


