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

#ifndef KJSEMBEDJSOBJECTPROXYIMP_H
#define KJSEMBEDJSOBJECTPROXYIMP_H

#include <qguardedptr.h>

#include <kjs/interpreter.h>
#include <kjsembed/jsproxy_imp.h>
#include <kjsembed/jsfactory.h>

namespace KJSEmbed {
namespace Bindings {

/**
 * Implements the JS methods of KJSEmbed::ObjectProxy.
 *
 * @author Richard Moore, rich@kde.org
 * @version $Id$
 */
class JSObjectProxyImp : public JSProxyImp
{
public:
    /** Identifiers for the method implementations available. */
    enum MethodId {
	/** Indicates an error, usually an supported signature. */
	MethodInternalError,

	/** Meta info about the object. */
	/* { */
	MethodProps,
	MethodIsWidgetType, MethodClassName, MethodSuperClassName,
	/* } */

	/** Methods for accessing the object tree. */
	/* { */
	MethodChildren, MethodParent, MethodChild, MethodChildCount,
	/* } */

	/** Fast slot signature. */
	MethodSlot,

	/** Custom slot signature. */
	MethodCustomSlot,

	/** Connections. */
	/* { */
	MethodSignals, MethodSlots,
	MethodConnect, MethodDisconnect,
        /* } */

	/** DOM methods. */
	/* { */
	MethodGetParentNode, MethodGetElementById, MethodGetElementsByTagName,
	MethodHasAttribute, MethodGetAttribute, MethodSetAttribute,
	/* } */

	/** Custom method. */
	MethodCustom = 0x1000
    };

    /**
     * Adds the tree bindings to the proxy.
     * <ul>
     * <li>parent()
     * <li>childAt(int index)
     * <li>childCount()
     * <li>findChild(string name)
     * <li>child(string name)
     * <li>child(int index)
     * </ul>
     */
    static void addBindingsTree( KJS::ExecState *exec, KJS::Object &object, JSObjectProxy *proxy );

    /**
     * Adds the DOM bindings to the proxy.
     * <ul>
     * <li>getElementById(name)
     * <li>hasAttribute(name)
     * <li>getAttribute(name)
     * <li>setAttribute(name,value)
     * </ul>
     */
    static void addBindingsDOM( KJS::ExecState *exec, KJS::Object &object, JSObjectProxy *proxy );

    /**
     * Adds the connect bindings to the proxy.
     * <ul>
     * <li>connect(signal,receiver,slot)
     * <li>disconnect(signal,receiver,slot)
     * </ul>
     */
    static void addBindingsConnect( KJS::ExecState *exec, KJS::Object &object, JSObjectProxy *proxy );


    JSObjectProxyImp( KJS::ExecState *exec,
		      int id, JSObjectProxy *parent );
    JSObjectProxyImp( KJS::ExecState *exec,
		      int id, const QCString &name, JSObjectProxy *parent );
    JSObjectProxyImp( KJS::ExecState *exec,
		      int id, int sigid, const QCString &name, JSObjectProxy *parent );
    JSObjectProxyImp( KJS::ExecState *exec,
		      int id, const char *ret, int sig, const QCString &name, JSObjectProxy *parent );
    ~JSObjectProxyImp() {}

    /** Implements the object tree interface. */
    /*{*/
    KJS::Value childAt( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );
    KJS::Value findChild( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

    KJS::Value children( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );
    KJS::Value properties( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );
    /*}*/

    /** Implements the DOM interface. */
    /*{*/
    KJS::Value getParentNode( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );
    KJS::Value getElementById( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );
    KJS::Value getElementsByTagName( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

    KJS::Value hasAttribute( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );
    KJS::Value getAttribute( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );
    KJS::Value setAttribute( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );
    /*}*/

    /** Calls the slots. */
    KJS::Value callSlot( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

    /** Implements signal-slot support. */
    /*{*/
    KJS::Value signalz( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );
    KJS::Value slotz( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

    KJS::Boolean connect( KJS::ExecState *exec, const KJS::Object &self, const KJS::List &args );
    KJS::Boolean disconnect( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

    KJS::Boolean connect( QObject *sender, const char *sig, const KJS::Object &recv, const QString &dest );
    /*}*/

    virtual bool implementsCall() const { return true; }
    virtual KJS::Value call( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

    const char *returnType() const { return rettype; }
    const char *slotName() const { return slotname; }
    int signature() const { return sigid; }
    JSObjectProxy *objectProxy() const { return proxy; }

protected:
    /** Calls a slot that has a custom signature. */
    KJS::Value callCustomSlot( KJS::ExecState *exec, KJS::Object &self, const KJS::List &args );

private:
    int id;
    QCString rettype; // return type
    int sigid; // signatureid
    int slotid;
    QCString slotname;
    JSObjectProxy *proxy;
    QGuardedPtr<QObject> obj;
    class JSObjectProxyImpPrivate *d;
};

} // namespace KJSEmbed::Bindings
} // namespace KJSEmbed

#endif // KJSEMBEDJSOBJECTPROXYIMP_H

// Local Variables:
// c-basic-offset: 4
// End:
