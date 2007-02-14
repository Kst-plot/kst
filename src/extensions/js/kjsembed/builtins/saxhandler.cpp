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
#include "kjsembed/jsbinding.h"
#include "saxhandler.h"

namespace KJSEmbed {
namespace BuiltIns {

SaxHandler::SaxHandler( KJS::ExecState *execstate )
    : QXmlDefaultHandler(), exec(execstate), error( ErrorNone )
{
}

SaxHandler::~SaxHandler()
{
}

void SaxHandler::setHandler( const KJS::Object &handler )
{
    jshandler = handler;
}

bool SaxHandler::startDocument()
{
    if ( !jshandler.isValid() ) {
	error = ErrorNoHandler;
	return false;
    }

    KJS::Identifier funName("startDocument");
    if ( !jshandler.hasProperty(exec, funName) )
	return QXmlDefaultHandler::startDocument();

    KJS::Object fun = jshandler.get(exec, funName).toObject( exec );
    if ( !fun.implementsCall() ) {
	error = ErrorNotCallable;
	return false;
    }

    KJS::Value ret = fun.call( exec, jshandler, KJS::List() );
    return ret.toBoolean( exec );
}

bool SaxHandler::endDocument()
{
    if ( !jshandler.isValid() ) {
	error = ErrorNoHandler;
	return false;
    }

    KJS::Identifier funName("endDocument");
    if ( !jshandler.hasProperty(exec, funName) )
	return QXmlDefaultHandler::endDocument();

    KJS::Object fun = jshandler.get(exec, funName).toObject( exec );
    if ( !fun.implementsCall() ) {
	error = ErrorNotCallable;
	return false;
    }

    KJS::Value ret = fun.call( exec, jshandler, KJS::List() );
    return ret.toBoolean( exec );
}

bool SaxHandler::startElement( const QString &ns, const QString &ln, const QString &qn,
			       const QXmlAttributes &attrs )
{
    if ( !jshandler.isValid() ) {
	error = ErrorNoHandler;
	return false;
    }

    KJS::Identifier funName("startElement");
    if ( !jshandler.hasProperty(exec, funName) )
	return QXmlDefaultHandler::startElement( ns, ln, qn, attrs );

    KJS::Object fun = jshandler.get(exec, funName).toObject( exec );
    if ( !fun.implementsCall() ) {
	error = ErrorNotCallable;
	return false;
    }

    KJS::List args;
    args.append( KJS::String(ns) );
    args.append( KJS::String(ln) );
    args.append( KJS::String(qn) );
    // TODO: XmlAttributes not yet supported

    KJS::Value ret = fun.call( exec, jshandler, args );
    return ret.toBoolean( exec );
}

bool SaxHandler::endElement( const QString &ns, const QString &ln, const QString &qn )
{
    if ( !jshandler.isValid() ) {
	error = ErrorNoHandler;
	return false;
    }

    KJS::Identifier funName("endElement");
    if ( !jshandler.hasProperty(exec, funName) )
	return QXmlDefaultHandler::endElement( ns, ln, qn );

    KJS::Object fun = jshandler.get(exec, funName).toObject( exec );
    if ( !fun.implementsCall() ) {
	error = ErrorNotCallable;
	return false;
    }

    KJS::List args;
    args.append( KJS::String(ns) );
    args.append( KJS::String(ln) );
    args.append( KJS::String(qn) );

    KJS::Value ret = fun.call( exec, jshandler, args );
    return ret.toBoolean( exec );
}

bool SaxHandler::characters( const QString &chars )
{
    if ( !jshandler.isValid() ) {
	error = ErrorNoHandler;
	return false;
    }

    KJS::Identifier funName("characters");
    if ( !jshandler.hasProperty(exec, funName) )
	return QXmlDefaultHandler::characters( chars );

    KJS::Object fun = jshandler.get(exec, funName).toObject( exec );
    if ( !fun.implementsCall() ) {
	error = ErrorNotCallable;
	return false;
    }

    KJS::List args;
    args.append( KJS::String(chars) );

    KJS::Value ret = fun.call( exec, jshandler, args );
    return ret.toBoolean( exec );
}

QString SaxHandler::errorString()
{
    switch( error ) {

	case ErrorNoHandler:
	    return QString("No handler specified");
	    break;
	case ErrorNotCallable:
	    return QString("One of the callbacks of the handler is not callable");
	    break;
	case ErrorNone:
	    // This only means that no error occured in the JS dispatch, there
	    // could still have been an error from the parser so we fall
	    // though to call the baseclass.
	    break;
	default:
	    break;
    }

    return QXmlDefaultHandler::errorString();
}

} // namespace KJSEmbed::BuiltIns
} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:
