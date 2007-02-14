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

#include <qvariant.h>
#include "global.h"

#include <kjs/interpreter.h>
#include <kjs/types.h>
#include <kjs/ustring.h>

#include "jsbinding.h"
#include "jsobjectproxy.h"
#include "jsobjectproxy_imp.h"
#include "jsvalueproxy_imp.h"

#include "jsvalueproxy.h"

namespace KJSEmbed {

typedef Bindings::JSValueProxyImp JSValueProxyImp;

JSValueProxy::JSValueProxy()
    : JSProxy( JSProxy::ValueProxy )
{
}

JSValueProxy::~JSValueProxy()
{
}

QString JSValueProxy::typeName() const
{
    return val.typeName();
}

void JSValueProxy::setValue( const QVariant &var )
{
    kdDebug(80001) << "JSValueProxy::setValue() variant, type is " << var.typeName() << endl;
    val = var;
}

QVariant JSValueProxy::toVariant() const 
{ 
    kdDebug(80001) << "JSValueProxy::toVariant() variant, type is " << val.typeName() << endl;
    return val; 
}

void JSValueProxy::addBindings(  KJS::ExecState *exec, KJS::Object &object )
{
    MethodTable methods[] = {
    { JSValueProxyImp::MethodTypeName, "typeName" },
    { 0, 0 }
    };
        int i = 0;
    do {
        JSValueProxyImp *obj = new JSValueProxyImp( exec, methods[i].id, this );
        obj->setName( KJS::Identifier( methods[i].name ) );
        object.put( exec , obj->name(), KJS::Object(obj) );
        i++;
    } while( methods[i].id );
}

KJS::UString JSValueProxy::toString( KJS::ExecState * /*exec*/ ) const
{
    QString s( "%1 (%2)" );
    return KJS::UString( s.arg("JSValueProxy").arg(val.typeName()) );
}

bool JSValueProxy::inherits( const char * clazz )
{
    return ( typeName() == clazz );
}

}

 // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:

