/*
 *  Copyright (C) 2002-2004, Richard J. Moore <rich@kde.org>
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

/**
 * @mainpage Framework for embedding the KJS Javascript Interpreter
 *
 * @section intro Introduction
 *
 * The KJSEmbed library provides a framework that makes it easy for
 * applications to embed KJS, the KDE JavaScript interpreter. The
 * facilities available include a JS console widget, a dialog loader
 * and a binding between JS and the properties and slots of QObjects.
 *
 * @section classes Important Classes
 *
 * The most important classes to consider are:
 *
 * @ref KJSEmbed::KJSEmbedPart :
 *   Main API for KJSEmbed.
 *
 * @ref KJSEmbed::JSConsoleWidget :
 *   A widget that provides an interactive JS console.
 *
 * @ref KJSEmbed::JSObjectProxy :
 *   A Javascript object that can access the properties of a QObject,
 *
 * @ref KJSEmbed::SecurityPolicy :
 *   Defines a security policy for @ref JSObjectProxy.
 *
 * @section basic Basic Usage
 *
 * The simplest way to use KJSEmbed is by simply creating a Javascript
 * console widget. The console allows the user to enter and run arbitrary
 * Javascript expressions.
 * <pre>
 *    KJSEmbed::JSConsoleWidget *win = new KJSEmbed::JSConsoleWidget();
 *    win->show();
 * </pre>
 * The embedding application can run scripts in the console using the
 * execute() method.
 *
 * The best way to use KJSEmbed is keep control of the interpreter
 * yourself using the KJSEmbedPart, this way you can make parts of your
 * application available to scripts. The following example creates its
 * own interpreter then binds it to the console:
 * <pre>
 *    KJSEmbed::KJSEmbedPart *js = new KJSEmbed::KJSEmbedPart();
 *    KJSEmbed::JSConsoleWidget *console = js->view();
 * </pre>
 *
 * @section proxy Publishing QObjects
 *
 * KJSEmbed allows applications to make arbitrary QObjects visible to a
 * Javascript interpreter. The binding itself is provided by the @ref JSProxyObject
 * class, but is more easily used via the addObject(...) methods of @ref KJSEmbedPart.
 *
 * The following code shows how easy it is to make an object available for
 * scripting. It creates a QVBox containing two QLabels then makes them visible
 * to KJSEmbed:
 * <pre>
 *    QVBox *toplevel = new QVBox( 0, "box" );
 *    QLabel *title = new QLabel( "Some Title", toplevel, "title");
 *    QLabel *main = new QLabel( "Some text, more text.", toplevel, "main" );
 *
 *    js->addObject( title );
 *    js->addObject( main, "text" );
 * </pre>
 *
 * Publishing an object makes it possibile for scripts to access both the
 * properties and slots as if it was a normal Javascript object. The code
 * above allows scripts read-write access to the label properties as this
 * script illustrates:
 * <pre>
 *    title.text = "World"
 *    title.text = "Hello " + title.text
 * </pre>
 * The script above would set the text of the label to 'Hello World'.
 *
 * The slots of a QObject bound to the interpreter are made available to
 * scripts as if they normal methods. In the example above, we could conceal
 * the label 'main' entirely by calling its hide() slot:
 * <pre>
 *    main.hide()
 * </pre>
 *
 * @section tree Access To the QObject Tree
 *
 * As well as providing script access to an individual widget, KJSEmbed
 * allows scripts to walk the object tree and access others.  If we
 * modified the previous example to publish the QBox widget 'toplevel' as
 * follows:
 * <pre>
 *    js->addObject( toplevel, "window" );
 * </pre>
 * Then, despite the fact we've only explicitly published a single widget,
 * we've also provided access to both 'main' and 'title'. The ability
 * to navigate the object tree is limited by the SecurityPolicy, the default
 * policy only allows scripts access to children of the published object.
 *
 * To achieve the same result as before, we could use script like this:
 * <pre>
 *    window.child("main").text = "World"
 *    window.child("main").text = "Hello " + window.child("main").text
 * </pre>
 * The result of this script is identical to the previous example.
 *
 * @section examples Some KJSEmbed examples
 * @subsection embedjs Example of embedding KJSEmbed into an application.
 * @image html embedjs.png
 * This is an example of how to embed and interface with KJSEmbed.  This
 * example covers:
 * @li embedding the kpart.
 * @li publishing the interface.
 * @li calling javascript members.
 * @li handling javascript objects returned by these members.
 * @li embedding the KJSEmbed console.
 * @dontinclude embedviewimp.cpp
 * To embed the interpreter we can just create a new KJSEmbed part.
 * @skipline m_part
 * To publish the objects we can then call @ref KJSEmbed::KJSEmbedPart::addObject on our part.
 * This will then add any QObject based class the the global scope of the
 * javascript interperter.
 * @skipline addObject
 * Once you have your objects published you can then execute javascript code from a file.
 * @skipline runFile
 * @dontinclude embedviewimp.cpp
 * When the script is running javascript methods can be accessed by calling the
 * @ref KJSEmbed::KJSEmbedPart::callMethod method.
 * @skipline args
 * @until callMethod
 * Any arguments that you wish to pass into the javascript method are contained in the
 * @ref KJS::List. This is just a list of @ref KJS::Value objects and can be created from
 * QVariants or from custom @ref KJS::Object based classes.
 * Once you have the data back from the method you can convert it easily from the @ref KJS::Value
 * type to a QVariant with the @ref KJSEmbed::convertToVariant method.
 * @line personalData
 * @until notes:
 * Complex arrays or @ref KJS::Objects are transparently converted to QVariant::Map types so
 * they can easily be manipulated from C++.
 * The KJSEmbed::console is also easy to add to applications.  An example of a method that
 * will toggle the console is below.
 * @skipline consoleClicked
 * @until }
 *
 * @subsection embedjs Example of Using Qt Designer files in KJSEmbed.
 * @image html jscalc.png
 * This is a very simple example that shows off how to use Qt widget files
 * and connect the objects to javascript functions.
 * @dontinclude calc.js
 * To load the Qt Designer user interface file and publish the objects in the XML file
 * the KJSEmbed Factory class has a UI loader.
 * @skipline Factory.loadui
 * Once the file is loaded the user interface object can then be manipulated by javascript.
 * @line new Calculator
 * @until application.exec
 * It is important to note that before the javascript will support connecting signals and slots
 * the application.exec() method must be called.
 *
 * Connecting the user interface to javascript methods is similar C++ in that you create
 * a method and then use the connect(...) method to connect the signal to the method.
 * @dontinclude calc.js
 * @skipline function
 * @until display
 * @skipline this.clear
 * @skipline clear
 * @skipline }
 */
