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

#include <qobject.h>
#include <qobjectlist.h>
#include <qdialog.h>
#include <qlistbox.h>
#include <qlistview.h>
#include <qmetaobject.h>
#include <qregexp.h>
#include <qsignal.h>
#include <qstrlist.h>
#include <qtabwidget.h>
#include <qtimer.h>
#include <qvariant.h>

#include <private/qucom_p.h>
#include <private/qucomextra_p.h>

#include <kjs/interpreter.h>
#include <kjs/types.h>
#include <kjs/ustring.h>

#include "kjsembedpart.h"
#include "jssecuritypolicy.h"

#include "global.h"
#include "jsfactory.h"
#include "slotproxy.h"
#include "slotutils.h"
#include "jsobjectproxy.h"
#include "jsobjectproxy_imp.h"

using namespace KJS;

namespace KJSEmbed {
namespace Bindings {

typedef JSProxy::MethodTable MethodTable;

//
// Factory methods that add the bindings.
//

void JSObjectProxyImp::addBindingsTree( KJS::ExecState *exec, KJS::Object &object, JSObjectProxy *proxy )
{
    MethodTable methods[] = {
	{ MethodParent, "parent" },
	{ MethodChildCount, "childCount" },
	{ MethodChild, "child" },
	{ MethodChildren, "children" },
	{ MethodIsWidgetType, "isWidgetType" },
	{ MethodClassName, "className" },
	{ MethodSuperClassName, "superClassName" },
	{ 0, 0 }
    };

    int i = 0;
    do {
	JSObjectProxyImp *obj = new JSObjectProxyImp( exec, methods[i].id, proxy );
	obj->setName( KJS::Identifier( methods[i].name ) );
	object.put( exec, methods[i].name, KJS::Object(obj) );
	i++;
    } while( methods[i].id );
}

void JSObjectProxyImp::addBindingsDOM( KJS::ExecState *exec, KJS::Object &object, JSObjectProxy *proxy )
{
    MethodTable methods[] = {
	{ MethodGetParentNode, "getParentNode" },
	{ MethodGetElementById, "getElementById" },
	{ MethodHasAttribute, "hasAttribute" },
	{ MethodGetAttribute, "getAttribute" },
	{ MethodSetAttribute, "setAttribute" },
	{ 0, 0 }
    };

    int i = 0;
    do {
	JSObjectProxyImp *obj = new JSObjectProxyImp( exec, methods[i].id, proxy );
	obj->setName( KJS::Identifier( methods[i].name ) );
	object.put( exec, methods[i].name, KJS::Object(obj) );
	i++;
    } while( methods[i].id );
}

void JSObjectProxyImp::addBindingsConnect( KJS::ExecState *exec, KJS::Object &object, JSObjectProxy *proxy )
{
    MethodTable methods[] = {
	{ MethodConnect, "connect" },
	{ MethodDisconnect, "disconnect" },
	{ MethodSignals, "signals" },
	{ MethodSlots, "slots" },
	{ 0, 0 }
    };

    int i = 0;
    do {
	JSObjectProxyImp *obj = new JSObjectProxyImp( exec, methods[i].id, proxy );
	obj->setName( KJS::Identifier( methods[i].name ) );
	object.put( exec, methods[i].name, KJS::Object(obj) );
	i++;
    } while( methods[i].id );
}

//
// The real implementation
//

JSObjectProxyImp::JSObjectProxyImp( KJS::ExecState *exec, int mid, JSObjectProxy *parent )
    : JSProxyImp(exec), id(mid), proxy(parent), obj(parent->obj)
{
}

JSObjectProxyImp::JSObjectProxyImp( KJS::ExecState *exec,
				    int mid, const QCString &name, JSObjectProxy *parent )
    : JSProxyImp(exec), id(mid), slotname(name), proxy(parent), obj(parent->obj)
{
}

JSObjectProxyImp::JSObjectProxyImp( KJS::ExecState *exec,
				    int mid, int sid, const QCString &name, JSObjectProxy *parent )
    : JSProxyImp(exec), id(mid), sigid(sid), slotname(name), proxy(parent), obj(parent->obj)
{
}

JSObjectProxyImp::JSObjectProxyImp( KJS::ExecState *exec,
				    int mid, const char *ret, int sid, const QCString &name,
				    JSObjectProxy *parent )
    : JSProxyImp(exec), id(mid), rettype(ret), sigid(sid), slotname(name), proxy(parent), obj(parent->obj)
{
}

//
// Custom methods.
//

KJS::Value JSObjectProxyImp::children( KJS::ExecState *exec, KJS::Object &, const KJS::List & )
{
    KJS::List items;
    const QObjectList *kids = obj->children();
    if ( kids ) {
        QObjectList l( *kids );

	for ( uint i = 0 ; i < l.count() ; i++ ) {
	    QObject *child = l.at( i );
	    QCString nm = ( child ? child->name() : "<null>" );
	    items.append( KJS::String( QString(nm) ) );
	}
    }

    return KJS::Object( proxy->js->builtinArray().construct( exec, items ) );
}

KJS::Value JSObjectProxyImp::properties( KJS::ExecState *exec, KJS::Object &, const KJS::List & )
{
    KJS::List items;
    QMetaObject *mo = obj->metaObject();
    QStrList propList( mo->propertyNames( true ) );

    for ( QStrListIterator iter(propList); iter.current(); ++iter ) {

	QCString name = iter.current();
	int propid = mo->findProperty( name.data(), true );
	if ( propid != -1 ) {
	    items.append( KJS::String( QString(name) ) );
	}
    }

    return KJS::Object( proxy->js->builtinArray().construct( exec, items ) );
}

KJS::Value JSObjectProxyImp::callCustomSlot( KJS::ExecState *, KJS::Object &, const KJS::List & )
{
    return KJS::Null();
}

//
// Invoke a slot or method.
//

KJS::Value JSObjectProxyImp::callSlot( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    return JSSlotUtils::invokeSlot( exec, self, args, this );
}

KJS::Value JSObjectProxyImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    if ( !proxy->isAllowed(exec->interpreter()) ) {
	kdWarning() << "JSObjectProxy::Method call from unknown interpreter!" << endl;
	return KJS::Null();
    }

    if (obj.isNull()) {
        kdWarning() << "JSObjectProxy::Method call on null object"<<endl;
        return KJS::Null();
    }

    switch( id ) {
	case MethodParent:
	{
	    QObject *po = obj->parent();
	    if ( po && proxy->securityPolicy()->isObjectAllowed( proxy, po ) )
		return proxy->part()->factory()->createProxy( exec, po, proxy );

	    return KJS::Null();
	}
	break;
	case MethodChildCount:
	{
	    const QObjectList *kids = obj->children();
	    return kids ? KJS::Number( kids->count() )	: KJS::Number(0);
	}
	break;
	case MethodIsWidgetType:
	{
	    return KJS::Boolean( obj->isWidgetType() );
	}
	break;
	case MethodClassName:
	{
	    return KJS::String( obj->className() );
	}
	break;
	case MethodSuperClassName:
	{
	    return KJS::String( obj->metaObject()->superClassName() );
	}
	break;
	case MethodChildren:
	    return children( exec, self, args );
	    break;
	case MethodProps:
	    return properties( exec, self, args );
	    break;
	case MethodSlot:
	    return callSlot( exec, self, args );
	    break;
	case MethodChild:
	case MethodGetElementById:
	    return getElementById( exec, self, args );
	    break;
	case MethodGetParentNode:
	    return getParentNode( exec, self, args );
	    break;
	case MethodHasAttribute:
	    return hasAttribute( exec, self, args );
	    break;
	case MethodGetAttribute:
	    return getAttribute( exec, self, args );
	    break;
	case MethodSetAttribute:
	    return setAttribute( exec, self, args );
	    break;
	case MethodGetElementsByTagName:
	    return getElementsByTagName( exec, self, args );
	    break;
	case MethodSignals:
	    return signalz( exec, self, args );
	    break;
	case MethodSlots:
	    return slotz( exec, self, args );
	    break;
	case MethodConnect:
	    return connect( exec, self, args );
	case MethodDisconnect:
	    return disconnect( exec, self, args );
	default:
	    break;
    }

    return ObjectImp::call( exec, self, args );
}

//
// Connections
//

KJS::Value JSObjectProxyImp::signalz( KJS::ExecState *exec, KJS::Object &, const KJS::List & )
{
    KJS::List items;
    QMetaObject *mo = obj->metaObject();
    QStrList signalList( mo->signalNames( true ) );

    for ( QStrListIterator iter(signalList); iter.current(); ++iter ) {

	QCString name = iter.current();
	QString nm( name );

	int signalid = mo->findSignal( name.data(), true );
	if ( (signalid != -1) && (mo->signal( signalid, true )->access == QMetaData::Public) )
	    items.append( KJS::String(nm) );
    }

    return KJS::Object( proxy->js->builtinArray().construct( exec, items ) );
}

KJS::Value JSObjectProxyImp::slotz( KJS::ExecState *exec, KJS::Object &, const KJS::List & )
{
    KJS::List items;
    QMetaObject *mo = obj->metaObject();
    QStrList slotList( mo->slotNames( true ) );

    for ( QStrListIterator iter(slotList); iter.current(); ++iter ) {

	QCString name = iter.current();
	QString nm( name );

	int slotid = mo->findSlot( name.data(), true );
	if ( (slotid != -1) && (mo->slot( slotid, true )->access == QMetaData::Public) )
	    items.append( KJS::String(nm) );
    }

    return KJS::Object( proxy->js->builtinArray().construct( exec, items ) );
}

KJS::Boolean JSObjectProxyImp::connect( KJS::ExecState *exec,
					const KJS::Object &self, const KJS::List &args )
{
    // connect sender, sig, slot
    // connect sender, sig, recv, method

    if ( (args.size() != 3) && (args.size() != 4) )
    	return KJS::Boolean(false);

    // Source object and signal
    JSObjectProxy *sendproxy = JSProxy::toObjectProxy( args[0].imp() );
    QObject *sender = sendproxy ? sendproxy->object() : 0;
    QString sig = args[1].toString(exec).qstring();
    kdDebug( 80001 ) << "connecting C++ signal" << sig << endl;
    // Receiver and slot/signal
    KJS::Object recvObj;
    QString dest;
    if ( args.size() == 3 ) {
	recvObj = self.toObject(exec);
	dest = args[2].toString(exec).qstring();
    }
    else if ( args.size() == 4 ) {
	recvObj = args[2].toObject(exec);
	dest = args[3].toString(exec).qstring();
    }

    // Try to connect to C++ slot
    JSObjectProxy *recvproxy = JSProxy::toObjectProxy( recvObj.imp() );
    if ( recvproxy ) {
	QObject *recv = recvproxy ? recvproxy->object() : 0;
	bool ok = JSSlotUtils::connect( sender, sig.ascii(), recv, dest.ascii() );
	if ( ok )
	    return KJS::Boolean(true);
    }

    return connect( sender, sig.ascii(), recvObj, dest );
}

KJS::Boolean JSObjectProxyImp::connect( QObject *sender, const char *sig,
					const KJS::Object &recv, const QString &dest )
{
    kdDebug(80001) << "Trying signature '" << sig << "'." << endl;
    // Try to connect to JS method
    JSSlotProxy *slotp = new JSSlotProxy( sender );
    slotp->setInterpreter( proxy->interpreter() );
    slotp->setProxy( proxy );
    slotp->setObject( recv );
    slotp->setMethod( dest );

    int id = JSSlotUtils::findSignature( sig );
    if ( id == JSSlotUtils::SignatureNotSupported ) {
	kdWarning(80001) << "Connect with unknown signature '" << sig << "' failed" << endl;
	return KJS::Boolean( false );
    }

    bool ok;
    switch ( id ) {
	case JSSlotUtils::SignatureNone:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_void()" );
	    break;
	case JSSlotUtils::SignatureInt:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_int(int)" );
	    break;
	case JSSlotUtils::SignatureUInt:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_uint(uint)" );
	    break;
	case JSSlotUtils::SignatureLong:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_long(long)" );
	    break;
	case JSSlotUtils::SignatureULong:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_ulong(ulong)" );
	    break;
	case JSSlotUtils::SignatureBool:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_bool(bool)" );
	    break;
	case JSSlotUtils::SignatureDouble:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_double(double)" );
	    break;
	case JSSlotUtils::SignatureString:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_string(const QString&)" );
	    break;
	case JSSlotUtils::SignatureCString:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_charstar(const char*)" );
	    break;
	case JSSlotUtils::SignatureColor:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_color(const QColor&)" );
	    break;
	case JSSlotUtils::SignatureFont:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_font(const QFont&)" );
	    break;
	case JSSlotUtils::SignaturePoint:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_point(const QPoint&)" );
	    break;
	case JSSlotUtils::SignatureRect:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_rect(const QRect&)" );
	    break;
	case JSSlotUtils::SignatureSize:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_size(const QSize&)" );
	    break;
	case JSSlotUtils::SignaturePixmap:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_pixmap(const QPixmap&)" );
	    break;
	case JSSlotUtils::SignatureURL:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_url(const KURL&)" );
	    break;
	case JSSlotUtils::SignatureIntInt:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_intint(int, int)" );
	    break;
	case JSSlotUtils::SignatureIntBool:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_intbool(int, bool)" );
	    break;
	case JSSlotUtils::SignatureIntIntIntInt:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_intintint(int, int, int)" );
	    break;

	case JSSlotUtils::SignatureDate:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_date(const QDate&)" );
	    break;
	case JSSlotUtils::SignatureTime:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_time(const QTime&)" );
	    break;
	case JSSlotUtils::SignatureDateTime:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_datetime(const QDateTime&)" );
	    break;
	case JSSlotUtils::SignatureImage:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_image(const QImage&)" );
	    break;
	case JSSlotUtils::SignatureQWidget:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_widget(QWidget*)" );
	    break;
	case JSSlotUtils::SignatureDateDate:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_datedate(const QDate&, const QDate& )" );
	    break;
	case JSSlotUtils::SignatureColorString:
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_colorstring(const QColor&, const QString&)" );
	    break;
	case JSSlotUtils::SignatureCustom:
	{
		QString mangledSig = sig;
		mangledSig.remove(' ')
			.remove("const")
			.remove('&')
			.remove('*');
		mangledSig = mangledSig.lower();

		kdDebug(80001) << "Custom slot signature: " << mangledSig << endl;
		break;
	}
	default:
	    kdWarning(80001) << "Unsupported signature '" << sig << "' connected with no args" << endl;
	    ok = JSSlotUtils::connect( sender, sig, slotp, "slot_none()" );
	    break;
    }

    if ( !ok ) {
	kdDebug(80001) << "Error connecting '" << sig << "'" << endl;
    }

    return KJS::Boolean(ok);
}

KJS::Boolean JSObjectProxyImp::disconnect( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    // disconnect sender, sig, slot
    // disconnect sender, sig, recv, method

    if ( (args.size() != 3) && (args.size() != 4) )
    	return KJS::Boolean(false);

    // Source object and signal
    JSObjectProxy *sendproxy = JSProxy::toObjectProxy( args[0].imp() );
    QObject *sender = sendproxy ? sendproxy->object() : 0;
    QString sig = args[1].toString(exec).qstring();

    // Receiver and slot/signal
    QObject *recv=0;
    QString dest;
    if ( args.size() == 3 ) {
	JSObjectProxy *recvproxy = JSProxy::toObjectProxy( self.imp() );
	recv = recvproxy ? recvproxy->object() : 0;
	dest = args[2].toString(exec).qstring();
    }
    else if ( args.size() == 4 ) {
	JSObjectProxy *recvproxy = JSProxy::toObjectProxy( args[2].imp() );
	recv = recvproxy ? recvproxy->object() : 0;
	dest = args[3].toString(exec).qstring();
    }

    return JSSlotUtils::disconnect( exec, self, sender, sig.ascii(), recv, dest.ascii() );
}

//
// DOM methods
//

KJS::Value JSObjectProxyImp::getParentNode( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( args.size())
	return KJS::Null();

    QObject *parent = obj->parent();
    if ( parent && proxy->securityPolicy()->isObjectAllowed( proxy, parent ) )
	return proxy->part()->factory()->createProxy( exec, parent, proxy );

    return KJS::Null();
}

KJS::Value JSObjectProxyImp::getElementById( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( !args.size() )
	return KJS::Null();

    const QObjectList *kids = obj->children();
    if ( !kids )
	return KJS::Null();

    QObjectList l( *kids );
    QObject *child = 0;

    if ( args[0].isA( KJS::NumberType ) ) {
	uint i = args[0].toUInt32( exec );

	if ( i >= l.count()  )
	    return KJS::Null();

	child = l.at( i );
    }
    else {
	QString s = args[0].toString( exec ).qstring();
	child = obj->child( s.ascii() );
    }

    if ( child && proxy->securityPolicy()->isObjectAllowed( proxy, child ) ) {
	kdDebug(80001) << "Creating subproxy for child " << child->className() << endl;
	return proxy->part()->factory()->createProxy( exec, child, proxy );
    }

    return KJS::Null();
}

KJS::Value JSObjectProxyImp::getElementsByTagName( KJS::ExecState *, KJS::Object &, const KJS::List & )
{
    return KJS::Null();
}

KJS::Value JSObjectProxyImp::hasAttribute( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( !args.size() )
	return KJS::Null();

    QMetaObject *meta = obj->metaObject();
    QString s = args[0].toString(exec).qstring();
    if ( meta->findProperty( s.ascii(), true ) != -1 )
	return KJS::Boolean(true);
    else
	return KJS::Boolean(false);
}

KJS::Value JSObjectProxyImp::getAttribute( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( !args.size() )
    {
        QString msg = i18n( "No property was defined." );
        return throwError(exec, msg);
    }

    QMetaObject *meta = obj->metaObject();
    QString s = args[0].toString(exec).qstring();
    kdDebug(80001) << "Get property " << s << " from " << obj->name() << endl;
    if ( meta->findProperty( s.ascii(), true ) == -1 )
    {
        QString msg = i18n( "Property '%1' could not be found." ).arg( s );
        return throwError(exec, msg,KJS::ReferenceError);
    }

    QVariant val = obj->property( s.ascii() );
    return convertToValue( exec, val );
}

KJS::Value JSObjectProxyImp::setAttribute( KJS::ExecState *exec, KJS::Object &, const KJS::List &args )
{
    if ( args.size() != 2 )
	return KJS::Boolean(false);

    QMetaObject *meta = obj->metaObject();
    QString s = args[0].toString(exec).qstring();

    if ( meta->findProperty( s.ascii(), true ) == -1 )
    {
        QString msg = i18n( "Property '%1' could not be found." ).arg( s );
        return throwError(exec, msg,KJS::GeneralError);
    }
    kdDebug(80001) << "Set property " << s << " from " << obj->name() << endl;
    QVariant val = convertToVariant( exec, args[1] );
    bool ok = obj->setProperty( s.ascii(), val );

    return KJS::Boolean(ok);
}

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:

