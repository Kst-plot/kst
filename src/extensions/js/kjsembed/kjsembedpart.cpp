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

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <qobject.h>
#include <qdialog.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qwidget.h>
#include <qwidgetfactory.h>

#ifndef QT_ONLY
#include <kaction.h>
#endif // QT_ONLY

#include <kjs/interpreter.h>
#include <kjs/object.h>
#include <kjs/ustring.h>
#include <kjs/types.h>
#include <kjs/value.h>

#include "global.h"
#include "jsfactory.h"
#include "jsbinding.h"
#include "jsobjectproxy.h"
#include "jsconsolewidget.h"
#include "jsbuiltin.h"

#ifndef QT_ONLY
#include "bindings/image_imp.h"
#endif // QT_ONLY

#include "kjsembedpart_imp.h"
#include "kjsembedpart.h"

#ifndef QT_ONLY

#include "kjsembedpart.moc"

namespace KJSEmbed {

class KJSEmbedActionRunner : public XMLActionRunner
{
public:
    KJSEmbedActionRunner( KJSEmbedPart *jspart ) : XMLActionRunner(), part(jspart) {}
    virtual ~KJSEmbedActionRunner() {}

    virtual bool run( XMLActionClient *client, const XMLActionClient::XMLActionScript &script ) {
	if ( script.type == "js" )
	    return part->runFile( script.src );
	else
	    return XMLActionRunner::run( client, script );
    }

private:
    KJSEmbedPart *part;
};

} // namespace KJSEmbed

#else // QT_ONLY

#include "qtstubs.h"

#endif // QT_ONLY

namespace KJSEmbed {

//
// KPart
//

KJSEmbedPart::KJSEmbedPart( QObject *parent, const char *name )
    : KParts::ReadOnlyPart( parent, name ? name : "kjsembed_part" ),
      jsConsole(0), jsfactory(0),
      widgetparent(0), widgetname(name ? name : "kjsembed_part"), deletejs(false)
{
    createInterpreter();
    createBuiltIn( js->globalExec(), js->globalObject() );
    createActionClient();
}

KJSEmbedPart::KJSEmbedPart( QWidget *wparent, const char *wname, QObject *parent, const char *name )
    : KParts::ReadOnlyPart( parent, name ? name : (wname?wname:"jsembed_part") ),
      jsConsole(0), jsfactory(0),
      widgetparent(wparent), widgetname(wname), deletejs(false)
{
    createInterpreter();
    createBuiltIn( js->globalExec(), js->globalObject() );
    createActionClient();
}

KJSEmbedPart::KJSEmbedPart( KJS::Interpreter *jsi, QWidget *wparent, const char *wname,
			    QObject *parent, const char *name )
    : KParts::ReadOnlyPart( parent, name ? name : (wname?wname:"jsembed_part") ),
      jsConsole(0), jsfactory(0), builtins(0),
      widgetparent(wparent), widgetname(wname), js(jsi), deletejs(false)
{
    if ( !js ) {
	createInterpreter();
	createBuiltIn( js->globalExec(), js->globalObject() );
    }
    createActionClient();
}

KJSEmbedPart::~KJSEmbedPart()
{
    if ( deletejs )
	delete js;
    delete jsfactory;
    delete builtins;
}

void KJSEmbedPart::createInterpreter()
{
    deletejs = true;
    js = new KJS::Interpreter();

    jsfactory = new JSFactory( this );
    jsfactory->addType( className() );
    jsfactory->addTypes( js->globalExec(), js->globalObject() );
}

void KJSEmbedPart::createBuiltIn( KJS::ExecState *exec, KJS::Object &parent )
{
    partobj = addObject( this, parent, "part" );

    builtins = new JSBuiltIn( this );
    builtins->add( exec, parent );
}

void KJSEmbedPart::createActionClient()
{
#ifndef QT_ONLY    
    xmlclient = new KJSEmbed::XMLActionClient( this );
    xmlclient->setActionCollection( actionCollection() );
    xmlclient->setRunner( new KJSEmbedActionRunner(this) );
#endif // QT_ONLY
}

JSConsoleWidget *KJSEmbedPart::view()
{
#ifndef QT_ONLY    
    if ( !jsConsole ) {
	QCString name = widgetname ? widgetname : QCString("jsembed_console");
	jsConsole = new JSConsoleWidget( this, widgetparent, name );
	setWidget( jsConsole );
	jsfactory->addType( jsConsole->className() );
    }
    return jsConsole;
#endif // QT_ONLY
return 0L;
}

QStringList KJSEmbedPart::constructorNames() const
{
    QStringList classes;

    KJS::List cons = constructorList();
    KJS::ListIterator it = cons.begin();
    while ( it != cons.end() ) {
	KJS::Value v = *it;
	classes += v.toString( js->globalExec() ).qstring();
	it++;
    }

    return classes;
}

KJS::Value KJSEmbedPart::constructors() const
{
    KJS::List items = constructorList();
    kdDebug(80001) << "KJSEmbedPart::constructors() returning " << items.size() << " items" << endl;
    return KJS::Object( js->builtinArray().construct( js->globalExec(), items ) );
}

KJS::List KJSEmbedPart::constructorList() const
{
    KJS::List items;

    KJS::Object obj = js->globalObject();
    KJS::ExecState *exec = js->globalExec();

    KJS::ReferenceList l = obj.propList( exec );
    KJS::ReferenceListIterator propIt = l.begin();
    while ( propIt != l.end() ) {

	KJS::Identifier name = propIt->getPropertyName( exec );

	if ( obj.hasProperty( exec, name ) ) {
	    KJS::Value v = obj.get( exec, name );
	    KJS::Object vobj = v.toObject( exec );

	    if ( vobj.implementsConstruct() )
		items.append( KJS::String( name.ustring() ) );
	}

	propIt++;
    }

    return items;
}

//
// Version information
//

QCString KJSEmbedPart::versionString() const
{
    return QCString(KJSEMBED_VERSION_STRING);
}

int KJSEmbedPart::versionMajor() const
{
    return KJSEMBED_VERSION_MAJOR;
}

int KJSEmbedPart::versionMinor() const
{
    return KJSEMBED_VERSION_MINOR;
}

//
// Execute a JS script.
//

KJS::Value KJSEmbedPart::evaluate( const QString &script, const KJS::Value &self )
{
    if ( execute( res, script, self ) )
	return res.value();

    return KJS::Null();
}

bool KJSEmbedPart::execute( const QString &script, const KJS::Value &self )
{
    return execute( res, script, self );
}

bool KJSEmbedPart::execute( KJS::Completion &result, const QString &script, const KJS::Value &self )
{
    KJS::Value val( self );
    result = js->evaluate( script, self.isNull() ? partobj : val );
    return (result.complType() == KJS::Normal) || (result.complType() == KJS::ReturnValue);
}

bool KJSEmbedPart::openURL( const KURL &url )
{
    if ( url.protocol() == "javascript" ) {
//	kdDebug(80001) << "KJSEmbedPart: openURL '" << url.url() << "' is javascript" << endl;

#ifndef QT_ONLY
	QString cmd = url.url();
#else
	QString cmd = url.toString();
#endif

	QString js( "javascript:" );
	cmd = cmd.replace( 0, js.length(), QString("") );

//	kdDebug(80001) << "KJSEmbedPart: JS command is '" << cmd << "'" << endl;
	return execute( cmd );
    }
    return false;
}

//
// Invoke a script file.
//

bool KJSEmbedPart::runFile( const QString &name, const KJS::Value &self )
{
    kdDebug(80001) << "KJSEmbedPart::runFile(): file is '" << name << "'" << endl;
    QString script = loadFile( name );
    return execute( script, self );
}

bool KJSEmbedPart::loadActionSet( const QString &file )
{
#ifndef QT_ONLY
    return xmlclient->load( file );
#else // QT_ONLY
    Q_UNUSED( file );
    return false;
#endif // QT_ONLY
}

QString KJSEmbedPart::loadFile( const QString &src )
{
    QString script;

    if ( src == "-" ) {
	QTextStream ts( stdin, IO_ReadOnly );
	script = ts.read();
    }
    else {
	QFile file( src );
	if ( file.open( IO_ReadOnly ) ) {
	    script = QString( file.readAll() );
	}
	else {
	    kdWarning() << "Could not open file '" << src << "', "
			<< strerror( errno ) << endl;
	    return QString::null;
	}
    }

    if ( script.startsWith( "#!" ) ) {
	int pos = script.find( "\n" );
	if ( pos > 0 )
	    script = script.mid( pos );
    }

    return script;
}

//
// Publishing objects.
//

KJS::Object KJSEmbedPart::bind( QObject *obj )
{
    KJS::Object jsobj = jsfactory->createProxy( js->globalExec(), obj );
    JSProxy::toObjectProxy(jsobj.imp() )->setOwner(JSProxy::Native);
    return jsobj;
}

KJS::Object KJSEmbedPart::addObject( QObject *obj, KJS::Object &parent, const char *name )
{
    KJS::Object jsobj = bind( obj );
    parent.put( js->globalExec(), name ? name : obj->name(), jsobj );

    return jsobj;
}

KJS::Object KJSEmbedPart::addObject( QObject *obj, const char *name )
{
    return addObject( obj, js->globalObject(), name );
}

void KJSEmbedPart::virtual_hook( int /*id*/, void * /*data*/ )
{

}

static KJS::Object scopeWalker( KJS::ExecState *exec, const KJS::Object &root, const QString &objectString )
{
  KJS::Object returnObject = root;
  QStringList objects = QStringList::split(".", objectString);
  for( uint idx = 0; idx < objects.count(); ++idx)
  {
    KJS::Identifier id = KJS::Identifier( KJS::UString( objects[idx] ));
    KJS::Value newObject = returnObject.get(exec, id );
    if( newObject.isValid() )
    	returnObject = newObject.toObject(exec);
  }
  return returnObject;
}

KJS::Value KJSEmbedPart::callMethod( const QString & methodName, const KJS::List & args ) const
{
  KJS::ExecState *exec = js->globalExec();
  KJS::Identifier id = KJS::Identifier( KJS::UString(methodName.latin1() ));
  KJS::Object obj = js->globalObject();
  KJS::Object fun = obj.get(exec, id ).toObject( exec );
  KJS::Value retValue;
  if ( !fun.implementsCall() )
  {
  	// We need to create an exception here...
  }
  else
        retValue = fun.call(exec, obj, args);
	kdDebug( 80001 ) << "Returned type is: " << retValue.type() << endl;  
	if( exec->hadException() )
	{
		kdWarning( 80001 ) << "Got error: " << exec->exception().toString(exec).qstring() << endl;
		return exec->exception();
	}
	else
	{
		if( retValue.type() == 1 && retValue.type() == 0)
		{
			kdDebug( 80001 ) << "Got void return type. " << endl;
			return KJS::Null();
		}
	}
  return retValue;
}

KJS::Value KJSEmbedPart::getValue( const QString & valueName ) const
{
  KJS::ExecState *exec = js->globalExec();
  KJS::Identifier id = KJS::Identifier( KJS::UString(valueName.latin1() ));
  KJS::Object obj = js->globalObject();
  return obj.get(exec, id );
}

void KJSEmbedPart::putValue( const QString & valueName, const KJS::Value & value )
{
  KJS::ExecState *exec = js->globalExec();
  KJS::Identifier id = KJS::Identifier( KJS::UString(valueName.latin1() ));
  KJS::Object obj = js->globalObject();
  obj.put(exec, id, value);
}

void KJSEmbedPart::putVariant( const QString & valueName, const QVariant & value )
{
	KJS::Value val = convertToValue( js->globalExec(), value);
	putValue( valueName, val );
}

QVariant KJSEmbedPart::getVariant( const QString & valueName ) const
{
	return convertToVariant( js->globalExec(), getValue( valueName ) );
} 

bool KJSEmbedPart::hasMethod( const QString & methodName )
{
  KJS::ExecState *exec = js->globalExec();
  KJS::Identifier id = KJS::Identifier( KJS::UString(methodName.latin1() ));
  KJS::Object obj = js->globalObject();
  KJS::Object fun = obj.get(exec, id ).toObject( exec );
  return fun.implementsCall();
}

}// namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:
