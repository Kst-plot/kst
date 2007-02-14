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
#include <qtimer.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <qstringlist.h>

#include <private/qucom_p.h>
#include <private/qucomextra_p.h>

#ifndef QT_ONLY
#include <klistview.h>
#include <kurl.h>
#else // QT_ONLY
#include "qtstubs.h"
#endif // QT_ONLY

#include <kjs/interpreter.h>
#include <kjs/types.h>
#include <kjs/ustring.h>

#include "kjsembedpart.h"
#include "jssecuritypolicy.h"

#include "global.h"
#include "jsfactory.h"
#include "slotproxy.h"
#include "jsvalueproxy.h"
#include "jsopaqueproxy.h"

#include "jsobjectproxy_imp.h"

#include "slotutils.h"

namespace KJSEmbed {
namespace Bindings {

JSSlotUtils::JSSlotUtils()
{
}

JSSlotUtils::~JSSlotUtils()
{
}

bool JSSlotUtils::connect( QObject *sender, const char *sig, QObject *recv, const char *dest )
{
    if ( (!sender) || (!recv) )
	return false;

    // Source
    QString si = QString("2%1").arg(sig);
    const char *sigc = si.ascii();

    // Connect to slot
    if ( recv->metaObject()->findSlot(dest, true) >= 0 ) {
	QString sl = QString("1%1").arg(dest);
	const char *slotc = sl.ascii();

	//kdDebug(80001) << "connect: "<<sender->name()<<" "<<sigc<<", slot "<<recv->name()<<" "<<slotc<< endl;
	bool ok = QObject::connect( sender, sigc, recv, slotc );
	if (ok)
	    return true;
    }

    // Connect to signal
    if ( recv->metaObject()->findSignal(dest, true) >= 0 ) {
	QString si2 = QString("2%1").arg(dest);
	const char *sig2c = si2.ascii();

	kdDebug(80001) << "connect: "<<sender->name()<<" "<<sigc<<", sig "<<recv->name()<<" "<<sig2c<< endl;
	bool ok = QObject::connect( sender, sigc, recv, sig2c );
	if (ok)
	    return true;
    }

    return false;
}

KJS::Boolean JSSlotUtils::disconnect( KJS::ExecState */*exec*/, KJS::Object &/*self*/,
				    QObject *sender, const char *sig,
				    QObject *recv, const char *dest )
{
    if ( (!sender) || (!recv) )
	return KJS::Boolean(false);

    bool ok;

    // Source
    QString si = QString("2%1").arg(sig);
    const char *sigc = si.ascii();

    // Disconnect from slot
    if ( recv->metaObject()->findSlot(dest, true) >= 0 ) {
	QString sl = QString("1%1").arg(dest);
	const char *slotc = sl.ascii();

	//kdDebug(80001) << "disconnect: "<<sender->name()<<" "<<sigc<<", slot "<<recv->name()<<" "<<slotc<< endl;
	ok = QObject::disconnect( sender, sigc, recv, slotc );
	if (ok)
	    return KJS::Boolean(ok);
    }

    // Disonnect to signal
    if ( recv->metaObject()->findSignal(dest, true) >= 0 ) {
	QString si2("2");
	si2 = si2 + dest;
	const char *sig2c = si2.ascii();

	//kdDebug(80001)<< "disconnect: "<<sender->name()<<" "<<sigc<<", sig "<<recv->name()<<" "<<sig2c<< endl;
	ok = QObject::disconnect( sender, sigc, recv, sig2c );
	if (ok)
	    return KJS::Boolean(ok);
    }

    return KJS::Boolean(false);
}

KJS::Value JSSlotUtils::extractValue( KJS::ExecState *exec, QUObject *uo, JSObjectProxy *parent )
{
    QCString typenm( uo->type->desc() );
    kdDebug(80001) << "JSSlotUtils:extractValue() " << typenm << endl;

    if ( typenm == "null" )
	return KJS::Null();
    else if ( typenm == "bool" )
	return KJS::Boolean( static_QUType_bool.get(uo) );
    else if ( typenm == "int" )
	return KJS::Number( static_QUType_int.get(uo) );
    else if ( typenm == "double" )
	return KJS::Number( static_QUType_double.get(uo) );
    else if ( typenm == "charstar" )
	return KJS::String( static_QUType_charstar.get(uo) );
    else if ( typenm == "QString" )
	return KJS::String( static_QUType_QString.get(uo) );
    else if ( typenm == "QVariant" )
	return convertToValue( exec, static_QUType_QVariant.get(uo) );
    else if ( typenm == "ptr" ) {
	void *ptr = static_QUType_ptr.get(uo);

	// If it's a QObject and we know the parent
	if ( ptr && parent ) {
	    QObject *qobj = (QObject *)(ptr); // Crash in kst
	    return parent->part()->factory()->createProxy( exec, qobj, parent);
	}
    }

    kdWarning(80001) << "JSSlotUtils:extractValue() Failed (" << typenm << ")" << endl;
    QString msg = i18n( "Value of type (%1) is not supported." ).arg( typenm );
    throwError(exec, msg,KJS::TypeError);
    return KJS::Null();
}

//
// Add a value to a QUObject.
//

void JSSlotUtils::implantInt( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v )
{
    static_QUType_int.set( uo, v.toInteger( exec ) );
}

void JSSlotUtils::implantCharStar( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v )
{
    static_QUType_charstar.set( uo, v.toString( exec ).ascii() );
}

void JSSlotUtils::implantBool( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v )
{
    static_QUType_bool.set( uo, v.toBoolean( exec ) );
}

void JSSlotUtils::implantDouble( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v )
{
    static_QUType_double.set( uo, v.toNumber( exec ) );
}

void JSSlotUtils::implantQString( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v )
{
    static_QUType_QString.set( uo, v.toString(exec).qstring() );
}

void JSSlotUtils::implantCString( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v )
{
    static_QUType_charstar.set( uo, v.toString(exec).ascii() );
}

void JSSlotUtils::implantURL( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v,KURL *url )
{
    *url = QString( v.toString(exec).qstring() );
    static_QUType_ptr.set( uo, url );
}

void JSSlotUtils::implantColor( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QColor *color )
{
    bool ok;
    QString s( v.toString(exec).qstring() );

    if ( s.startsWith("#") ) {

	QRegExp re("#([0-9a-f][0-9a-f]){3,4}");
	re.setCaseSensitive( false );

	if ( re.search(s) != -1 ) {
	    uint r = re.cap(1).toUInt(&ok, 16);
	    uint g = re.cap(2).toUInt(&ok, 16);
	    uint b = re.cap(3).toUInt(&ok, 16);

	    if ( re.numCaptures() == 3 )
		*color = QColor(r,g,b);
	    else if ( re.numCaptures() == 4 ) {
		uint a = re.cap(4).toUInt(&ok, 16);
		uint pix = r;
		pix = pix << 8;
		pix = pix | g;
		pix = pix << 8;
		pix = pix | b;
		pix = pix << 8;
		pix = pix | a;

		*color = QColor( qRgba(r,g,b,a), pix );
	    }
	}

    }
    else {
	// Try for a named color
	*color = QColor( s );
    }

    static_QUType_ptr.set( uo, color );
}

void JSSlotUtils::implantPoint( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QPoint *point )
{
    KJS::Object o = v.toObject( exec );
    if ( !o.isValid() )
	return;

    int x, y;
    KJS::Identifier zero("0"), one("1"), ex("x"), wi("y");

    if ( o.hasProperty(exec, zero) && o.hasProperty(exec, one) ) {
	x = o.get( exec, zero ).toInteger(exec);
	y = o.get( exec, one ).toInteger(exec);
    }
    else if ( o.hasProperty(exec, ex) && o.hasProperty(exec, wi) ) {
	x = o.get( exec, ex ).toInteger(exec);
	y = o.get( exec, wi ).toInteger(exec);
    }
    else
	return;

    *point = QPoint( x, y );
    static_QUType_ptr.set( uo, point );
}

void JSSlotUtils::implantSize( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QSize *size )
{
    KJS::Object o = v.toObject( exec );
    if ( !o.isValid() )
	return;

    int w, h;
    KJS::Identifier zero("0"), one("1"), wid("width"), hih("height");

    if ( o.hasProperty(exec, zero) && o.hasProperty(exec, one) ) {
	w = o.get( exec, zero ).toInteger(exec);
	h = o.get( exec, one ).toInteger(exec);
    }
    else if ( o.hasProperty(exec, wid) && o.hasProperty(exec, hih) ) {
	w = o.get( exec, wid ).toInteger(exec);
	h = o.get( exec, hih ).toInteger(exec);
    }
    else
	return;

    *size = QSize( w, h );
    static_QUType_ptr.set( uo, size );
}

void JSSlotUtils::implantRect( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QRect *rect )
{
    KJS::Object o = v.toObject( exec );
    if ( !o.isValid() )
	return;

    int x, y, w, h;
    KJS::Identifier zero("0"), one("1"), two("2"), three("3");
    KJS::Identifier ex("x"), wi("y"), wid("width"), hih("height");

    if ( o.hasProperty(exec, zero) && o.hasProperty(exec, one)
	 && o.hasProperty(exec, two) && o.hasProperty(exec, three) ) {
	x = o.get( exec, zero ).toInteger(exec);
	y = o.get( exec, one ).toInteger(exec);
	w = o.get( exec, two ).toInteger(exec);
	h = o.get( exec, three ).toInteger(exec);
    }
    else if ( o.hasProperty(exec, ex) && o.hasProperty(exec, wi)
	      && o.hasProperty(exec, wid) && o.hasProperty(exec, hih) ) {
	x = o.get( exec, ex ).toInteger(exec);
	y = o.get( exec, wi ).toInteger(exec);
	w = o.get( exec, wid ).toInteger(exec);
	h = o.get( exec, hih ).toInteger(exec);
    }
    else
	return;

    *rect = QRect( x, y, w, h );
    static_QUType_ptr.set( uo, rect );
}

void JSSlotUtils::implantDate(  KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QDate *date )
{
	QDateTime jsDate = convertDateToDateTime(exec, v);
	date->setYMD( jsDate.date().year(), jsDate.date().month(), jsDate.date().day() );
	static_QUType_ptr.set( uo, date );
}

void JSSlotUtils::implantStringList(KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QStringList *lst )
{
  *lst = convertArrayToStringList(exec,v);
  static_QUType_ptr.set(uo,lst);
}

void JSSlotUtils::implantTime( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QTime *time )
{
	QDateTime jsDate = convertDateToDateTime(exec, v);
	time->setHMS( jsDate.time().hour(), jsDate.time().minute(), jsDate.time().second() );
	static_QUType_ptr.set( uo, time );
}

void JSSlotUtils::implantDateTime( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QDateTime *datetime  )
{
	QDateTime jsDate = convertDateToDateTime(exec, v);
	datetime->setDate( jsDate.date() );
	datetime->setTime( jsDate.time() );
	static_QUType_ptr.set( uo, datetime );
}

void JSSlotUtils::implantPixmap( KJS::ExecState *exec, QUObject *uo, const KJS::Value &v, QPixmap *pix )
{
    *pix = convertToVariant(exec, v).toPixmap() ;
    static_QUType_ptr.set( uo, pix );
}

bool JSSlotUtils::implantQVariant( KJS::ExecState *, QUObject *uo, const KJS::Value &v)
{
    JSValueProxy *prx = JSProxy::toValueProxy( v.imp() );
    if ( !prx )
	return false;
    kdDebug(80001) << "We got a " << prx->typeName() << endl;

    static_QUType_QVariant.set( uo, prx->toVariant() );
    return true;
}

bool JSSlotUtils::implantValueProxy( KJS::ExecState *, QUObject *uo,
				     const KJS::Value &val, const QString &clazz )
{
    JSValueProxy *prx = JSProxy::toValueProxy( val.imp() );
    if ( !prx )
	return false;
    if ( prx->typeName() != clazz )
	return false;

    kdDebug(80001) << "We got a " << prx->typeName() << " and is valid " << prx->toVariant().isValid() << endl;
    QVariant var = prx->toVariant();
    kdDebug(80001) << "We got a " << var.typeName()<< " and is valid " << var.isValid() << endl;
    static_QUType_QVariant.set( uo, var );
    return var.isValid();
}

bool JSSlotUtils::implantOpaqueProxy( KJS::ExecState *, QUObject *uo,
				      const KJS::Value &v, const QString &clazz )
{
    JSOpaqueProxy *prx = JSProxy::toOpaqueProxy( v.imp() );
    if ( !prx )
	return false;
    if ( prx->typeName() != clazz )
	return false;

    static_QUType_ptr.set( uo, prx->toNative<void>() );
    return true;
}

bool JSSlotUtils::implantObjectProxy( KJS::ExecState *, QUObject *uo,
				      const KJS::Value &v, const QString &clazz )
{
    JSObjectProxy *prx = JSProxy::toObjectProxy( v.imp() );
    if ( !prx )
	return false;

    if ( !prx->object()->inherits( clazz.latin1() ) )
	return false;

    static_QUType_ptr.set( uo, prx->toObjectProxy()->object() );
    return true;
}

int JSSlotUtils::findSignature( const QString &sig )
{
    // No args
    if ( sig.contains("()") )
	return SignatureNone;

    // One arg
    if ( sig.contains("(int)") )
	return SignatureInt;
    else if ( sig.contains("(uint)") )
	return SignatureUInt;
    else if ( sig.contains("(long)") )
	return SignatureLong;
    else if ( sig.contains("(ulong)") )
	return SignatureULong;
    else if ( sig.contains("(bool)") )
	return SignatureBool;
    else if ( sig.contains("(double)") )
	return SignatureDouble;

    else if ( sig.contains("(const QString&)") )
	return SignatureString;
    else if ( sig.contains("(const QCString&)") )
	return SignatureCString;
    else if ( sig.contains("(const char*)") )
	return SignatureCString;
    else if ( sig.contains("(const KURL&)") )
	return SignatureURL;
    else if ( sig.contains("(const QColor&)") )
	return SignatureColor;
    else if ( sig.contains("(const QPoint&)") )
	return SignaturePoint;
    else if ( sig.contains("(const QRect&)") )
	return SignatureRect;
    else if ( sig.contains("(const QSize&)") )
	return SignatureSize;
    else if ( sig.contains("(const QPixmap&)") )
	return SignaturePixmap;
    else if ( sig.contains("(const QFont&)") )
	return SignatureFont;
    else if ( sig.contains("(const QDate&)") )
	return SignatureDate;
    else if ( sig.contains("(const QTime&)") )
	return SignatureTime;
    else if ( sig.contains("(const QDateTime&)") )
	return SignatureDateTime;
    else if ( sig.contains("(const QImage&)") )
	return SignatureImage;
    else if ( sig.contains("(QWidget*)") )
	return SignatureQWidget;
    // Two args
    else if ( sig.contains("(const QDate&, const QDate&)") )
	return SignatureDateDate;
    else if ( sig.contains("(const QColor&, const QString&)") )
	return SignatureColorString;

    else if ( sig.contains("(const QString&,const QString&,const QString&)") )
	return SignatureStringStringString;

    else if ( sig.contains("(const QString&,const QString&)") )
	return SignatureStringString;
    else if ( sig.contains("(int,int)") )
	return SignatureIntInt;
    else if ( sig.contains("(int,int,int,int)") )
	return SignatureIntIntIntInt;
    else if ( sig.contains("(int,int,int,int,int)") )
	return SignatureIntIntIntIntInt;
    else if ( sig.contains("(int,int,int,int,bool)") )
	return SignatureIntIntIntIntBool;

    else if ( sig.contains("(const QString&,int)") )
	return SignatureStringInt;
    else if ( sig.contains("(const QString&,uint)") )
	return SignatureStringInt;

    else if ( sig.contains("(const KURL&,const KURL&)") )
	return SignatureURLURL;
    else if ( sig.contains("(const KURL&,const QString&)") )
	return SignatureURLString;
    else if ( sig.contains("(const QString&,const KURL&)") )
	return SignatureStringURL;
    else if ( sig.contains("(const QRect&,bool)") )
	return SignatureRectBool;
    else if ( sig.contains("(const QString&,bool)") )
	return SignatureStringBool;
    else if ( sig.contains("(int,bool)") )
	return SignatureIntBool;
    else if ( sig.contains("(int,int,bool)") )
	return SignatureIntIntBool;
    else if ( sig.contains("(int,int,const QString&)") )
	return SignatureIntIntString;
    else if ( sig.contains("(const QString&,bool,int)") )
	return SignatureStringBoolInt;
    else if ( sig.contains("(const QString&,bool,bool)") )
	return SignatureStringBoolBool;
    else if ( sig.contains("(const QString&,int,int)") )
	return SignatureStringIntInt;
    else if ( sig.contains("(int,const QColor&,bool)") )
	return SignatureIntColorBool;
    else if ( sig.contains("(int,const QColor&)") )
	return SignatureIntColor;

    else if ( sig.contains("(int,int,float,float)") )
	return SignatureIntIntFloatFloat;
    else if ( sig.contains("(const QString&,bool,bool,bool)") )
	return SignatureStringBoolBoolBool;
    else if ( sig.contains("(int,int,int,int,int,int)") )
	return SignatureIntIntIntIntIntInt;

    // Handle anything that falls through
    if ( sig.contains("QString") || sig.contains("QColor") ||
	sig.contains("int") || sig.contains("bool") ||
        sig.contains("float") || sig.contains("KURL") ||
        sig.contains("QVariant") || sig.contains("QSize") ||
        sig.contains("QRect") || sig.contains("QPixmap") ||
        sig.contains("QCString") || sig.contains("QPoint") ||
	sig.contains("double") || sig.contains("QFont") ||
	sig.contains("QDate") || sig.contains("QTime") ||
	sig.contains("QDateTime") || sig.contains("QStringList") ||
	sig.contains("QWidget") || sig.contains("QObject") ||
	sig.contains("QPen") || sig.contains("QImage") )
	   return SignatureCustom;

    kdWarning(80001) << "findSignature: not supported type " << sig << endl;
    return SignatureNotSupported;
}

#define MAX_SUPPORTED_ARGS (12)

KJS::Value JSSlotUtils::invokeSlot( KJS::ExecState *exec, KJS::Object &, const KJS::List &args,
				    JSObjectProxyImp *proxyimp )
{
    QUObject uo[ MAX_SUPPORTED_ARGS ] = { QUObject(), QUObject(), QUObject(),
					  QUObject(), QUObject(), QUObject(),
					  QUObject(), QUObject(), QUObject(),
					  QUObject(), QUObject(), QUObject() };

    KJS::Value retValue;
    JSObjectProxy *proxy = proxyimp->objectProxy();
    QCString slotname( proxyimp->slotName() );
    int sigid = proxyimp->signature();
    QPtrList<uint> uintlist;
    uintlist.setAutoDelete(true);
    QObject *obj = proxy->object();
    int slotid = obj->metaObject()->findSlot( slotname, true );
    if ( slotid == -1 )
    {
	QString msg = i18n( "Slot %1 was not found." ).arg( slotname );
  return throwError(exec, msg,KJS::ReferenceError);
    }

    if ( args.size() > MAX_SUPPORTED_ARGS )
    {
	QString msg = i18n( "Slots with more than 1 argument are not supported.", "Slots with more than %n arguments are not supported.", MAX_SUPPORTED_ARGS );
  return throwError(exec, msg,KJS::ReferenceError);
    }
    // Keep args in scope for duration of the method.
    KURL url[MAX_SUPPORTED_ARGS];
    QColor color[MAX_SUPPORTED_ARGS];
    QPoint point[MAX_SUPPORTED_ARGS];
    QSize size[MAX_SUPPORTED_ARGS];
    QRect rect[MAX_SUPPORTED_ARGS];
    QDate date[MAX_SUPPORTED_ARGS];
    QDateTime datetime[MAX_SUPPORTED_ARGS];
    QTime time[MAX_SUPPORTED_ARGS];
    QPixmap pix[MAX_SUPPORTED_ARGS];
    QStringList slst[MAX_SUPPORTED_ARGS];
    
    bool notsupported = true;

    if ( args.size() == 1 ) {
	//kdDebug( 80001 ) << "One Arg" << endl;
	switch (sigid) {
	    case SignatureInt:
		implantInt( exec, uo+1, args[0] );
		break;
	    case SignatureBool:
		implantBool( exec, uo+1, args[0] );
		break;
	    case SignatureString:
		implantQString( exec, uo+1, args[0] );
		break;
	    case SignatureCString:
		implantCString( exec, uo+1, args[0] );
		break;
	    case SignatureDouble:
		implantDouble( exec, uo+1, args[0] );
		break;
	    case SignatureURL:
		implantURL( exec, uo+1, args[0], &url[0] );
		break;
	    case SignatureColor:
		implantColor( exec, uo+1, args[0], &color[0] );
		break;
	    case SignaturePoint:
		implantPoint( exec, uo+1, args[0], &point[0] );
		break;
	    case SignatureSize:
		implantSize( exec, uo+1, args[0], &size[0] );
		break;
	    case SignatureRect:
		implantRect( exec, uo+1, args[0], &rect[0] );
		break;
	    default:
		notsupported = true;
		break;
	}

    } else if ( args.size() == 2 ) {
	//kdDebug( 80001 ) << "Two Args" << endl;
	switch (sigid) {
	    case SignatureIntInt:
		implantInt( exec, uo+1, args[0] );
		implantInt( exec, uo+2, args[1] );
		break;
	    case SignatureStringInt:
		implantQString( exec, uo+1, args[0] );
		implantInt( exec, uo+2, args[1] );
		break;
	    case SignatureRectBool:
		implantRect( exec, uo+1, args[0], &rect[0] );
		implantBool( exec, uo+2, args[1] );
		break;
	    case SignatureStringString:
		implantQString( exec, uo+1, args[0] );
		implantQString( exec, uo+2, args[1] );
		break;
	    case SignatureStringBool:
		implantQString( exec, uo+1, args[0] );
		implantBool( exec, uo+2, args[1] );
		break;
	    case SignatureIntColor:
		implantInt( exec, uo+1, args[0] );
		implantColor( exec, uo+2, args[1], &color[0] );
		break;
	    case SignatureURLURL:
		implantURL( exec, uo+1, args[0], &url[0] );
		implantURL( exec, uo+2, args[1], &url[1] );
		break;
	    case SignatureURLString:
		implantURL( exec, uo+1, args[0], &url[0] );
		implantQString( exec, uo+2, args[1] );
		break;
	    case SignatureStringURL:
		implantQString( exec, uo+1, args[0] );
		implantURL( exec, uo+2, args[1], &url[0] );
		break;

	    default:
		notsupported = true;
		break;
	}

    } else  {
	//kdDebug( 80001 ) << args.size() << " Args" << endl;
	notsupported = false;
	switch (sigid) {
	    case SignatureNone:
		break;
	    case SignatureStringStringString:
		if ( args.size() != 3 )
		    return KJS::Null();
		implantQString( exec, uo+1, args[0] );
		implantQString( exec, uo+2, args[1] );
		implantQString( exec, uo+3, args[2] );
		break;
	    case SignatureIntIntString:
		if ( args.size() != 3 )
		    return KJS::Null();
		implantInt( exec, uo+1, args[0] );
		implantInt( exec, uo+2, args[1] );
		implantQString( exec, uo+3, args[2] );
		break;
	    case SignatureIntIntIntInt:
		if ( args.size() != 4 )
		    return KJS::Null();
		implantInt( exec, uo+1, args[0] );
		implantInt( exec, uo+2, args[1] );
		implantInt( exec, uo+3, args[2] );
		implantInt( exec, uo+4, args[3] );
		break;
	    case SignatureIntIntIntIntBool:
		if ( args.size() != 5 )
		    return KJS::Null();
		implantInt( exec, uo+1, args[0] );
		implantInt( exec, uo+2, args[1] );
		implantInt( exec, uo+3, args[2] );
		implantInt( exec, uo+4, args[3] );
		implantBool( exec, uo+5, args[4] );
		break;
	    case SignatureIntIntIntIntIntInt:
		if ( args.size() != 6 )
		    return KJS::Null();
		implantInt( exec, uo+1, args[0] );
		implantInt( exec, uo+2, args[1] );
		implantInt( exec, uo+3, args[2] );
		implantInt( exec, uo+4, args[3] );
		implantInt( exec, uo+5, args[4] );
		implantInt( exec, uo+6, args[5] );
		break;
	    default:
		notsupported = true;
	}
    }

    if ( notsupported ) {
	//kdDebug( 80001 ) << "Trying method 2" << endl;
        // Rip apart the call signature to get the args slow but effective.
	notsupported = false;
	int argsStart = slotname.find('(');
	int argsEnd = slotname.find(')');
        QString fargs = slotname.mid( argsStart+1, argsEnd-argsStart-1 );

	// Iterate over the parameters
        QStringList argList = QStringList::split(',', fargs, true);
	uint count = QMIN( argList.count(), MAX_SUPPORTED_ARGS );

	kdDebug( 80001 ) << "======== arg count " << count << endl;
        for( uint idx = 0; idx < count; idx++ ) {
	    kdDebug( 80001 ) << "======== Handling arg " << idx << endl;

	    QString arg = argList[idx];
	    arg = arg.replace("const", "");
	    arg = arg.replace("&", "");
	    arg = arg.simplifyWhiteSpace();
	    kdDebug( 80001 ) << "Testing " << arg.latin1() << endl;

	    if(arg.contains("uint") == 1) {
		unsigned int *unsignedint=new uint;
		*unsignedint=args[idx].toUInt32(exec);
		uintlist.append(unsignedint);
		static_QUType_ptr.set( uo+1+idx, unsignedint) ;
	    }
	    else if( arg.contains("int") == 1 )
		implantInt( exec, uo+1+idx, args[idx] );
	    else if ( arg.contains("double") == 1 )
		implantDouble(exec,  uo+1+idx, args[idx]);
	    else if ( arg.contains("QStringList") == 1 )
		implantStringList( exec, uo+1+idx, args[idx], &slst[idx] );
	    else if ( arg.contains("QString") == 1 )
		implantQString(exec,  uo+1+idx, args[idx] );
	    else if ( arg.contains("QCString") == 1 )
		implantCString(exec,  uo+1+idx, args[idx] );
	    else if ( arg.contains("bool") == 1 )
		implantBool( exec, uo+1+idx, args[idx] );
	    else if ( arg.contains("KURL") == 1 )
		implantURL( exec, uo+1+idx, args[idx], &url[idx] );
	    else if ( arg.contains("QColor") == 1 )
	        implantColor( exec, uo+1+idx, args[idx], &color[idx] );
	    else if ( arg.contains("QPoint") == 1 )
		implantPoint( exec, uo+1+idx, args[idx], &point[idx] );
	   else if ( arg.contains("QSize") == 1 )
		implantSize( exec, uo+1+idx, args[idx], &size[idx] );
	   else if ( arg.contains("QRect") == 1 )
		implantRect( exec, uo+1+idx, args[idx], &rect[idx] );
	   else if ( arg.contains("QDate") == 1 )
		implantDate( exec, uo+1+idx, args[idx], &date[idx] );
	   else if ( arg.contains("QTime") == 1 )
		implantTime( exec, uo+1+idx, args[idx], &time[idx] );
	   else if ( arg.contains("QDateTime") == 1 )
		implantDateTime( exec, uo+1+idx, args[idx], &datetime[idx] );
	   else if ( arg.contains("QPixmap") == 1 )
		implantPixmap( exec, uo+1+idx, args[idx], &pix[idx] );
	   else if ( arg.contains("char") == 1)
	        implantCharStar( exec, uo+1+idx, args[idx] );
	    else if ( ( arg.contains("QObject") == 1 ) || ( arg.contains("QWidget") == 1 ) )
	   {
	   	kdDebug( 80001 ) << "Doing a QObject" << endl;
		notsupported = !implantObjectProxy( exec, uo+1+idx, args[idx], "QObject" );
	   }
	    else if ( QVariant::nameToType(arg.latin1()) != QVariant::Invalid ) {
		bool ok = implantValueProxy( exec, uo+1+idx, args[idx], arg );
		if ( !ok ) {
		    notsupported = true;
		    break;
		}
		else
		{
			kdDebug( 80001 ) << "Implanted the variant " << static_QUType_QVariant.get(uo+1+idx).isValid() << endl;
		}
	    }
	    /* else if ( ( arg.contains("QVariant") == 1 ) ||
	        ( arg.contains("QPen") == 1 ) || (arg.contains("QFont") == 1 )  ||
		( arg.contains("QBrush") == 1 ))
		if(!implantQVariant(exec,  uo+1+idx, args[idx] )){
		    notsupported = true;
		    break;
		} */
	    else if ( arg.contains("*") == 1 ) {
		QRegExp re("(\\w+)\\*");
		if ( re.search(arg) >= 0 ) {
		    bool ok = implantObjectProxy( exec, uo+1+idx, args[idx], re.cap(1) );
		    if ( !ok ) {
			ok = implantOpaqueProxy( exec, uo+1+idx, args[idx], re.cap(1) );
			if ( !ok ) {
			    notsupported = true;
			    break;
			}
		    }
		}
		else {
		    notsupported = true;
		    break;
		}
	    }  else {
		notsupported = true;
		break; // Abort on the first unsupported parameter
	    }
        }
    }

    if ( notsupported ) {
	kdWarning(80001) << "Signature " << slotname << " has not been implemented" << endl;
	return KJS::Null();
    } else {
        kdDebug(80001) << "Call Signature: " << slotname << endl;

	obj->qt_invoke( slotid, uo );
    }
    // Handle return types
    QCString ret( proxyimp->returnType() );
    if ( ret.isEmpty() ) {
	// Basic type (void, int etc.)
	return extractValue( exec, uo );
    }

    // Object type
    kdDebug(80001) << "Handling a pointer return of type " << ret << endl;

    if ( proxy->part()->factory()->isQObject(ret) ) {
	kdDebug(80001) << "Return type is QObject " << ret << endl;
	return extractValue( exec, uo, proxy );
    }

    // Opaque type
    kdDebug(80001) << "Return type is opaque " << ret << endl;

    JSOpaqueProxy *opaque = new JSOpaqueProxy(static_QUType_ptr.get(uo), ret.data());
    //opaque->setValue(  );

    KJS::Object opaqueObj( opaque );
    opaque->addBindings( exec, opaqueObj );

    return opaqueObj;
}

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:

