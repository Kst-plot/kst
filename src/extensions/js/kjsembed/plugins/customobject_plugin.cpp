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
#include <kjsembed/jsopaqueproxy.h>
#include <kjsembed/jsbinding.h>
#include <kjsembed/jsfactory.h>
#include <kjsembed/jsfactory_imp.h>
#include <kjsembed/kjsembedpart.h>
#include <kjsembed/customobject_imp.h>
#include <qvariant.h>
#include <qbrush.h>

#include "customobject_plugin.h"

namespace KJSEmbed {
namespace Bindings {

class MyCustomObject
{
	public:
	enum Mode { On, Off };
	Mode mode;
	QString thing;
};

MyCustomObjectLoader::MyCustomObjectLoader( QObject *parent, const char *name, const QStringList &args ) :
	JSBindingPlugin(parent, name, args)
{
}

KJS::Object MyCustomObjectLoader::createBinding(KJSEmbedPart */*jspart*/, KJS::ExecState *exec, const KJS::List &/*args*/) const
{
	kdDebug() << "Loading a custom object" << endl;
	MyCustomObject *obj = new MyCustomObject();
	JSOpaqueProxy *prx = new JSOpaqueProxy(  (void *) obj, "MyCustomObject" );

	KJS::Object proxyObj(prx);
	MyCustomObjectImp::addBindings( exec, proxyObj );
	return proxyObj;
}

MyCustomObjectImp::MyCustomObjectImp( KJS::ExecState *exec, int id )
    : JSProxyImp(exec), mid(id)
{
}

MyCustomObjectImp::~MyCustomObjectImp()
{
}

void MyCustomObjectImp::addBindings( KJS::ExecState *exec, KJS::Object &object ) {

    kdDebug() << "MyCustomObjectImp::addBindings()" << endl;
    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( object.imp() );
    if ( !op ) {
        kdWarning() << "MyCustomObjectImp::addBindings() failed, not a JSOpaqueProxy" << endl;
        return;
    }

    if ( op->typeName() != "MyCustomObject" ) {
	kdWarning() << "MyCustomObjectImp::addBindings() failed, type is " << op->typeName() << endl;
	return;
    }

    JSProxy::MethodTable methods[] = {
	{ Methodmode, "mode"},
	{ MethodsetMode, "setMode"},
	{ Methodthing,  "thing"},
	{ MethodsetThing, "setThing"},
	{ 0, 0 }
    };

    int idx = 0;
    do {
        MyCustomObjectImp *meth = new MyCustomObjectImp( exec, methods[idx].id );
        object.put( exec , methods[idx].name, KJS::Object(meth) );
        ++idx;
    } while( methods[idx].id );

    //
    // Define the enum constants
    //
    struct EnumValue {
	const char *id;
	int val;
    };

    EnumValue enums[] = {
	// MyCustomObject::mode
	{ "On", 0 },
	{ "Off", 1 },
	{ 0, 0 }
    };

    int enumidx = 0;
    do {
        object.put( exec, enums[enumidx].id, KJS::Number(enums[enumidx].val), KJS::ReadOnly );
        ++enumidx;
    } while( enums[enumidx].id );
}

KJS::Value MyCustomObjectImp::call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args ) {

    kdDebug() << "MyCustomObjectImp::call() " << mid << endl;
    JSOpaqueProxy *op = JSProxy::toOpaqueProxy( self.imp() );
    if ( !op ) {
        kdWarning() << "MyCustomObjectImp::call() failed, not a JSOpaqueProxy" << endl;
        return KJS::Value();
    }

    if ( op->typeName() != "MyCustomObject" ) {
	kdWarning() << "MyCustomObjectImp::call() failed, type is " << op->typeName() << endl;
	return KJS::Value();
    }

    MyCustomObject *obj = op->toNative<MyCustomObject>();

    KJS::Value retValue = KJS::Value();
    switch ( mid ) {
    case Methodthing:
    {
	retValue = KJS::String(obj->thing);
	break;
    }
    case MethodsetThing:
    {
	obj->thing = extractString(exec, args, 0);
	break;
    }
    case Methodmode:
    {
	retValue = KJS::Number( (int)obj->mode );
	break;
    }
    case MethodsetMode:
    {
	obj->mode = (MyCustomObject::Mode) extractInt(exec, args, 0);
	break;
    }
    default:
        kdWarning() << "MyCustomObject has no method " << mid << endl;
        break;
    }

    op->setValue((void*) obj, "MyCustomObject");
    return retValue;
}


int MyCustomObjectImp::extractInt( KJS::ExecState *exec, const KJS::List &args, int idx)
{
     return (args.size() > idx) ? args[idx].toInteger(exec) : 0;
}
QString MyCustomObjectImp::extractString(KJS::ExecState *exec, const KJS::List &args, int idx)
{
     return (args.size() > idx) ? args[idx].toString(exec).qstring() : QString::null;
}

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

#include <kgenericfactory.h>
typedef KGenericFactory<KJSEmbed::Bindings::MyCustomObjectLoader> MyCustomObjectLoaderFactory;
K_EXPORT_COMPONENT_FACTORY( libcustomobjectplugin, MyCustomObjectLoaderFactory( "MyCustomObjectLoader" ) )
