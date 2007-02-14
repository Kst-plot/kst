/*
 *  Copyright (C) 2001-2004, Ian Reinhart Geiser <geiseri@kde.org>
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
#include "jsbuiltinproxy.h"
namespace KJSEmbed {
JSBuiltinProxy::JSBuiltinProxy( const QString &name ) : JSProxy( JSProxy::BuiltinProxy ), m_name(name)
{

}

JSBuiltinProxy::~JSBuiltinProxy()
{

}

QString JSBuiltinProxy::typeName() const
{
    return m_name;
}

bool JSBuiltinProxy::inherits( const char *clazz )
{
   if( m_name == clazz )  return true;
   return false;
}

KJS::UString JSBuiltinProxy::toString( KJS::ExecState */*exec*/ ) const
{
  QString s( "%1 (%2)" );
  return KJS::UString( s.arg("JSBuiltinProxy").arg(m_name).latin1() );
}

}
