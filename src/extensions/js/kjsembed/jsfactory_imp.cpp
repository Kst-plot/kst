/*
 *  Copyright (C) 2001-2004, Richard J. Moore <rich@kde.org>
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

#include <qwidget.h>
#include <qwidgetfactory.h>
#include <qobjectlist.h>
#include <qtoolbox.h>
#include <qtabwidget.h>
#include <qwizard.h>

#include "global.h"
#include "jsproxy.h"
#include "jsobjectproxy.h"
#include "kjsembedpart.h"

#include "jsfactory.h"
#include "jsfactory_imp.h"

namespace KJSEmbed {
namespace Bindings {

JSFactoryImp::JSFactoryImp( KJS::ExecState *exec, JSFactory *jsfact, int mid, const QString &p )
    : JSProxyImp(exec), fact(jsfact), id(mid), param(p), defaultVal()
{
    setName( KJS::Identifier(KJS::UString(param)) );
}

JSFactoryImp::~JSFactoryImp()
{
}

KJS::Object JSFactoryImp::construct( KJS::ExecState *exec, const KJS::List &args )
{
    if ( id != NewInstance ) {
	kdDebug(80001) << "JSFactoryImp has no such constructor, id " << id << endl;

	QString msg = i18n( "JSFactoryImp has no constructor with id '%1'." ).arg( id );
  return throwError(exec, msg,KJS::ReferenceError);
    }

    return fact->create( exec, param, args );
}

void JSFactoryImp::setDefaultValue( const KJS::Value &value )
{
    defaultVal = value;
}

KJS::Value JSFactoryImp::defaultValue( KJS::ExecState *exec, KJS::Type hint ) const
{
    if ( defaultVal.isValid() )
	return defaultVal;
    return JSProxyImp::defaultValue( exec, hint );
}

KJS::Value JSFactoryImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args )
{
    QString arg0 = (args.size() > 0) ? args[0].toString(exec).qstring() : QString::null;
    QString arg1 = (args.size() > 1) ? args[1].toString(exec).qstring() : QString::null;

    kdDebug( 80001 ) << "JSFactoryImp::call() " << id << endl;
    KJS::Object err;

    switch(id){
    case MethodLoadUI:
    {
	JSObjectProxy *cproxy = JSProxy::toObjectProxy( args[1].imp() );
	QObject *cqo = cproxy ? cproxy->object() : 0;

	JSObjectProxy *proxy = JSProxy::toObjectProxy( args[2].imp() );
	QWidget *qw = proxy ? proxy->widget() : 0;
	QWidget *obj = fact->loadUI( arg0, cqo, qw, arg1.latin1() );
	if ( obj )
	{
		KJS::Object parentObject = fact->createProxy( exec, obj );
	// Add properties for child widgets.
		const QObjectList *lst = obj->children();
		if ( lst )
		{
			QObjectListIt it( *lst );
			QObject *child;
			while ( (child = it.current()) != 0 )
			{
				publishChildren( child, parentObject);
				++it;
			}
		}
		return parentObject;
	}

	QString msg = i18n( "Unable to create ui from file '%1'." ).arg(arg0);
  err = throwError(exec, msg,KJS::ReferenceError);
	break;
    }
    case MethodCreateROPart:
    {

	kdDebug(80001) << "MethodCreateROPart called, args.size is " << args.size() << endl;
	QObject *qo = 0;
	QObject *ropart = 0;

	if ( args.size() == 3 ) {
	    JSObjectProxy *proxy = JSProxy::toObjectProxy( args[1].imp() );
	    qo = proxy ? proxy->object() : 0;
	    ropart = fact->createROPart( arg0, qo, args[2].toString(exec).ascii() );
	}
	else if ( args.size() == 4 ) {
	    JSObjectProxy *proxy = JSProxy::toObjectProxy( args[2].imp() );
	    qo = proxy ? proxy->object() : 0;
	    ropart = fact->createROPart( arg0, arg1, qo, args[3].toString(exec).ascii() );
	}
	else if ( args.size() == 5 ) {
	    JSObjectProxy *proxy = JSProxy::toObjectProxy( args[2].imp() );
	    qo = proxy ? proxy->object() : 0;
	    QStringList slist = extractQStringList( exec, args, 4 );
	    ropart = fact->createROPart( arg0, arg1, qo, args[3].toString(exec).ascii(), slist );
	}

	if ( ropart ) {
	    kdDebug(80001) << "should be returning kpart type " << ropart->className() << endl;
	}
	else {
	    kdDebug(80001) << "could not create the kpart" << endl;
	}
	if ( ropart )
	    return fact->createProxy( exec, ropart );

	QString msg = i18n( "Unable to create read-only part for service '%1'." ).arg(arg0);
  err = throwError(exec, msg,KJS::ReferenceError);
	break;
    }
    case MethodConstructors:
    {
	kdDebug(80001) << "constructors called" << endl;
	return fact->part()->constructors();
	break;
    }
    case MethodWidgets:
    {

	KJS::List l;
	QStringList widgets = QWidgetFactory::widgets();
	for ( QStringList::Iterator it = widgets.begin(); it != widgets.end(); ++it ) {
	    l.append( KJS::String( *it ) );
	}

	return KJS::Object( exec->interpreter()->builtinArray().construct( exec, l ) );
	break;
    }
    case MethodIsSupported:
    {
	return KJS::Boolean( fact->isSupported( arg0 ) );
	break;
    }
    case MethodIsQObject:
    {
	return KJS::Boolean( fact->isQObject( arg0 ) );
	break;
    }
    case MethodIsOpaque:
    {
	return KJS::Boolean( fact->isOpaque( arg0 ) );
	break;
    }
    case MethodIsValue:
    {
	return KJS::Boolean( fact->isValue( arg0 ) );
	break;
    }
    case MethodTypes:
    {
	KJS::List l;
	QStringList types = fact->types();
	for ( QStringList::Iterator it = types.begin(); it != types.end(); ++it ) {
	    l.append( KJS::String( *it ) );
	}

	return KJS::Object( exec->interpreter()->builtinArray().construct( exec, l ) );
	break;
    }
    case MethodListPlugins:
    {
	return convertToValue( exec, fact->listBindingPlugins( exec, self)  );
	break;
    }
    case MethodCreateRWPart:
    {

	kdDebug(80001) << "MethodCreateRWPart called, args.size is " << args.size() << endl;
	QObject *qo = 0;
	QObject *rwpart = 0;

	if ( args.size() == 3 ) {
	    JSObjectProxy *proxy = JSProxy::toObjectProxy( args[1].imp() );
	    qo = proxy ? proxy->object() : 0;
	    rwpart = fact->createRWPart( arg0, qo, args[2].toString(exec).ascii() );
	}
	else if ( args.size() == 4 ) {
	    JSObjectProxy *proxy = JSProxy::toObjectProxy( args[2].imp() );
	    qo = proxy ? proxy->object() : 0;
	    rwpart = fact->createRWPart( arg0, arg1, qo, args[3].toString(exec).ascii() );
	}

	if ( rwpart ) {
	    kdDebug(80001) << "should be returning kpart type " << rwpart->className() << endl;
	}
	else {
	    kdDebug(80001) << "could not create the kpart" << endl;
	}
	if ( rwpart )
	    return fact->createProxy( exec, rwpart );

	QString msg = i18n( "Unable to create read-write part for service '%1'." ).arg(arg0);
  err = throwError(exec, msg,KJS::ReferenceError);
	break;
    }
    case MethodCreateObject:
    {
	// Trim first argument from the list and forward the rest on.
	KJS::List objectArgs;
	if( args.size() > 1 )
	{
		for( int idx = 1; idx < args.size(); idx++)
			objectArgs.append(args[idx]);
	}
        return fact->create(exec,arg0, objectArgs);
        break;
    }
    default:
    {
	kdWarning() << "JSBuiltInImp has no method " << id << endl;
	QString msg = i18n( "JSFactoryImp has no method with id '%1'." ).arg( id );
  err = throwError(exec, msg,KJS::ReferenceError);
    }
    }
    return err;
}


void JSFactoryImp::publishChildren(QObject *obj, KJS::Object &parent)
{
	QString name = obj->name();
	name.remove(":");
	name.remove(" ");
	name.remove("<");
	name.remove(")");
	KJS::Object newParent = fact->part()->addObject( obj, parent, name.latin1() );
	JSProxy::toProxy( newParent.imp() ) ->setOwner( JSProxy::JavaScript );

	// Hack to publish QToolBoxes children
	QToolBox *box = dynamic_cast<QToolBox*>(obj);
	QTabWidget *tab = dynamic_cast<QTabWidget*>(obj);
	QWizard *wiz = dynamic_cast<QWizard*>(obj);

	if( box ) {
		int count = box->count();
		for( int idx = 0; idx < count; ++idx)
		publishChildren(box->item(idx),newParent);
	} else if ( tab ) {
		int count = tab->count();
		for( int idx = 0; idx < count; ++idx)
		publishChildren(tab->page(idx),newParent);
	} else if ( wiz ) {
		int count = wiz->pageCount();
		for( int idx = 0; idx < count; ++idx)
		publishChildren(wiz->page(idx),newParent);
	} else {
		const QObjectList *lst = obj->children();
		if ( lst ) {
			QObjectListIt it( *lst );
			QObject *child;
			while ( (child = it.current()) != 0 ) {
				publishChildren( child, newParent);
				++it;
			}
		}
	}
}

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:
