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

#ifndef KJSEMBEDJSCONSOLEPART_H
#define KJSEMBEDJSCONSOLEPART_H
#include "global.h"
#include <qstringlist.h>

#ifndef QT_ONLY
#include <kparts/part.h>
#endif

#include <kjs/interpreter.h>
#include <kjs/object.h>
#include <kjs/value.h>

#include <kjsembed/jsbinding.h>

#ifndef QT_ONLY
#include <kjsembed/xmlactionclient.h>
#else
#include <kjsembed/qtstubs.h>
#endif

class QWidget;

#define KJSEMBED_VERSION_STRING "0.3"
#define KJSEMBED_VERSION_MAJOR 0
#define KJSEMBED_VERSION_MINOR 3

/**
 * Namespace containing the KJSEmbed library.
 */
namespace KJSEmbed {

class JSObjectProxy;
class JSConsoleWidget;
class JSFactory;
class JSBuiltIn;

/**
 * A KPart for embedding KJS in an application.
 *
 * @version $Id$
 * @author Richard Moore, rich@kde.org
 * This is the easiest class for interacting and embedding the interpreter.  To use KJSEmbed in yours
 * application you can simply create a new KJSEmbedPart.  Once you have this object you can expose your
 * QObect based classes with a few simple methods.  The interface also allows for the manipulation of QVariant data
 * structures between Javascript and C++.  Complex Javascript arrays are transparently converted to QMaps and QValueLists
 * depending on the type of the Array.  KJSEmbed also provides transparent access to most QVariant datatypes between
 * C++ and Javascript.  The easiest way to integrate KJSEmbed into your application is to create a QObject based class
 * that will be your interface between C++ and Javascript.  This object than can expose C++ methods via Qt slots, and
 * QVariant datatypes via Qt properties.  This class will also have an interface where C++ objects can call Javascript
 * methods.
 */
class KJSEMBED_EXPORT KJSEmbedPart : public KParts::ReadOnlyPart
{
    Q_OBJECT
    Q_PROPERTY( QStringList constructorNames READ constructorNames )
    Q_PROPERTY( QCString versionString READ versionString )
    Q_PROPERTY( int versionMajor READ versionMajor )
    Q_PROPERTY( int versionMinor READ versionMinor )

public:
    /** Create a KJSEmbedPart. */
    KJSEmbedPart( QObject *parent=0, const char *name=0 );

    /**
     * Creates a KJSEmbedPart for which the parents of the view and part are different.
     *
     * Caution! you cannot use parent = [KQ]Application and wparent = 0L.
     * This will cause a crash on exit since the widget will not be destroyed
     * but the part will get destroyed at the end of the QApplication
     * constructor.  This results in the widget trying to do GUI related things
     * after the QPaint stuff is destroyed.
     */
    KJSEmbedPart( QWidget *wparent, const char *wname=0, QObject *parent=0, const char *name=0 );

    /**
     * Create a KJSEmbedPart with the specified interpreter.
     *
     * See warning above.
     */
    KJSEmbedPart( KJS::Interpreter *js, QWidget *wparent, const char *wname=0,
		  QObject *parent=0, const char *name=0 );

    /** Cleans up. */
    virtual ~KJSEmbedPart();

    //
    // Version information
    //

    /** Returns the version of KJSEmbed. */
    QCString versionString() const;

    /** Returns the minor version number of KJSEmbed. */
    int versionMajor() const;

    /** Returns the major version number of KJSEmbed. */
    int versionMinor() const;

    //
    // Script environment.
    //

    /** Checks to see if a desired method is available in Javascript.
    * @code
    * if ( js->hasMethod( "javascriptFunction" ) )
    *   ... // do something with that function.
    * else
    *   ... // function not present.
    */
    bool hasMethod( const QString &methodName );

    /** Calls a method from Javascript starting from the global context.
    * @code
    * KJS::List args;
    * args.append(KJS::String("String Arg") );
    * args.append(KJS::Number(10) );
    * args.append(KJSEmbed::convertToValue(js->globalExec(), QColor("blue" ) ) );
    * KJS::Value val = js->callMethod("javascriptFunction", args );
    * QString myString = val.toString().qstring();
    * @endcode
    */

    KJS::Value callMethod(const QString &methodName, const KJS::List &args) const;
    /** Gets a value from Javascript starting from the global context.
    * @code
    * KJS::Value val = js->getValue("someValue");
    * QString myString = val.toString().qstring();
    * @endcode
    */
    KJS::Value getValue( const QString &valueName ) const;

    /** Sets a value in a Javascript starting from the global context.
    * Note if this value is not present in the javascript context it will
    * become defined once this method is called.
    * @code
    * KJS::Value val = KJSEmbed::convertToValue(js->globalExec(), QColor("blue"));
    * js->putValue("myVar", val);
    * @endcode
    */
    void putValue( const QString &valueName, const KJS::Value &value);

    /** Convinence method that wraps around @ref putValue that only handles values
    * that are supported by QVariant.
    * @code
    * js->putVariant( "myVar", QColor("blue") );
    * @endcode
    */
    void putVariant( const QString &valueName, const QVariant &value);

    /** Convinence method that wraps around @ref getValue that only handles values
    * that are supported by QVariant.
    * @code
    * QColor value = js->getVariant( "myVar").toColor();
    * @endcode
    */
    QVariant getVariant( const QString &valueName ) const;

    /** Returns the global object of the interpreter. */
    KJS::Object globalObject() const { return js->globalObject(); }

    /** Returns the completion object for the last script executed. */
    KJS::Completion completion() const { return res; }

    /** Returns the JS object of the part. */
    KJS::Object partObject() const { return partobj; }

    /**
     * Publishes 'obj' as property 'name' of the global object. If 'name' is
     * not specified then the name property of 'obj' is used instead.
     * @code
     * // In C++
     * QObject *myClass = new QObjectBasedClass(this, "jsObject");
     * js->addObject( myClass );
     * // In Javascript
     * jsObject.someProperty = "Test Property";
     * // In C++ again
     * kdDebug() << "New Value: " << myClass->someProperty() << endl;  // displays "Test Property"
     * @endcode
     */
    KJS::Object addObject( QObject *obj, const char *name=0 );

    /**
     * Publishes 'obj' as property 'name' of object 'parent'. The binding is defined
     * using @ref JSObjectProxy, and is subject to the current default SecurityPolicy.
     */
    KJS::Object addObject( QObject *obj, KJS::Object &parent, const char *name=0 );

    /** Returns a JS::Object that provides a binding to the specified QObject. */
    KJS::Object bind( QObject *obj );

    //
    // Query the supported types
    //

    /** Returns a list of the names of the constructors of this interpreter. */
    QStringList constructorNames() const;

    /** Returns a JS Array of the constructors of this interpreter. */
    KJS::Value constructors() const;

    /** Returns a list of the constructors of this interpreter. */
    KJS::List constructorList() const;

public slots:
    //
    // Reimplement the KPart API.
    //

    /** Returns the view widget, creating one if required.*/
    virtual KJSEmbed::JSConsoleWidget *view();

    /** Opens a URL. If the URL has the protocol 'javascript' then it is executed. */
    virtual bool openURL( const KURL &url );

    //
    // XML Action Handling
    //

    /** Returns the current XMLActionClient. */
    KJSEmbed::XMLActionClient *actionClient() const { return xmlclient; }

    /** Loads the XML actions defined in the specified file to the default XMLActionClient. */
    bool loadActionSet( const QString &file );

    //
    // Script environemnt
    //

    /** Returns the current interpreter. */
    KJS::Interpreter *interpreter() const { return js; }

    /** Returns the execution context of the interpreter. */
    KJS::ExecState *globalExec() const { return js->globalExec(); }

    /** Returns the current factory object. */
    JSFactory *factory() const { return jsfactory; }

    /** Evaluates the specified string, with the specified value as 'this'. */
    virtual KJS::Value evaluate( const QString &script, const KJS::Value &self=KJS::Null() );

    /** Executes the specified string, with the specified value as 'this'. */
    bool execute( const QString &script, const KJS::Value &self=KJS::Null() );

    /**
     * Executes the specified string, with the specified value as 'this' and sets
     * result to the completion object returned by the interpreter.
     */
    virtual bool execute( KJS::Completion &result, const QString &script, const KJS::Value &self );

    /** Loads and runs the specified file. */
    virtual bool runFile( const QString &filename, const KJS::Value &self=KJS::Null() );

    /**
     * Loads but does not execute the specified script file. If the
     * first line of the file starts with the string '#!' then that
     * line will be omitted from the returned string.
     */
    QString loadFile( const QString &file );

protected:
    /** Creates the interpreter. */
    void createInterpreter();

    /**
     * Creates the built-in objects and methods in the specified parent object
     * normally the global object of the interpreter. This method is called
     * automatically unless you create the part with a custom interpreter, if
     * you do then you have to call this method yourself if you want the
     * built-ins to be available.
     */
    void createBuiltIn( KJS::ExecState *exec, KJS::Object &parent );

    /** Sets up the XMLActionClient. */
    void createActionClient();

    /** Reimplemented to disable file support. */
    virtual bool openFile() { return false; }

    /** @internal Hook for virtual that cannot break compatibility. */
    virtual void virtual_hook( int id, void *data );

private:
    XMLActionClient *xmlclient;
    JSConsoleWidget *jsConsole;
    JSFactory *jsfactory;
    JSBuiltIn *builtins;
    QWidget *widgetparent;
    QCString widgetname;

    KJS::Object partobj;
    KJS::Completion res;
    KJS::Interpreter *js;
    bool deletejs;

    class KJSEmbedPartPrivate *d;
};


} // namespace KJSEmbed

#endif // KJSEMBEDJSCONSOLEPART_H

// Local Variables:
// c-basic-offset: 4
// End:

