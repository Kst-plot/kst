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

#include <kaction.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kinstance.h>
#include <klocale.h>
#include <kparts/part.h>

#include <jsfactory.h>
#include <jsconsolewidget.h>
#include <kjsembedpart.h>

#include "jsconsoleplugin.h"
#include "jsconsoleplugin.moc"

typedef KGenericFactory<KJSEmbed::JSConsolePlugin> JSConsolePluginFactory;
K_EXPORT_COMPONENT_FACTORY( libjsconsoleplugin, JSConsolePluginFactory( "jsconsoleplugin" ) )

//
// KParts::Plugin Implementation
//
namespace KJSEmbed {

JSConsolePlugin::JSConsolePlugin( QObject *parent, const char *name, const QStringList & )
    : KParts::Plugin( parent, name )
{
    js = 0;

    KActionMenu *menu;
    menu = new KActionMenu( i18n("&JavaScript"), "jsconsole",
			    actionCollection(), "jsconsole" );
    menu->setDelayed( false );

    menu->insert( new KAction( i18n("&Console"), 0,
			       this, SLOT( showConsole() ),
			       actionCollection(), "jsconsole_show") );
}

void JSConsolePlugin::init()
{
    Q_ASSERT( js == 0 );
    js = new KJSEmbedPart( 0, 0, name(), 0, name() ); // shouldn't "this" be the parent?

    KJS::Interpreter *interp = js->interpreter();
    KJS::Object global( interp->globalObject() );
    js->addObject( js->view(), global, "console" );
    js->addObject( parent(), global, "plugin_parent" );
    js->addObject( KApplication::kApplication(), global, "application" );

    if ( parent()->inherits("KParts::Part") ) {
	KParts::Part *part = static_cast<KParts::Part *>( parent() );
	js->addObject( part, "kpart" );
	if ( part->widget() ) {
	    js->addObject( part->widget(), "widget" );
	    js->addObject( part->widget()->topLevelWidget(), "window" );
	}
    }
}

JSConsolePlugin::~JSConsolePlugin()
{
    delete js;
}

void JSConsolePlugin::showConsole()
{
    if ( !js )
        init();
    js->widget()->show();
}

} // namespace KJSEmbed

// Local Variables:
// c-basic-offset: 4
// End:




