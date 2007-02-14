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

#ifndef KJSEMBEDJSOBJECTPROXY_H
#define KJSEMBEDJSOBJECTPROXY_H
#include "global.h"
#include <qguardedptr.h>
#include <qcstring.h>
#include <qwidget.h>

#include <kjs/object.h>
#include <kjsembed/jsproxy.h>
#include <kjsembed/jsbinding.h>

namespace KJS { class Interpreter; }

namespace KJSEmbed {

namespace Bindings { class JSObjectProxyImp; }

class JSSecurityPolicy;
class KJSEmbedPart;
class JSObjectEventProxy;

/**
 * A JS object that provides a binding to a QObject.
 * <h3>Introduction</h3>
 * This class defines a @ref KJS::ObjectImp that allows scripts to access
 * the properties of a QObject. The scripts can also create child objects,
 * load dialogs from .ui files created by Designer and navigate the  object
 * tree in a similar manner to the DCOP/QObject bridge.
 * <h3>Example Usage</h3>
 * The following example creates a @ref KJS::ObjectImp that provides
 * a binding to the properties of a @ref QLineEdit . This binding is
 * then used to create a property 'edit' for the object 'jsparent'.
 * <pre>
 *
 *   QLineEdit *edit = new QLineEdit();
 *   KJSEmbed::JSObjectProxy *proxy = new KJSEmbed::JSObjectProxy( js, edit );
 *   jsparent.put( js->globalExec(), "edit", proxy );
 *
 * </pre>
 *
 * <h3>Security Facilities</h3>
 * In order to ensure scripts don't run amok and to ensure the script
 * interpreter used by KHTML remains secure, JSObjectProxy applies a
 * security policy. Every time a script tries to access an object or
 * property the following tests are performed:
 * <ul>
 * <li>Does this request come from the correct @ref KJS::Interpreter?
 * <li>Is the script allowed to access the specified QObject?
 * <li>Is the script allowed to access the specified property?
 * </ul>
 * The @ref KJSEmbed::JSSecurityPolicy class decides if the request should
 * be granted. The security policy is automatically inherited by any child
 * proxies created using the object tree accessor methods.
 *
 * @see KJSEmbed::JSFactory
 * @author Richard Moore, rich@kde.org
 * $Id$
 */
class KJSEMBED_EXPORT JSObjectProxy : public JSProxy
{
public:
    /**
     * Create a JS binding to the target object. The binding will allow scripts to
     * access any QObject that is descended the target and no others.
     */
    JSObjectProxy( KJSEmbedPart *part, QObject *target );

    /**
     * Create a JS binding to the target object. The binding will allow scripts to
     * access any QObject that is descended from the specified root. If the specified
     * root is 0 then access is granted to all objects.
     */
    JSObjectProxy( KJSEmbedPart *part, QObject *target, QObject *root );

    /**
     * Create a JS binding to the target object. The binding will allow scripts to
     * access any QObject that is descended from the specified root, according to
     * the specified @ref JSSecurityPolicy . If the specified root is 0 then access
     * is granted to all objects.
     */
    JSObjectProxy( KJSEmbedPart *part, QObject *target, QObject *root, const JSSecurityPolicy *sp );

    virtual ~JSObjectProxy();

    /** Returns the KJSEmbedPart in which this proxy lives. */
    KJSEmbedPart *part() const { return jspart; }

    /** Returns the interpreter in which this proxy lives. */
    KJS::Interpreter *interpreter() const { return js; }

    /** Returns the root object that defines the limit of the scope of this proxy. */
    QObject *rootObject() const { return root; }

    /** Returns the QObject the proxy is attached to. */
    QObject *object() const { return obj; }

    /** Returns the className of the proxied object */
    QString typeName() const { return obj->className(); }

    /** Returns the associated QWidget, or 0 if the object is not a widget. */
    QWidget *widget() const
	{
	    QObject *w = obj;
	    return (w && w->isWidgetType()) ? static_cast<QWidget *>(w) : 0;
	}

    //void *toVoidStar() { return obj; }
    //template<class T>
    //T *toNative(){ return dynamic_cast<QObject*>(obj); }

    /** Returns true iff the content of this proxy inherits the specified base-class. */
    bool inherits( const char *clazz ) { return obj->isA( clazz ); }

    /** Returns the @ref JSSecurityPolicy of the proxy. */
    const JSSecurityPolicy *securityPolicy() const { return policy; }

    /**
     * Specifies the @ref JSSecurityPolicy that should be applied to this proxy.
     * Setting the policy to 0 restores the default policy.
     */
    void setSecurityPolicy( const JSSecurityPolicy *sp );

    /** Reimplemented to return the value of the specified property if present. */
    virtual KJS::Value get( KJS::ExecState *exec, const KJS::Identifier &p ) const;

    /** Reimplemented to set the value of the specified property if possible. */
    virtual void put( KJS::ExecState *exec, const KJS::Identifier &p,
		      const KJS::Value &v, int attr = KJS::None );

    /** Reimplemented to return the name and class of the target. */
    virtual KJS::UString toString( KJS::ExecState *exec ) const;

    /**
     * Adds methods for traversing the QObject tree to the specified
     * @ref KJS::Object . Only QObjects descended from the root specified
     * in the constructor can be reached through JS proxies created with
     * these bindings.
     * <ul>
     * <li>properties()
     * <li>create(...)
     * </ul>
     */
    virtual void addBindings( KJS::ExecState *exec, KJS::Object &object );

protected:
    void addBindingsClass( KJS::ExecState *exec, KJS::Object &object );

    /**
     * Adds bindings for the constants defined by enums of the target.
     */
    void addBindingsEnum( KJS::ExecState *exec, KJS::Object &object );

    /**
     * Adds bindings for slots defined by the target of the proxy.
     */
    void addBindingsSlots( KJS::ExecState *exec, KJS::Object &object );


private:
    bool isAllowed( KJS::Interpreter *js ) const;

    void addSlotBinding( const QCString &name, KJS::ExecState *exec, KJS::Object &object );

private:
    KJSEmbedPart *jspart;
    KJS::Interpreter *js;
    QGuardedPtr<QObject> obj;
    QGuardedPtr<QObject> root;
    QGuardedPtr<JSObjectEventProxy> evproxy;
    const JSSecurityPolicy *policy;
    class JSObjectProxyPrivate *d;
    friend class Bindings::JSObjectProxyImp;
};

} // namespace KJSEmbed

#endif // KJSEMBEDJSOBJECTPROXY_H

// Local Variables:
// c-basic-offset: 4
// End:
