// -*- c++ -*-

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

#ifndef KJSEMBEDJSPROXY_H
#define KJSEMBEDJSPROXY_H

#include <kjs/object.h>
#include "global.h"

namespace KJSEmbed {

class JSObjectProxy;
class JSValueProxy;
class JSOpaqueProxy;

/**
 * Base class for all proxy objects.
 *
 * @version $Id$
 * @author Richard Moore, rich@kde.org
 */
class KJSEMBED_EXPORT JSProxy : public KJS::ObjectImp
{
public:
    /** Identifiers for the various types of proxy. */
    enum ProxyType {
        UnknownProxy,
        ObjectProxy,
        ValueProxy,
        OpaqueProxy,
        BuiltinProxy
    };

  /** Identifies the owner of the currently wrapped pointer */
    enum Owner  {   Native, /** This is a C++ pointer and it will not be deleted by the garbage collector */
                    JavaScript /** This pointer is owned by Javascript and it will be deleted by the garbage collector */
                };
    /**
     * Structure to store information about a method.
     */
    struct MethodTable {
        int id;
        const char *name;
    };

    /**
    * Adds a @ref MethodTable of methods to an object.
    * Note: The method table MUST end in a {0,0} entry.
     */
    template< class T >
            static void addMethods( KJS::ExecState *exec, MethodTable *table, KJS::Object &object )
    {
        int idx = 0;
        if( !table[idx].name ) return; // check for empty list
        do {
            T *meth = new T( exec, table[idx].id );
            object.put( exec , table[idx].name, KJS::Object(meth), KJS::Function );
            ++idx;
        } while( table[idx].name );
    }

    /**
    * Structure to store information about enums.
     */ 
    struct EnumTable {
        const char *id;
        int val;
    };

    /**
     * Adds a @ref EnumTable of static values to an object.
     * Note: The method table MUST end in a {0,0} entry.
     */
    static void addEnums( KJS::ExecState *exec, EnumTable *table, KJS::Object &object );

    /**
     * Checks to see if an object is a desired type.
     * @param object - the object you want to test.
     * @param prxyType - the type of prxy you want.
     * @param classname - the name of the class you want.
     * @returns if the two above conditions are met.
     */
    static bool checkType( const KJS::Object &object, ProxyType prxyType, const QString &classname);

    
    JSProxy( int type );
    virtual ~JSProxy();

    /** Returns the string value of the class name **/
    virtual QString typeName() const = 0;
    /** Returns true iff the content of this proxy inherits the specified base-class. */
    virtual bool inherits( const char *clazz ) = 0;
    /** Returns the type of the proxy. */
    int proxyType() const { return proxytype; }

    /** Returns true iff this is a JSObjectProxy. */
    bool isObjectProxy() const { return proxytype == ObjectProxy; }

    /** Returns true iff this is a JSValueProxy. */
    bool isValueProxy() const { return proxytype == ValueProxy; }

    /** Returns true iff this is a JSOpaqueProxy. */
    bool isOpaqueProxy() const { return proxytype == OpaqueProxy; }

    /** Returns this object as a JSObjectProxy or 0 if the type is wrong. */
    JSObjectProxy *toObjectProxy();

    /** Returns this object as a JSValueProxy or 0 if the type is wrong. */
    JSValueProxy *toValueProxy();

    /** Returns this object as a JSOpaqueProxy or 0 if the type is wrong. */
    JSOpaqueProxy *toOpaqueProxy();

    /**
     * Converts the specified ValueImp to a JSProxy if possible, if the
     * ValueImp is not a JSProxy then 0 is returned.
     */
    static JSProxy *toProxy( KJS::ValueImp *imp );

    /**
     * Converts the specified ValueImp to a JSObjectProxy if possible, if the
     * ValueImp is not a JSObjectProxy then 0 is returned.
     */
    static JSObjectProxy *toObjectProxy( KJS::ValueImp *imp );

    /**
     * Converts the specified ValueImp to a JSValueProxy if possible, if the
     * ValueImp is not a JSValueProxy then 0 is returned.
     */
    static JSValueProxy *toValueProxy( KJS::ValueImp *imp );

    /**
     * Converts the specified ValueImp to a JSOpaqueProxy if possible, if the
     * ValueImp is not a JSOpaqueProxy then 0 is returned.
     */
    static JSOpaqueProxy *toOpaqueProxy( KJS::ValueImp *imp );

    /**
    * Transfers ownership of the contained pointer to C++ or Javascript.
    * JavaScript - Javascript GC will delete the pointer
    * Native - Javascript will not delete the pointer, but just the proxy.
    */
    void setOwner( Owner state = Native );

    /**
    * Returns the current ownership.
    */
    Owner owner() const;

protected:
    /**
    *  Who really owns the current pointer
    */
    Owner ownership;
private:
    int proxytype;
    class JSProxyPrivate *d;
};

} // namespace KJSEmbed

#endif // KJSEMBEDJSPROXYIMP_H

// Local Variables:
// c-basic-offset: 4
// End:
