/*
 *  Copyright (C) 2004, Richard J. Moore <rich@kde.org>
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

#include <stdio.h>
#include <errno.h>

#include <qstring.h>
#include <qregexp.h>
#include <qapplication.h>
#include <qmainwindow.h>

#include <kjs/interpreter.h>
#include <kjs/ustring.h>

#include "kjsembedpart.h"
#include "jsconsolewidget.h"
#include "jssecuritypolicy.h"
#include "kjsembed/global.h"

int main( int argc, char **argv )
{
#ifdef _WIN32
#   ifdef CONSOLEIO
    RedirectIOToConsole();
#   endif
#endif
    // Setup QApplication
    QApplication *app;
    app = new QApplication( argc, argv );
    app->connect( app, SIGNAL( lastWindowClosed() ), SLOT(quit()) );

    // Setup Interpreter
    KJSEmbed::JSSecurityPolicy::setDefaultPolicy( KJSEmbed::JSSecurityPolicy::CapabilityAll );
    KJSEmbed::KJSEmbedPart *part = new KJSEmbed::KJSEmbedPart;
    KJS::Interpreter *js = part->interpreter();
    KJS::ExecState *exec = js->globalExec();

    // Publish bindings
    KJS::Object appobj = part->addObject( app, "application" );

    // Build args array
    KJS::List l;
    for ( int i = 1 ; i < argc ; i++ )
	l.append( KJS::String( argv[i] ) );

    KJS::Object argobj( js->builtinArray().construct( exec, l ) );
    appobj.put( exec, "args", argobj );

    if ( argc > 1 ) {
	// Run script
	bool ok = part->runFile( argv[1] );
	if ( !ok ) {
	    KJS::Completion jsres = part->completion();
	    (*KJSEmbed::conerr()) << jsres.value().toString(exec).qstring() << endl;
	    return 1;
	}
    }

    int result = 0;
    part->execute( QString("include('cmdline.js');") );

    return result;
}

// Local Variables:
// c-basic-offset: 4
// End:
