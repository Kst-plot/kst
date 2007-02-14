

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

#include "global.h"

#include "jsobjectproxy.h"
#include "jsvalueproxy.h"
#include "jsopaqueproxy.h"

#include "jsproxy.h"

namespace KJSEmbed {

JSProxy::JSProxy( int type )
    : KJS::ObjectImp(), ownership(Native),
      proxytype(type)
{
}

JSProxy::~JSProxy()
{
    if( ownership == Native )
        kdDebug() << "C++ JSProxy " << proxytype << " going away" << endl;
    else
        kdDebug() << "JS JSProxy " << proxytype << " going away" << endl;
}

JSObjectProxy *JSProxy::toObjectProxy()
{
    return dynamic_cast<JSObjectProxy *>( this );
}

JSValueProxy *JSProxy::toValueProxy()
{
    return dynamic_cast<JSValueProxy *>( this );
}

JSOpaqueProxy *JSProxy::toOpaqueProxy()
{
    return dynamic_cast<JSOpaqueProxy *>( this );
}
void JSProxy::setOwner( Owner state )
{
    kdDebug(80001) << "Set ownership for " << typeName() << " to " << state << endl;
    ownership = state;
}


JSProxy::Owner JSProxy::owner( ) const
{ return ownership; }

bool JSProxy::checkType( const KJS::Object &object, ProxyType prxyType, const QString & classname )
{
    JSProxy *prxy = toProxy(object.imp());
    if( !prxy )
        return false;
    if( prxy->proxyType() != prxyType )
        return false;
    if( prxy->typeName() != classname )
        return false;
    return true;
}


//
// Static conversion methods
//
JSProxy *JSProxy::toProxy( KJS::ValueImp *imp )
{
    return dynamic_cast<JSProxy *>( imp );
}

JSObjectProxy *JSProxy::toObjectProxy( KJS::ValueImp *imp )
{
    return dynamic_cast<JSObjectProxy *>( imp );
}

JSValueProxy *JSProxy::toValueProxy( KJS::ValueImp *imp )
{
    return dynamic_cast<JSValueProxy *>( imp );
}

JSOpaqueProxy *JSProxy::toOpaqueProxy( KJS::ValueImp *imp )
{
    return dynamic_cast<JSOpaqueProxy *>( imp );
}

void JSProxy::addEnums( KJS::ExecState * exec, EnumTable * table, KJS::Object & object )
{
    int enumidx = 0;
    if( !table[enumidx].id ) return; // check for empty list
    do {
        object.put( exec, table[enumidx].id, KJS::Number(table[enumidx].val), KJS::ReadOnly );
        ++enumidx;
    } while( table[enumidx].id );
}

}

 // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:
