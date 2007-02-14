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

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kjs/interpreter.h>

#include <jsconsolewidget.h>
#include <jsobjectproxy.h>
#include <jsfactory.h>
#include <jssecuritypolicy.h>

int main( int argc, char **argv )
{
    KAboutData about( "test-kjsembed", I18N_NOOP("KJS Embed Test"), "0.1",
		      I18N_NOOP("Test"),
		      KAboutData::License_LGPL, I18N_NOOP("(c) 2001-2003 Richard Moore") );
    KCmdLineArgs::init( argc, argv, &about );
    KApplication app;

    // Setup Interpreter
    KJS::Interpreter *js = KJSEmbed::JSFactory::defaultJS();
    KJS::Object global = js->globalObject();

    // Create Console
     KJSEmbed::JSConsoleWidget *console = new  KJSEmbed::JSConsoleWidget( js );

    console->addBindings( global );
    JSFactory::publish( &app, js, global, "app" );
    JSFactory::publish( console, js, global, "console" );

    // Setup Window
    app.setMainWidget( console );
    app.connect( &app, SIGNAL( lastWindowClosed() ), SLOT(quit()) );
    console->resize( 600, 450 );
    console->show();

    return app.exec();
}

// Local Variables:
// c-basic-offset: 4
// End:
