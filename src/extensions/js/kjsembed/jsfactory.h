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

#ifndef KJSEMBEDFACTORY_H
#define KJSEMBEDFACTORY_H
#include "global.h"
#include <qmap.h>
#include <qdict.h>
#include <qstringlist.h>

#include <kjs/interpreter.h>
#include <kjs/object.h>

class QEvent;
class QObject;
class QWidget;
class QTextStream;

namespace KParts {
    class ReadOnlyPart;
    class ReadWritePart;
}

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

namespace Bindings {
    class JSFactoryImp;
    class JSBindingPlugin;
    class JSBindingBase;
}

class KJSEmbedPart;
class JSEventMapper;
class JSOpaqueProxy;
class JSObjectProxy;

/**
 * Factory class for KJSEmbed.
 *
 * @author Richard Moore, rich@kde.org
 */
class KJSEMBED_EXPORT JSFactory
{
public:
    /**
     * The types of proxy that can be associated with a class name. These are
     * used when decoding the return values of slots.
     */
    enum ProxyTypes {
	TypeInvalid=0,
        TypeQObject=1,
        TypeValue=2,
	TypeOpaque=4,
	TypePlugin=8,
	TypeQObjectPlugin=9 /*TypeObject|TypePlugin*/
    };

    /** Creates a factory object for the specified part. */
    JSFactory( KJSEmbedPart *part );

    /** Cleans up. */
    virtual ~JSFactory();

    /**
     * Returns the mapper that translates between event types and the names of
     * the handler methods.
     */
    JSEventMapper *eventMapper() const { return evmapper; }

    /** Returns the part that the factory is attached to. */
    KJSEmbedPart *part() const { return jspart; }

    //
    // Methods for creating objects and wrappers.
    //

    /** Creates an object of the specified class, then returns a proxy. */
    KJS::Object create( KJS::ExecState *exec, const QString &classname, const KJS::List &args );

    /** Creates a proxy object for the specified target with the specified context. */
    KJS::Object createProxy( KJS::ExecState *exec, QObject *target,
			     const JSObjectProxy *context=0 ) const;

    /** Creates a proxy object for the specified target with the specified context. */
    KJS::Object createProxy( KJS::ExecState *exec, QTextStream *target,
			     const JSObjectProxy *context=0 ) const;

    /** Creates a proxy object for the specified target with the specified context. */
    KJS::Object createProxy( KJS::ExecState *exec, QEvent *target,
			     const JSObjectProxy *context ) const;
    
    
    /**
    * Registers an opaque proxy factory with the with the factory.
    * To add an opaque type to the system you need to provide a factory based off of @ref KJSEmbed::Bindings::JSBindingBase
     */
    void registerOpaqueType( const QString &className, KJSEmbed::Bindings::JSBindingBase *bindingFactory);

    /**
    * Remove an opaque type from the system.
     */
    void unregisterOpaqueType( const QString &className );

    /**
     * When passed an object for a type handled by an Opaque proxy this method
     * will add the bindings for that opaque object to the proxy.  If the object
     * is not supported it is unmodified.
     */
    void extendOpaqueProxy( KJS::ExecState *exec, KJS::Object &proxy) const;

    /**
    * Registers a QObject proxy factory with the with the factory.
    * To add an opaque type to the system you need to provide a factory based off of @ref KJSEmbed::Bindings::JSBindingBase
     */
    void registerObjectType( const QString &className, KJSEmbed::Bindings::JSBindingBase *bindingFactory);

    /**
    * Remove an opaque type from the system.
     */
    void unregisterObjectType( const QString &className );

    /**
     * When passed an object for a type handled by an Opaque proxy this method
     * will add the bindings for that opaque object to the proxy.  If the object
     * is not supported it is unmodified.
     */
    void extendObjectProxy( KJS::ExecState *exec, KJS::Object &proxy) const;

    
    //
    // Methods for creating custom objects.
    //

    /**
     * Creates a ReadOnlyPart that views the specified service type. The
     * service type will often be a MIME type like 'text/html'. The part
     * created is the first offer returned by the trader, and will have the
     * specified parent and name.
     */
    KParts::ReadOnlyPart *createROPart( const QString &svc, QObject *parent=0, const char *name=0 );

    /**
     * Creates a ReadOnlyPart that views the specified service type and matches
     * the specified constraint.
     */
    KParts::ReadOnlyPart *createROPart( const QString &svc, const QString &constraint,
					QObject *parent=0, const char *name=0 );

    /**
     * Creates a ReadOnlyPart that views the specified service type and matches
     * the specified constraint.
     */
    KParts::ReadOnlyPart *createROPart( const QString &svc, const QString &constraint,
					QObject *parent, const char *name,
					const QStringList &args );

    /**
     * Creates a ReadWritePart that edits the specified service type. The
     * service type will often be a MIME type like 'text/plain'. The part
     * created is the first offer returned by the trader, and will have the
     * specified parent and name.
     */
    KParts::ReadWritePart *createRWPart( const QString &svc, QObject *parent=0, const char *name=0 );

    /**
     * Creates a ReadWritePart that edits the specified service type and matches
     * the specified constraint.
     */
    KParts::ReadWritePart *createRWPart( const QString &svc, const QString &constraint,
					 QObject *parent=0, const char *name=0 );

    /**
     * Creates a ReadWritePart that edits the specified service type and matches
     * the specified constraint.
     */
    KParts::ReadWritePart *createRWPart( const QString &svc, const QString &constraint,
					 QObject *parent, const char *name,
					 const QStringList &args );

    /**
     * Loads the widget defined in the specified .ui file. If the widget
     * cannot be created then 0 is returned.
     */
    QWidget *loadUI( const QString &uiFile, QObject *connector=0, QWidget *parent=0, const char *name=0 );

    /**
    * Queries KTrader for a plugin that provides the asked for object binding.
    *If the binding was added the object is returned, otherwise a KJS::Null is.
    */
    QStringList listBindingPlugins( KJS::ExecState *exec, KJS::Object &self);
    //
    // Methods that tell the factory how to handle different classes.
    //
    /**
    * Adds a binding plugin type to the list of available types the factory can create.
    */
    void addBindingPluginTypes(KJS::ExecState *exec, KJS::Object &parent);
    bool isBindingPlugin(const QString &classname) const;
    /**
    * Creates the actual object from the binding plugin.
    */
    KJS::Object createBindingPlugin(KJS::ExecState *exec, const QString &classname, const KJS::List &args );
    /**
     * Returns true iff the factory knows the type of proxy to use for the
     * class with the name specified.
     */
    bool isSupported( const QString &clazz ) const;

    /**
     * Returns true iff the class with the specified name is handled with the
     * QObject proxy type.
     */
    bool isQObject( const QString &clazz ) const;

    /**
     * Returns true iff the class with the specified name is handled with the
     * value proxy type.
     */
    bool isValue( const QString &clazz ) const;

    /**
     * Returns true iff the class with the specified name is handled with the
     * opaque proxy type.
     */
    bool isOpaque( const QString &clazz ) const;

    /**
     * Returns the ProxyType of the class with the specified name. If the
     * named class is not known to the interpreter then TypeInvalid is
     * returned.
     */
    uint proxyType( const QString &clazz ) const;


    /**
     *Allows adding of an already loaded binding plugin for a certain class type
     *
     */
    void addQObjectPlugin(const QString &classname, KJSEmbed::Bindings::JSBindingPlugin* plugin);

    void addBindingsPlugin(KJS::ExecState *exec, KJS::Object &target) const;

    /**
     * Tells the factory the specified type of proxy to use for the named
     * class. Note that you can remove support for a type by using this method
     * with TypeInvalid.
     */
    void addType( const QString &clazz, uint proxytype=JSFactory::TypeQObject );

    /** Adds the types defined by the factory to the specified parent. */
    void addTypes( KJS::ExecState *exec, KJS::Object &parent );

    QStringList types() const;

protected:
    /** Adds custom bindings to the specified proxy object. */
    KJS::Object extendProxy( KJS::ExecState *exec, KJS::Object &target ) const;

    /** Creates an instance of the named class and returns it in a JSValueProxy. */
    KJS::Object createValue( KJS::ExecState *exec, const QString &cname, const KJS::List &args );

    /**
     * Creates an instance of a QObject subclass. If the instance cannot be
     * created then 0 is returned.
     */
    QObject *create( const QString &classname, QObject *parent=0, const char *name=0  );

    /** Creates an instance of the named class and returns it in a JSOpaqueProxy. */
    KJS::Object createOpaque( KJS::ExecState *exec, const QString &cname, const KJS::List &args );


    /** Creates an instance of the named binding QObject. */
    QObject *createBinding( const QString &cname, QObject *parent, const char *name );

    /**
     * Creates an instance of the named QObject. This method is only used for
     * non-widget objects.
     */
    QObject *createObject( const QString &cname, QObject *parent, const char *name );

    /**
     * Creates an instance of the named QWidget. Note that this method is only
     * used to create widgets that are not supported by QWidgetFactory.
     */
    QWidget *createWidget( const QString &cname, QWidget *parent, const char *name );

private:
    
    /** Adds the types defined by QWidgetFactory to the specified parent. */
    void addWidgetFactoryTypes( KJS::ExecState *exec, KJS::Object &parent );

    /** Adds support for custom QObject types to the specified parent. */
    void addCustomTypes( KJS::ExecState *exec, KJS::Object &parent );

    /** Adds support for QObject binding types to the specified parent. */
    void addBindingTypes( KJS::ExecState *exec, KJS::Object &parent );

    /**
     * Adds support for any QObject types that are known about, but have no
     * custom support (and no constructor).
     */
    void addObjectTypes( KJS::ExecState *exec, KJS::Object &parent );

    void addOpaqueTypes( KJS::ExecState *exec, KJS::Object &parent );

    void addValueTypes( KJS::ExecState *exec, KJS::Object &parent );

private:
    KJSEmbedPart *jspart;
    JSEventMapper *evmapper;
    QMap<QString,uint> objtypes;
    class JSFactoryPrivate *d;
};

} // namespace KJSEmbed

#endif // KJSEMBEDFACTORY_H

// Local Variables:
// c-basic-offset: 4
// End:
