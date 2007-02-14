// $Id$
// -*- c++ -*-

/*
 *  Copyright (C) 2003, Ian Reinhart Geiser <geiseri@kde.org>
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
#include <kapplication.h>
#include <dcopref.h>
#include <dcopclient.h>
#include <kdebug.h>
#include <qregexp.h>

#include "dcop_imp.h"
#include "dcop_imp.moc"

#include <kdatastream.h>
#include <kurl.h>
#include <qvariant.h>
#include <qstring.h>
#include <qmap.h>
#include <qfont.h>
#include <qpixmap.h>
#include <qbrush.h>
#include <qrect.h>
#include <qsize.h>
#include <qcolor.h>
#include <qimage.h>
#include <qpoint.h>
#include <qvaluelist.h>
#include <qiconset.h>
#include <qpointarray.h>
#include <qbitmap.h>
#include <qcursor.h>
#include <qdatetime.h>
#include <qmemarray.h>
#include <qkeysequence.h>
#include <qbitarray.h>

#include <kjs/interpreter.h>
#include <kjs/identifier.h>
#include <kjs/types.h>
#include <kjs/ustring.h>
#include <kjs/value.h>

#include "../jsbinding.h"
#include <kjsembed/jsopaqueproxy.h>
#include <kjsembed/jsobjectproxy.h>
#include <kjsembed/kjsembedpart.h>
namespace KJSEmbed {
namespace Bindings {
KJS::Object JSDCOPRefLoader::createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const
{
    QString application = extractQString(exec, args, 0);
    QString object = extractQString(exec, args, 1);
    JSOpaqueProxy * prx;
    if( application.isEmpty() )
        prx= new JSOpaqueProxy( new DCOPRef(), "DCOPRef" );
    else
        prx= new JSOpaqueProxy( new DCOPRef(application.latin1(),object.latin1()), "DCOPRef" );
    
    KJS::Object proxyObj( prx );
    prx->setOwner( JSProxy::JavaScript );
    addBindings(jspart,exec,proxyObj);
    return proxyObj;
}

void JSDCOPRefLoader::addBindings( KJSEmbedPart *jspart, KJS::ExecState *exec, KJS::Object &proxy ) const
{
    JSDCOPRef::addBindings( exec, proxy );
}

JSDCOPRef::JSDCOPRef(KJS::ExecState *exec, int id)
    : JSProxyImp(exec), mid(id)
{
}

JSDCOPRef::~JSDCOPRef()
{
}

void JSDCOPRef::addBindings( KJS::ExecState *exec, KJS::Object &object )
{
    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( object.imp() );
    if ( !op ) {
        kdWarning() << "JSDCOPRef::addBindings() failed, not a JSOpaqueProxy" << endl;
        return;
    }

    if ( op->typeName() != "DCOPRef" ) {
        kdWarning() << "JSDCOPRef::addBindings() failed, type is " << op->typeName() << endl;
        return;
    }

    DCOPRef *ref = op->toNative<DCOPRef>();

    JSProxy::MethodTable methods[] = {
	{ Methodcall, "call"},
	{ Methodsend, "send" },
	{ Methodapp, "app" },
	{ Methodobj, "obj" },
	{ Methodtype, "type" },
	{ MethodsetRef, "setRef" },
	{ 0, 0 }
	};

    int idx = 0;
    do {
        JSDCOPRef *meth = new JSDCOPRef( exec, methods[idx].id );
        object.put( exec, methods[idx].name, KJS::Object(meth) );
        ++idx;
    } while( methods[idx].id );

}

KJS::Value JSDCOPRef::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {


    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );
    if ( !op ) {
        kdWarning() << "JSDCOPRef::call() failed, not a JSOpaqueProxy" << endl;
        return KJS::Boolean(false);
    }

    if ( op->typeName() != "DCOPRef" ) {
        kdWarning() << "JSDCOPRef::call() failed, type is " << op->typeName() << endl;
        return KJS::Boolean(false);
    }
    DCOPRef *ref = op->toNative<DCOPRef>();
    kdDebug() << "Ref valid" << ref << endl;
    if( !ref )
    {
    	kdWarning() << "Ref invalid" << ref << endl;
	return KJS::Null();
    }
    kdDebug() << "DCOPRef call. " << mid << endl;
    KJS::Value retValue = KJS::Value();
    switch ( mid ) {
	case Methodcall:
	{
            if ( args.size() < 1 )
            {
		kdWarning() << "Not enough args..."  << endl;
	        return KJS::Boolean(false);
	    }
            QByteArray data, replyData;
	    QDataStream ds( replyData, IO_ReadOnly);
            QCString type;

            QString app = ref->app();
            QString interface = ref->obj();
            QString function = extractQString(exec, args, 0);
            QStringList argTypes = JSDCOPClient::getTypes( function );

            if( args.size() > 1)
                   for( int idx = 1; idx < args.size(); ++idx)
                   {
                        JSDCOPClient::marshall(convertToVariant(exec,args[idx] ), argTypes[idx - 1], data);
                   }
	    if( !kapp->dcopClient()->call(app.local8Bit(), interface.local8Bit(), function.local8Bit(), data, type, replyData))
                retValue =  KJS::Boolean(false);
            else
                retValue = JSDCOPClient::demarshall(exec, type, ds);
            break;
	}
	case Methodsend:
	{
             if ( args.size() < 1 )
                  return KJS::Boolean(false);

             QByteArray data;
             QString app = ref->app();
             QString interface = ref->obj();
             QString function = extractQString(exec, args, 0);

             QStringList argTypes = JSDCOPClient::getTypes( function );

             if( args.size() > 1)
                for( int idx = 1; idx < args.size(); ++idx)
                {
                    JSDCOPClient::marshall(convertToVariant(exec,args[idx] ), argTypes[idx - 1], data);
                }
             retValue = KJS::Boolean( kapp->dcopClient()->send(app.local8Bit(), interface.local8Bit(),
                        function.local8Bit(), data));
            break;
	}
	case Methodapp:
	{
	    retValue = convertToValue( exec, ref->app() );
            break;
	}
	case Methodobj:
	{
	    retValue = convertToValue( exec, ref->obj() );
            break;
	}
	case Methodtype:
	{
	    retValue = convertToValue( exec, ref->type() );
            break;
	}
	case MethodsetRef:
	{
      QString arg0 = extractQString(exec, args, 0);
      QString arg1 = extractQString(exec, args, 1);
      QString arg2 = extractQString(exec, args, 0);
	    ref->setRef(arg0.latin1(), arg1.latin1(), arg2.latin1() );
            break;
	}
        default:
	{
            kdWarning() << "JSDCOPClient has no method " << mid << endl;
            break;
	}
    }
    op->setValue( ref, "DCOPRef" );
    return retValue;
}

KJS::Object JSDCOPClientLoader::createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &/*args*/) const
{
    JSOpaqueProxy * prx = new JSOpaqueProxy( kapp->dcopClient(), "DCOPClient" );
    prx->setOwner( JSProxy::Native );
    KJS::Object proxyObj( prx );
    addBindings( jspart, exec, proxyObj );
    return proxyObj;
}

void JSDCOPClientLoader::addBindings( KJSEmbedPart *jspart, KJS::ExecState *exec, KJS::Object &proxy ) const
{
    JSDCOPClient::addBindings( exec, proxy );
}

JSDCOPClient::JSDCOPClient( KJS::ExecState *exec, int id  )
    : JSProxyImp(exec), mid(id)
{
}

JSDCOPClient::~JSDCOPClient()
{
}


void JSDCOPClient::addBindings( KJS::ExecState *exec, KJS::Object &object )
{
    if( !JSProxy::checkType(object, JSProxy::OpaqueProxy, "DCOPClient") ) return;

    JSProxy::MethodTable methods[] = {
    { Methodattach, "attach"},
    { Methoddetach, "detach"},
    { MethodisAttached, "isAttached" },
    { MethodregisteredApplications, "registeredApplications" },
    { MethodremoteObjects, "remoteObjects" },
    { MethodremoteInterfaces, "remoteInterfaces" },
    { MethodremoteFunctions, "remoteFunctions" },
    { MethodconnectDCOPSignal, "connectDCOPSignal" },
    { MethoddisconnectDCOPSignal, "disconnectDCOPSignal"},
    { Methodcall, "call"},
    { Methodsend, "send"},
    { MethodDCOPStart, "dcopStart"},
    { MethodappId, "appId"},
    { MethodisApplicationRegistered, "isApplicationRegistered" },
    { 0, 0 }
    };

    JSProxy::addMethods<JSDCOPClient>(exec, methods, object );
}

KJS::Value JSDCOPClient::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {
    if( !JSProxy::checkType(self, JSProxy::OpaqueProxy, "DCOPClient") ) return KJS::Value();
    
    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );

    KJS::Value retValue = KJS::Value();
    switch ( mid ) {
        case Methodattach: {
	    retValue = KJS::Boolean(attach( ));
	    break;
	}
	case Methoddetach: {
	    retValue = KJS::Boolean(detach( ));
	    break;
	}
	case MethodisAttached: {
	    retValue = KJS::Boolean(isAttached( ));
	    break;
	}
	case MethodregisteredApplications: {
	    retValue = convertToValue(exec, registeredApplications());
	    break;
	}
	case MethodremoteObjects: {
	    QString arg0 = extractQString(exec, args, 0);
	    retValue = convertToValue( exec, remoteObjects(arg0));
	    break;
	}
	case MethodremoteInterfaces: {
	    QString arg0 = extractQString(exec, args, 0);
	    QString arg1 = extractQString(exec, args, 1);
	    this->remoteInterfaces(arg0, arg1);
	    break;
	}
	case MethodremoteFunctions: {
	    QString arg0 = extractQString(exec, args, 0);
	    QString arg1 = extractQString(exec, args, 1);
	    remoteFunctions(arg0, arg1);
	    break;
	}
	case MethodconnectDCOPSignal: {
	    QString arg0 = extractQString(exec, args, 0);
	    QString arg1 = extractQString(exec, args, 1);
	    QString arg2 = extractQString(exec, args, 2);
	    QString arg3 = extractQString(exec, args, 3);
	    QString arg4 = extractQString(exec, args, 4);
	    bool arg5 = (args.size() > 5) ? args[5].toBoolean(exec) : false;
	    connectDCOPSignal( arg0, arg1, arg2, arg3, arg4, arg5);
	    break;
	}
	case MethoddisconnectDCOPSignal: {
	    QString arg0 = extractQString(exec, args, 0);
	    QString arg1 = extractQString(exec, args, 1);
	    QString arg2 = extractQString(exec, args, 2);
	    QString arg3 = extractQString(exec, args, 3);
	    QString arg4 = extractQString(exec, args, 4);
	    disconnectDCOPSignal(arg0, arg1, arg2, arg3, arg4);
	    break;
	}
	case MethodDCOPStart: {
		QString arg0 = extractQString(exec, args, 0);

		retValue = KJS::String( dcopStart( arg0, QStringList() ) );
		break;
	}
	case MethodappId:
		retValue = KJS::String( kapp->dcopClient()->appId().data() );
		break;
	case MethodisApplicationRegistered: {
		QString arg0 = extractQString(exec, args, 0);
		retValue = KJS::Boolean( kapp->dcopClient()->isApplicationRegistered( arg0.latin1() )  );
		break;
	}
	case Methodsend:
		return dcopSend( exec, self, args );
		break;
	case Methodcall:
		return dcopCall( exec, self, args );
		break;
        default:
        kdWarning() << "JSDCOPClient has no method " << mid << endl;
        break;
    }
    return retValue;
}

bool JSDCOPClient::attach() const
{
	if( !kapp->dcopClient()->isAttached() )
		return kapp->dcopClient()->attach();
	return true;
}
bool JSDCOPClient::detach() const
{	return kapp->dcopClient()->detach();

}

bool JSDCOPClient::isAttached() const
{
	return kapp->dcopClient()->isAttached();
}

KJS::Value JSDCOPClient::dcopCall( KJS::ExecState * exec, KJS::Object &, const KJS::List & args )
{
	if ( args.size() < 3 )
		return KJS::Boolean(false);

	QStringList types;
	QByteArray data, replyData;
	QDataStream ds( replyData, IO_ReadOnly);
	QCString type;

	QString app = extractQString(exec, args, 0);
	QString interface = extractQString(exec, args, 1);
	QString function = extractQString(exec, args, 2);
	QStringList argTypes = getTypes( function );

	if( args.size() > 3)
		for( int idx = 3; idx < args.size(); ++idx)
		{
			QVariant var = convertToVariant(exec,args[idx] );
			marshall(var, argTypes[idx -3 ], data);
		}
	if( !kapp->dcopClient()->call(app.local8Bit(), interface.local8Bit(), function.local8Bit(), data, type, replyData))
		return KJS::Boolean(false);
	else
		return demarshall(exec, type, ds);
}

KJS::Value JSDCOPClient::dcopSend( KJS::ExecState * exec, KJS::Object &, const KJS::List & args )
{
	if ( args.size() < 3 )
		return KJS::Boolean(false);

	QByteArray data;
	QString app = extractQString(exec, args, 0);
	QString interface = extractQString(exec, args, 1);
	QString function = extractQString(exec, args, 2);
	QStringList argTypes = getTypes( function );

	if( args.size() > 3)
		for( int idx = 3; idx < args.size(); ++idx)
		{
			QVariant var = convertToVariant(exec,args[idx] );
			marshall(var, argTypes[idx - 3], data);
		}
	return KJS::Boolean( kapp->dcopClient()->send(app.local8Bit(), interface.local8Bit(),
                        function.local8Bit(), data));
}

KJS::Value JSDCOPClient::demarshall( KJS::ExecState * exec, const QCString& type, QDataStream& data)
{
	kdDebug() << "Demarshall " << type << endl;

	if( type == "DCOPRef" )
	{
		DCOPRef *ref = new DCOPRef();
                data >> *ref;
                JSOpaqueProxy *prx = new JSOpaqueProxy(ref, "DCOPRef" );

                KJS::Object proxyObj(prx);
		kdDebug() << "DCOPRef  " << ref->app() << endl;
		Bindings::JSDCOPRef::addBindings( exec, proxyObj );
                return proxyObj;
	}
	else
		return convertToValue(exec, demarshall(type, data));
}

QStringList JSDCOPClient::getTypes( const QString& function )
{
	QCString myFunction = DCOPClient::normalizeFunctionSignature (function.latin1());
	QString args = myFunction.mid( myFunction.find('(') + 1 );
	args = args.left( args.length() - 1);
	return QStringList::split(',', args);
}

void JSDCOPClient::marshall( const QVariant& data, const QString& typeName, QByteArray& buffer )
{
	QDataStream stream(buffer, IO_WriteOnly | IO_Append);

	if ( typeName == "QString" )
	{
		stream << data.toString();
	}
	else if ( typeName == "QCString")
	{
		stream << data.toCString();
	}
	else if ( typeName == "int")
	{
		stream << data.toInt();
	}
	else if ( typeName == "uint")
	{
		stream << data.toUInt();
	}
	else if ( typeName == "bool")
	{
		stream << data.toBool();
	}
	else if ( typeName == "double")
	{
		stream << data.toDouble();
	}
	else if ( typeName == "QMap" )
	{
		stream << data.toMap();
	}
	else if ( typeName == "QValueList")
	{
		stream << data.toList();
	}
	else if ( typeName == "QFont")
	{
		stream << data.toFont();
	}
	else if ( typeName == "QPixmap")
	{
		stream << data.toPixmap();
	}
	else if ( typeName == "QBrush")
	{
		stream << data.toBrush();
	}
	else if ( typeName == "QRect")
	{
		stream << data.toRect();
	}
	else if ( typeName == "QSize")
	{
		stream << data.toSize();
	}
	else if ( typeName == "QColor")
	{
		stream << data.toColor();
	}
	else if ( typeName == "QPalette")
	{
		stream << data.toPalette();
	}
	else if ( typeName == "QColorGroup")
	{
		stream << data.toColorGroup();
	}
	/*else if ( typeName == "QIconSet")
	{
		stream << data.toIconSet();
	}*/
	else if ( typeName == "QPoint")
	{
		stream << data.toPoint();
	}
	else if ( typeName == "QImage")
	{
		stream << data.toImage();
	}
	else if ( typeName == "QPointArray")
	{
		stream << data.toPointArray();
	}
	else if ( typeName == "QRegion")
	{
		stream << data.toRegion();
	}
	else if ( typeName == "QBitmap")
	{
		stream << data.toBitmap();
	}
	else if ( typeName == "QCursor")
	{
		stream << data.toCursor();
	}
	/*else if ( typeName == "QSizePolicy")
	{
		stream << data.toSizePolicy();
	}*/
	else if ( typeName == "QDate")
	{
		stream << data.toDate();
	}
	else if ( typeName == "QTime")
	{
		stream << data.toTime();
	}
	else if ( typeName == "QDateTime")
	{
		stream << data.toDateTime();
	}
	else if ( typeName == "QByteArray")
	{
		stream << data.toByteArray();
	}
	else if ( typeName == "QBitArray")
	{
		stream << data.toBitArray();
	}
	else if ( typeName == "QKeySequence")
	{
		stream << data.toKeySequence();
	}
	else if ( typeName == "KURL" )
	{
		KURL url = data.toString();
		stream << url;
	}
	else
		stream << 0;

}

QString JSDCOPClient::dcopStart( const QString &appName, const QStringList& args )
{
	QString error;
	QString startFunction;
	if (appName.endsWith(".desktop"))
		startFunction = "start_service_by_desktop_path(QString,QStringList)";
	else
		startFunction = "start_service_by_desktop_name(QString,QStringList)";

	QByteArray data, replyData;
	QCString replyType;
	QDataStream arg(data, IO_WriteOnly);
	arg << appName << args;

	if ( !kapp->dcopClient()->call( "klauncher", "klauncher", startFunction.latin1(),  data, replyType, replyData) )
	{
		kdWarning() << "Error: Dcop call failed" << endl;
	}
	else
	{
		QDataStream reply(replyData, IO_ReadOnly);

		if ( replyType != "serviceResult" )
		{
			kdWarning() << "Error: No serviceResult " << endl;
		}
		else
		{
			int result;
			QCString dcopName;
			QString error;
			reply >> result >> dcopName >> error;
			if (result != 0)
			{
				kdWarning() << "Error: " <<  error.local8Bit().data() << endl;
			}
			else if (!dcopName.isEmpty())
			{
				return dcopName;
			}
			else
				kdWarning() << "Error: no app name returned." << endl;
		}
	}

	return "";

}

QStringList JSDCOPClient::remoteFunctions( const QString & remApp, const QString & remObj )
{
	QStringList returnList;
	QCStringList lst = kapp->dcopClient()->remoteFunctions(remApp.local8Bit(), remObj.local8Bit());
	for(uint idx = 0; idx < lst.count(); ++idx)
		returnList += lst[idx];
	return returnList;
}

QStringList JSDCOPClient::remoteInterfaces( const QString & remApp, const QString & remObj )
{
	QStringList returnList;
	QCStringList lst = kapp->dcopClient()->remoteInterfaces(remApp.local8Bit(), remObj.local8Bit());
	for(uint idx = 0; idx < lst.count(); ++idx)
		returnList += lst[idx];
	return returnList;
}

QStringList JSDCOPClient::remoteObjects( const QString & remApp )
{
	QStringList returnList;
	QCStringList lst = kapp->dcopClient()->remoteObjects(remApp.local8Bit());
	for(uint idx = 0; idx < lst.count(); ++idx)
		returnList += lst[idx];
	return returnList;
}

QStringList JSDCOPClient::registeredApplications( )
{
	QStringList returnList;
	QCStringList lst = kapp->dcopClient()->registeredApplications( );
	for(uint idx = 0; idx < lst.count(); ++idx)
		returnList += lst[idx];
	return returnList;
}

bool JSDCOPClient::connectDCOPSignal (const QString &sender, const QString &senderObj, const QString &signal, const QString &receiverObj, const QString &slot, bool Volatile)
{
	return kapp->dcopClient()->connectDCOPSignal(sender.latin1(), senderObj.latin1(), signal.latin1(), receiverObj.latin1(), slot.latin1(), Volatile);
}

bool JSDCOPClient::disconnectDCOPSignal (const QString &sender, const QString &senderObj, const QString &signal, const QString &receiverObj, const QString &slot)
{
	return kapp->dcopClient()->disconnectDCOPSignal(sender.latin1(), senderObj.latin1(), signal.latin1(), receiverObj.latin1(), slot.latin1());
}

KJS::Object JSDCOPInterfacerLoader::createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const
{
    QObject *parent = extractQObject(exec, args, 0);
    QString name = extractQString(exec, args, 1);
    QObject *obj = new JSDCOPInterface(jspart->interpreter(),parent, name.latin1() );
    JSObjectProxy *prxy = new JSObjectProxy(jspart, obj);
    KJS::Object object(prxy);
    addBindings(jspart,exec,object);
    return object;
}

void JSDCOPInterfacerLoader::addBindings( KJSEmbedPart *jspart, KJS::ExecState *exec, KJS::Object &proxy ) const
{
    JSObjectProxy *prx = JSProxy::toObjectProxy( proxy.imp() );
    if( prx )
        prx->addBindings( exec, proxy );
}

void JSDCOPInterface::publish( const QString & signature )
{
	if( m_Members.find(signature.latin1() ) == m_Members.end())
		m_Members.append(signature.latin1() );
}

JSDCOPInterface::~ JSDCOPInterface( )
{
}

JSDCOPInterface::JSDCOPInterface( KJS::Interpreter *js, QObject * parent, const char * name )
	: BindingObject( parent, name ),DCOPObject(name), m_js(js)
{
	m_Members.clear();
}

QCStringList JSDCOPInterface::functionsDynamic( )
{
	return m_Members;
}

bool JSDCOPInterface::processDynamic( const QCString & rawFunction, const QByteArray & data, QCString & replyType, QByteArray & replyData )
{
	bool isOkay = false;

	QRegExp reg = QRegExp("([_\\d\\w]+)(\\()(.*)(\\))");
	reg.search(rawFunction);

	kdDebug() << "Raw function: " << rawFunction << endl;
	kdDebug() << "Reply type: " << replyType << endl;
	kdDebug() << "Signature: " << reg.cap(1) << endl;
	kdDebug() << "Args: " << reg.cap(3) << endl;

	QString signature = reg.cap(1);

	QStringList argStrings = QStringList::split(',', reg.cap(3), false);
	kdDebug() << argStrings.count() << " args..." << endl;

	KJS::ExecState *exec = m_js->globalExec();
	KJS::Object obj = m_js->globalObject();
	KJS::Identifier id( KJS::UString( signature.latin1() ) );
	KJS::Object fun = obj.get(exec, id ).toObject( exec );
	KJS::Value retValue;


	if ( fun.implementsCall() )
	{
		QDataStream ds( data, IO_ReadOnly );
		KJS::List args;
		for( uint idx = 0; idx < argStrings.count(); ++idx)
		{
			kdDebug() << "Get arg: " << argStrings[idx] << endl;
			args.append( JSDCOPClient::demarshall( exec, argStrings[idx].latin1(), ds));
		}

		retValue = fun.call(exec, obj, args);

		if( exec->hadException() )
		{
			kdWarning() << "Got error: " << exec->exception().toString(exec).qstring() << endl;
		}
		else
		{
			kdDebug() << "Returned type is a " << retValue.type() << endl;
      if( retValue.isA(KJS::UndefinedType) )
			{
				replyType = "void";
				isOkay = true;
			}
      else if( retValue.isA(KJS::ObjectType) )
			{
          JSOpaqueProxy *oprx = JSProxy::toOpaqueProxy( retValue.imp() );
          if( oprx )
          {
            replyType = "DCOPRef";
            QDataStream refStream( replyData, IO_WriteOnly );
            if( oprx->typeName() == "DCOPRef" )
            {
                DCOPRef ref(*oprx->toNative<DCOPRef>());
            refStream << ref;
            isOkay = true;
            }
          }
			}
			else
			{
				QVariant returnVariant = convertToVariant(exec, retValue);
				JSDCOPClient::marshall(returnVariant,returnVariant.typeName(), replyData);
				replyType = returnVariant.typeName();
				isOkay = true;
			}
		}
	}
	return isOkay;
}

QVariant JSDCOPClient::demarshall( const QCString & type, QDataStream & reply )
{
	kdDebug() << "Type: '" << type << "'"<< endl;
	/*if ( type == "QMap" )  // ? dont handle?
	{
		QMap<QVariant, QVariant> result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QValueList") // ? dont handle?
	{
		QValueList<QVariant> result;
		reply >> result;
		return QVariant(result);
	}else if ( type == "QVariantList")
	{
		QVariantList result;
		reply >> result;
		return QVariant(result);
	}
	else */
	if ( type == "QString")
	{
		QString result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "int")
	{
		int result;
		reply >> result;
		kdDebug() << "Result: " << result << endl;
		return QVariant(result);
	}
	else if ( type == "uint")
	{
		uint result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "bool")
	{
		bool result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "double")
	{
		double result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QCString")
	{
		QCString result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QVariant")
	{
		QVariant result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QFont")
	{
		QFont result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QPixmap")
	{
		QPixmap result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QBrush")
	{
		QBrush result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QRect")
	{
		QRect result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QSize")
	{
		QSize result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QColor")
	{
		QColor result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QPalette")
	{
		QPalette result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QColorGroup")
	{
		QColorGroup result;
		reply >> result;
		return QVariant(result);
	}
	/*else if ( type == "QIconSet")
	{
		QIconSet result;
		reply >> result;
		return QVariant(result);
	}*/
	else if ( type == "QPoint")
	{
		QPoint result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QImage")
	{
		QImage result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QPointArray")
	{
		QPointArray result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QRegion")
	{
		QRegion result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QBitmap")
	{
		QBitmap result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QCursor")
	{
		QCursor result;
		reply >> result;
		return QVariant(result);
	}
	/*else if ( type == "QSizePolicy")
	{
		QSizePolicy result;
		reply >> result;
		return QVariant(result);
	}*/
	else if ( type == "QDate")
	{
		QDate result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QTime")
	{
		QTime result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QDateTime")
	{
		QDateTime result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QByteArray")
	{
		QByteArray result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QBitArray")
	{
		QBitArray result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QKeySequence")
	{
		QKeySequence result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QStringList")
	{
		QStringList result;
		reply >> result;
		return QVariant(result);
	}
	else if ( type == "QCStringList")
	{
		QStringList result;
		reply >> result;
		return QVariant(result);
	}
	else
		return QVariant();

}
} // namespace Bindings
} // namespace KJSEmbed




