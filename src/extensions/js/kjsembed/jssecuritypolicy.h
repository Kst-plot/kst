// -*- c++ -*-

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

#ifndef KJSEMBEDSECURITYPOLICY_H
#define KJSEMBEDSECURITYPOLICY_H
#include "global.h"
#include <kjsembed/jsobjectproxy.h>

namespace KJSEmbed {

class JSObjectProxy;

/**
 * Implements the default security policy.
 * This class defines the interface for security policies, and provides
 * a default implementation that should be sufficient for most situations.
 * <h3>Security Checks</h3>
 * A SecurityPolicy must define tests for the following situations:
 * <ul>
 * <li>Does this request come from the correct @ref KJS::Interpreter ?
 * <li>Is the script allowed to access the specified QObject ?
 * <li>Is the script allowed to access the specified property ?
 * </ul>
 * The various isAllowed() methods are called automatically by @ref JSObjectProxy
 * when a script attempts to perform a controlled operation.
 *
 * <h3>Default Policy</h3>
 * The security policy applied by default is as follows:
 * <ul>
 * <li>Only the interpreter specified when the initial binding was
 * defined may access the proxy.
 * <li>Scripts may only access QObjects that are children of the root
 * object. The root object is specified when the embedding application
 * creates the initial binding, and is automatically inherited by any
 * sub-proxies that may be created by the script.
 * <li>Scripts may access any properties of the objects they can access.
 * </ul>
 * Applications tjat want a custom policy should define a SecurityPolicy
 * that re-implement the various isAllowed() methods, then use
 * @ref JSObjectProxy::setSecurityPolicy() to apply the policy to a proxy.
 *
 * @see KJSEmbed::JSObjectProxy
 * @author Richard Moore, rich@kde.org
 * $Id$
 */
class KJSEMBED_EXPORT JSSecurityPolicy
{
public:
    JSSecurityPolicy( uint capabilities = 0 );
    virtual ~JSSecurityPolicy();

    /**
     * Defines a set of flags that indicate if access to a given API
     * should be allowed.
     */
    enum Capabilities {
	CapabilityTree = 0x0001,
	CapabilityGetProperties = 0x0002,
	CapabilitySetProperties = 0x0004,
	CapabilitySlots = 0x0008,
	CapabilityFactory = 0x0010,
	CapabilityTopLevel = 0x0020,
	CapabilityCustom = 0x4000,

	CapabilityNone = 0x0000,
	CapabilityReadOnly = CapabilityTree | CapabilityGetProperties,
	CapabilityReadWrite = CapabilityReadOnly | CapabilitySetProperties | CapabilitySlots,
	CapabilityAll = 0xffff
    };

    /**
     * Returns true if any of the specified capabilities are allowed.
     */
    bool hasCapability( uint cap ) const { return (cap & caps) ? true : false; }

    /**
     * Specifies that the passed capabilities are allowed.
     */
    void setCapability( uint cap ) { caps = caps | cap; }

    /**
     * Specifies that the passed capabilities are disallowed.
     */
    void clearCapability( uint cap ) { caps = caps | (CapabilityAll & cap); }

    /**
     * Returns true if the specified interpreter may access the proxy.
     */
    virtual bool isInterpreterAllowed( const JSObjectProxy *prx, const KJS::Interpreter *interp ) const;

    /**
     * Returns true if scripts are allowed to see the specified QObject.
     */
    virtual bool isObjectAllowed( const JSObjectProxy *prx, const QObject *obj ) const;

    /**
     * Returns true if scripts are allowed to see the specified property.
     */
    virtual bool isPropertyAllowed( const JSObjectProxy *prx, const QObject *obj, const char *prop ) const;

    /**
     * Returns true if scripts are allowed to create the specified child object.
     */
    virtual bool isCreateAllowed( const JSObjectProxy *prx, const QObject *parent,
				  const QString &clazz, const QString &name ) const;

    /**
     * Returns the default SecurityPolicy.
     */
    static JSSecurityPolicy *defaultPolicy();

    /**
     * Sets the default SecurityPolicy.
     */
    static void setDefaultPolicy( JSSecurityPolicy *pol );

    /**
     * Sets the default SecurityPolicy.
     */
    static void setDefaultPolicy( uint capabilities );

    /**
     * Deletes the default SecurityPolicy.
     */
    static void deleteDefaultPolicy();

private:
    static JSSecurityPolicy *policy;
    uint caps;
    class JSSecurityPolicyPrivate *d;
};

} // namespace KJSEmbed

#endif // KJSEMBEDSECURITYPOLICY_H
