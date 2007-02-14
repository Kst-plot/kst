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

#include <qevent.h>
#include <qtextstream.h>

#include "global.h"

#include <kjs/interpreter.h>
#include <kjs/types.h>
#include <kjs/ustring.h>

#include "jsbinding.h"
#include "jsobjectproxy.h"
#include "jsobjectproxy_imp.h"
#include "jsvalueproxy_imp.h"

#include "jsopaqueproxy_imp.h"
#include "jsopaqueproxy.h"

namespace KJSEmbed {

typedef Bindings::JSOpaqueProxyImp JSOpaqueProxyImp;

JSOpaqueProxy::JSOpaqueProxy()
    : JSProxy( JSProxy::OpaqueProxy ), ptr(0)
{
}

JSOpaqueProxy::JSOpaqueProxy( QTextStream *ts )
    : JSProxy( JSProxy::OpaqueProxy ), ptr(0)
{
    setValue( ts );
}

JSOpaqueProxy::JSOpaqueProxy( const QEvent *ev )
    : JSProxy( JSProxy::OpaqueProxy ), ptr(0)
{
    setValue( ev );
}

JSOpaqueProxy::~JSOpaqueProxy()
{
	kdDebug() << "Delete JSOpaqueProxy " << endl;

    if( ptr ) {
      if(owner() == JavaScript
      	&& ptr->type() != typeid(void) ) ptr->cleanup();
      delete ptr;
    }
}

QString JSOpaqueProxy::typeName() const
{
    return ptrtype;
}

bool JSOpaqueProxy::inherits( const char *clazz )
{
    return (ptrtype == clazz);
}

void JSOpaqueProxy::setValue( QTextStream *ts )
{
  if( ptr ) {
    if(owner() == JavaScript) ptr->cleanup();
    delete ptr;
  }
  ptr = new Pointer<QTextStream>(ts);
  ptrtype = "QTextStream";
}

void JSOpaqueProxy::setValue( const QEvent *ev )
{
  if( ptr ) {
    if(owner() == JavaScript) ptr->cleanup();
    delete ptr;
  }
  ptr = new Pointer<const QEvent>(ev);
  ptrtype = "QEvent";
}

QTextStream *JSOpaqueProxy::toTextStream()
{
    return toNative<QTextStream>();
}

const QEvent *JSOpaqueProxy::toEvent()
{
    return toNative<QEvent>();
}

void JSOpaqueProxy::addBindings( KJS::ExecState *exec, KJS::Object &object )
{
    MethodTable methods[] = {
	{ JSOpaqueProxyImp::MethodTypeName, "typeName" },
	{ 0, 0 }
    };

    int i = 0;
    do {
	JSOpaqueProxyImp *obj = new JSOpaqueProxyImp( exec, methods[i].id, this );
	obj->setName( KJS::Identifier( methods[i].name ) );
	object.put( exec , obj->name(), KJS::Object(obj) );
	i++;
    } while( methods[i].id );
}

KJS::UString JSOpaqueProxy::toString( KJS::ExecState */*exec*/ ) const
{
    QString s( "%1 (%2)" );
    return KJS::UString( s.arg("JSOpaqueProxy").arg(ptrtype) );
}


} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:

