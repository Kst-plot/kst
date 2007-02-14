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

#include <kdebug.h>
#include <kjsembed/jsobjectproxy.h>
#include <kjsembed/jsbinding.h>
#include <kjsembed/kjsembedpart.h>
#include <kjsembed/customobject_imp.h>
#include <qvariant.h>


#include "customqobject_plugin.h"

namespace KJSEmbed {
namespace Bindings {

MyCustomQObjectLoader::MyCustomQObjectLoader( QObject *parent, const char *name, const QStringList &args ) :
	JSBindingPlugin(parent, name, args)
{
}

KJS::Object MyCustomQObjectLoader::createBinding(KJSEmbedPart *jspart, KJS::ExecState *exec, const KJS::List &args) const
{
	kdDebug() << "Loading a custom object" << endl;

	QObject *parent = 0L;
	JSObjectProxy *proxy = JSProxy::toObjectProxy( args[0].imp() );
	if ( proxy )
		parent = proxy->object();
	MyCustomQObjectImp *imp =  new MyCustomQObjectImp(parent, "MyCustomQObject" );
	JSObjectProxy *prx = new JSObjectProxy( jspart, imp );

	KJS::Object proxyObj( prx );
	prx->addBindings( exec, proxyObj );
	CustomObjectImp::addBindings( exec, proxyObj );
	return proxyObj;
}

MyCustomQObjectImp::MyCustomQObjectImp(QObject *parent, const char *name )
    : QObject(parent, name)
{
	kdDebug() << "New MyCustomQObjectImp " << endl;
}

MyCustomQObjectImp::~MyCustomQObjectImp()
{
}

MyCustomQObjectImp::Mode MyCustomQObjectImp::mode() const
{
	kdDebug() << "mode() " << endl;
	return m_mode;
}
void MyCustomQObjectImp::setMode( Mode md)
{
	kdDebug() << "setMode() " << endl;
	m_mode = md;
}
QString MyCustomQObjectImp::thing() const
{
	kdDebug() << "thing()" << endl;
	return m_thing;
}
void MyCustomQObjectImp::setThing( const QString &t)
{
	kdDebug() << "setThing() " << t << endl;
	m_thing = t;
}
} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed


#include <kgenericfactory.h>
typedef KGenericFactory<KJSEmbed::Bindings::MyCustomQObjectLoader> MyCustomQObjectLoaderFactory;
K_EXPORT_COMPONENT_FACTORY( libcustomqobjectplugin, MyCustomQObjectLoaderFactory( "MyCustomQObjectLoader" ) )


#include "customqobject_plugin.moc"
