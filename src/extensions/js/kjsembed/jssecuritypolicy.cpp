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

#include "jssecuritypolicy.h"

namespace KJSEmbed {

JSSecurityPolicy *JSSecurityPolicy::policy=0;

JSSecurityPolicy::JSSecurityPolicy( uint capabilities )
    : caps( capabilities )
{
}

JSSecurityPolicy::~JSSecurityPolicy()
{
}

JSSecurityPolicy *JSSecurityPolicy::defaultPolicy()
{
    if ( !policy )
	policy = new JSSecurityPolicy( CapabilityAll );
    return policy;
}

void JSSecurityPolicy::setDefaultPolicy( JSSecurityPolicy *pol )
{
    if ( policy )
	delete policy;
    policy = pol ? pol : defaultPolicy();
}

void JSSecurityPolicy::setDefaultPolicy( uint caps )
{
    if ( policy )
	delete policy;
    policy = new JSSecurityPolicy( caps );
}

void JSSecurityPolicy::deleteDefaultPolicy()
{
    delete policy;
    policy = 0;
}

bool JSSecurityPolicy::isInterpreterAllowed( const JSObjectProxy *prx, const KJS::Interpreter *i ) const
{
    if ( !prx->interpreter() )
	return true;
    else if ( prx->interpreter() == i )
	return true;

    return false;
}

bool JSSecurityPolicy::isObjectAllowed( const JSObjectProxy *prx, const QObject *obj ) const
{
    if ( !obj )
	return false;

    do {
	if ( obj == prx->rootObject() )
	    return true;
    } while( (obj = obj->parent()) );

    return false;
}

bool JSSecurityPolicy::isPropertyAllowed( const JSObjectProxy *prx,
				const QObject *obj, const char * /*prop*/ ) const
{
    return hasCapability( CapabilityGetProperties|CapabilitySetProperties ) && isObjectAllowed( prx, obj );
}

bool JSSecurityPolicy::isCreateAllowed( const JSObjectProxy *prx, const QObject *parent,
				      const QString &/*clazz*/, const QString &/*name*/ ) const
{
    if ( hasCapability(CapabilityFactory) && isObjectAllowed( prx, parent ) )
	return true;
    if ( hasCapability(CapabilityTopLevel) && (parent == 0) )
	return true;
    return false;
}

} // namespace KJSEmbed


