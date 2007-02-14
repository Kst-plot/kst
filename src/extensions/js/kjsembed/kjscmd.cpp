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

#include <stdio.h>

#include <errno.h>
#include <qstring.h>

#include <qregexp.h>

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kimageio.h>
#include <klocale.h>
#include <kmainwindow.h>

#include <kjs/interpreter.h>
#include <kjs/ustring.h>

#include "kjsembedpart.h"
#include "jsconsolewidget.h"
#include "jssecuritypolicy.h"

static KCmdLineOptions options[] =
{
    { "c", 0, 0 },
    { "console", I18N_NOOP("Displays the KJSEmbed console"), 0 },
    { "i", 0, 0 },
    { "interactive", I18N_NOOP("Runs an interactive command line prompt"), 0 },
    { "exec", I18N_NOOP("Call application.exec() automatically after running the script"), 0 },
    { "nogui", I18N_NOOP("Disables all GUI facilities"), 0 },
    { "+[file]", I18N_NOOP("Script to execute, or '-' for stdin"), 0 },
    { 0, 0, 0 }
};

int main( int argc, char **argv )
{
    bool nogui = false;
    bool doexec = false;
    QCString appName = "kjscmd";

    // Set the name of the instance
    for ( int i = 1 ; i < argc ; i++ ) {
        QCString arg( argv[i] );
	if ( arg[0] != '-' ) {
	    appName = arg.mid( arg.findRev('/') );
	    if ( !appName )
		appName = argv[1];
	    else
		argv[0] = appName.data();
	}
    }
    
    // Clean app name
    appName = appName.replace("/", "");
    appName = appName.replace(".js", "");

    // Search for nogui option
    for ( int i = 1 ; i < argc ; i++ ) {
	if ( strcmp( argv[i], "--exec" ) == 0 )
	    doexec = true;
	if ( strcmp( argv[i], "--nogui" ) == 0 )
	    nogui = true;
    }

    // Cmd Line
    KAboutData about( appName.data(), I18N_NOOP("KJSCmd"), KJSEMBED_VERSION_STRING,
		      I18N_NOOP("Runs KJS scripts from the command line."),
		      KAboutData::License_LGPL, I18N_NOOP("(c) 2001-2004 Richard Moore") );
    about.addAuthor( "Richard Moore", 0, "rich@kde.org" );
    about.addAuthor( "Ian Reinhart Geiser", 0, "geiser@kde.org" );

    KCmdLineArgs::init( argc, argv, &about );
    KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    // Setup KApplication
    KApplication *app;
    if ( nogui ) {
	app = new KApplication( false, false );
    }
    else {
	app = new KApplication;
	app->connect( app, SIGNAL( lastWindowClosed() ), SLOT(quit()) );
    }

    // Setup the rest
    KImageIO::registerFormats();

    // Setup Interpreter
    KJSEmbed::JSSecurityPolicy::setDefaultPolicy( KJSEmbed::JSSecurityPolicy::CapabilityAll );
    KJSEmbed::KJSEmbedPart *part = new KJSEmbed::KJSEmbedPart;
    KJS::Interpreter *js = part->interpreter();
    KJS::ExecState *exec = js->globalExec();

    // Publish bindings
    KJS::Object appobj = part->addObject( app, "application" );

    // Build args array
    KJS::List l;
    for ( int i = 1 ; i < args->count() ; i++ )
	l.append( KJS::String( args->arg(i) ) );

    KJS::Object argobj( js->builtinArray().construct( exec, l ) );
    appobj.put( exec, "args", argobj );

    bool showConsole = false;

    // Create Visible Console?
    if ( args->isSet("console")
         || ( !args->count() && !args->isSet("interactive") ) ) {

	if ( !nogui ) {

	    KJSEmbed::JSConsoleWidget *console = part->view();
	    console->resize( 600, 450 );
	    console->show();
	    part->addObject( console, "console" );

	    showConsole = true;
	}
    }

    if ( args->count() ) {
	// Run script
	bool ok = part->runFile( args->arg(0) );
	KJS::Completion jsres = part->completion();
	if ( jsres.complType() != KJS::Normal) {
	    QTextStream err( stderr, IO_WriteOnly );
	    QTextStream out( stdout, IO_WriteOnly );
	    
	    switch ( jsres.complType() ) {
	    case KJS::Break:
	    case KJS::Continue:
	        err << jsres.value().toString(exec).qstring() << endl;
		return 1;
		break;
	    case KJS::ReturnValue:
	        out << jsres.value().toString(exec).qstring() << endl;
		return 0;
		break;
	    case KJS::Throw:
	    {
	
          KJS::Object exception = jsres.value().toObject(exec);
        int line = exception.get(exec,KJS::Identifier("line")).toNumber(exec);
        QString type = exception.get(exec,KJS::Identifier("name")).toString(exec).qstring();
        QString message = exception.get(exec,KJS::Identifier("message")).toString(exec).qstring();
        
        err << "Uncaught " << type << " exception at: " << line << endl;
        err << message << endl;
		return 1;
	        break;
	    }
	    default:
	        err << "Unknown error." << endl;
		return 1;
	    	break;
	    }
	}
    }

    int result = 0;
    if ( !nogui && ( doexec || showConsole ) )
	result = app->exec();

    if ( args->isSet( "interactive" ) ) {
	part->execute( QString("include('cmdline.js');") );
    }

    return result;
}

// Local Variables:
// c-basic-offset: 4
// End:
