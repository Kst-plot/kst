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
#include <global.h>
#include <kjs/interpreter.h>
#include <kjsembed/kjsembedpart.h>

#include "builtins/stddialog_imp.h"
#include "qtbindings/qt_imp.h"

#ifndef QT_ONLY
#include "builtins/stdaction_imp.h"
#include "builtins/resources.h"
#endif // QT_ONLY

#include "bindings/kconfig_imp.h"

#include "jsfactory_imp.h"
#include "jsbuiltinproxy.h"

#include "jsbuiltin_imp.h"
#include "jsbuiltin.h"

namespace KJSEmbed {

typedef Bindings::JSBuiltInImp JSBuiltInImp;
typedef Bindings::JSFactoryImp JSFactoryImp;
typedef JSProxy::MethodTable MethodTable;
typedef Bindings::Config Config;

JSBuiltIn::JSBuiltIn( KJSEmbedPart *part )
    : jspart(part)
{
    init( jspart->globalExec() );
}

JSBuiltIn::~JSBuiltIn()
{
}

void JSBuiltIn::init( KJS::ExecState *exec )
{
    JSFactory *jsfact = jspart->factory();

    //
    // Setup the Factory object
    //
    factory = KJS::Object(new JSBuiltinProxy("Factory") );

    MethodTable factoryMethods[] = {
	{ JSFactoryImp::MethodCreateObject, "createObject" },
	{ JSFactoryImp::MethodCreateROPart, "createROPart" },
	{ JSFactoryImp::MethodLoadUI, "loadui" },
	{ JSFactoryImp::MethodWidgets, "widgets" },
	{ JSFactoryImp::MethodConstructors, "constructors" },
	{ JSFactoryImp::MethodIsSupported, "isSupported" },
	{ JSFactoryImp::MethodTypes, "types" },
	{ JSFactoryImp::MethodListPlugins, "listBindingPlugins" },
	{ JSFactoryImp::MethodCreateRWPart, "createRWPart" },
	{ 0, 0 }
    };

    int i = 0;
    do {
	JSFactoryImp *obj = new JSFactoryImp( exec, jsfact,
					      factoryMethods[i].id, factoryMethods[i].name );
	factory.put( exec , obj->name(), KJS::Object(obj), KJS::Function );
	i++;
    } while( factoryMethods[i].id );

    //
    // Setup the System object
    //
    system = KJS::Object(new JSBuiltinProxy("System")  );
    MethodTable systemMethods[] = {
	{ JSBuiltInImp::MethodOpenFile, "openFile" },
	{ JSBuiltInImp::MethodReadFile, "readFile" },
	{ JSBuiltInImp::MethodWriteFile, "writeFile" },
	{ JSBuiltInImp::MethodExit, "exit" },
	{ 0, 0 }
    };

    i = 0;
    do {
	JSBuiltInImp *obj = new JSBuiltInImp( this, systemMethods[i].id, systemMethods[i].name );
	system.put( exec , obj->name(), KJS::Object(obj), KJS::Function );
	i++;
    } while( systemMethods[i].id );

    // The stdio streams
    system.put( exec ,"stdin", jsfact->createProxy( exec, KJSEmbed::conin() ), KJS::Function );
    system.put( exec ,"stdout", jsfact->createProxy( exec, KJSEmbed::conout() ), KJS::Function );
    system.put( exec ,"stderr", jsfact->createProxy( exec, KJSEmbed::conerr() ), KJS::Function );

    // Qt namespace object
    qt = KJS::Object(new JSBuiltinProxy("Qt") );
    QtImp::addStaticBindings( exec, qt );
    QtImp::addBindings( exec, qt );

    //  The global config object
    Config *scriptCfg = new Config( jspart );

    system.put( exec, "KJSConfig", jsfact->createProxy( exec, scriptCfg ) );

    //
    // Setup builtin KDE objects
    //
    stddialog = KJS::Object(new JSBuiltinProxy("StdDialog") );
    BuiltIns::StdDialogImp::addBindings( exec, stddialog );

#ifndef QT_ONLY
    stdaction = KJS::Object(new JSBuiltinProxy("StdAction")  );
    BuiltIns::StdActionImp::addBindings( jsfact, exec, stdaction );

    stddrs = KJS::Object(new JSBuiltinProxy("StdDirs")  );
    BuiltIns::StdDirsImp::addBindings( exec, stddrs );

    stdicns = KJS::Object(new JSBuiltinProxy("StdAction") );
    BuiltIns::StdIconsImp::addBindings( jsfact, exec, stdicns );
#endif // QT_ONLY
}

void JSBuiltIn::add( KJS::ExecState *exec, KJS::Object &parent )
{
    // Add the static objects
    parent.put( exec, "Factory", builtinFactory() );
    parent.put( exec, "System", builtinSystem() );
    parent.put( exec, "StdDialog", builtinStdDialog() );
    parent.put( exec, "Qt", builtinQt() );
#ifndef QT_ONLY
    parent.put( exec, "StdAction", builtinStdAction() );
    parent.put( exec, "StdDirs", builtinStdDirs() );
    parent.put( exec, "StdIcons", builtinStdIcons() );
#endif // QT_ONLY

    // Redundant, and will break things, use dump(this) instead
    //parent.put( exec, "Global", exec->interpreter()->globalObject() );

    // Add the global functions
    addGlobals( exec, parent );
}

void JSBuiltIn::addGlobals( KJS::ExecState *exec, KJS::Object &parent )
{
    MethodTable methods[] = {
	{ JSBuiltInImp::MethodLoadScript,"load" },
	{ JSBuiltInImp::MethodDumpObject,"dump" },
	{ JSBuiltInImp::MethodRunKSpy,"run_kspy" },
	{ JSBuiltInImp::MethodPrint,"print" },
	{ JSBuiltInImp::MethodPrintLn, "println" },
	{ JSBuiltInImp::MethodWarn,"warn" },
	{ JSBuiltInImp::MethodReadLine,"readLine" },
	{ JSBuiltInImp::MethodExit,"exit" },
	{ JSBuiltInImp::MethodDumpCompletion,"dumpCompletion" },
	{ JSBuiltInImp::MethodSaxLoadFile,"saxLoadFile" },
	{ JSBuiltInImp::MethodAlert,"alert" },
	{ JSBuiltInImp::MethodConfirm,"confirm" },
	{ JSBuiltInImp::MethodPrompt,"prompt" },
	{ JSBuiltInImp::MethodI18n,"i18n" },
	{ JSBuiltInImp::MethodImport,"include" },
	{ JSBuiltInImp::MethodShell, "shell" },
	//{ JSBuiltInImp::MethodImport,"import" }, reserved keyword
	{ 0, 0 }
    };
    int i = 0;
    do {
	JSBuiltInImp *obj = new JSBuiltInImp( this, methods[i].id, methods[i].name );
    parent.put( exec , obj->name(), KJS::Object(obj), KJS::Function );
	i++;
    } while( methods[i].id );
}

} // namespace KJSEmbed

