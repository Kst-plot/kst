/*
 *  Copyright (C) 2002-2003, Richard J. Moore <rich@kde.org>
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

#include <qfile.h>
#include <kaction.h>
#include <kdebug.h>
#include <kstdaction.h>

#include "xmlactionclient.h"
#include "xmlactionclient.moc"

namespace KJSEmbed {

//
// XML Tags and Attributes
//

QString tag_header("header");
QString tag_action("action");
QString tag_type( "type" );
QString tag_label( "label" );
QString tag_icons( "icons" );
QString tag_shortcut( "shortcut" );
QString tag_name( "name" );
QString tag_group( "group" );
QString tag_text( "text" );
QString tag_statustext( "statustext" );
QString tag_whatsthis( "whatsthis" );
QString tag_script( "script" );
QString tag_data( "data" );
QString tag_item( "item" );

QString attr_type( "type" );
QString attr_src( "src" );
QString attr_exclusive( "exclusive" );

QString type_include( "include" );
QString type_debug( "debug" );

//
// Default Runner
//

bool XMLActionRunner::run( XMLActionClient *client, const XMLActionClient::XMLActionScript &s )
{
//    kdWarning() << "Runner:run called, type=" << s.type << " text=" << s.text << " src=" << s.src << endl;

    if ( s.type == type_include ) {
	kdDebug(80001) << "IncludeAction: " << s.src << endl;
	return client->load( s.src );
    }
    else if ( s.type == type_debug ) {
	kdDebug(80001) << "DebugAction: " << s.text << endl;
	return true;
    }

    return false;
}

//
// Main Client Class
//

XMLActionClient::XMLActionClient( QObject *parent, const char *name )
    : QObject( parent, name ? name : "XMLActionClient" ),
      ac(0), actrun(0)
{
}

XMLActionClient::~XMLActionClient()
{
    delete actrun;
}

bool XMLActionClient::load( const QString &filename )
{
    XMLActionHandler h( this );
    return load( &h, filename );
}

bool XMLActionClient::load( XMLActionHandler *hand, const QString &filename )
{
    QFile f( filename );
    QXmlInputSource src( &f );

    QXmlSimpleReader reader;
    reader.setContentHandler( hand );
    bool ok = reader.parse( src );
    if ( !ok ) {
	kdWarning() << "Loading actionset " << filename << " failed, " << hand->errorString() << endl;
    }

    return ok;
}

bool XMLActionClient::bind( const QString &name, const XMLActionScript &s )
{
//    kdWarning() << "Runner:bind called, name=" << name << " type=" << s.type
//		<< " text=" << s.text << " src=" << s.src << endl;

    scripts[name] = s;
    return true;
}

bool XMLActionClient::bind( KAction *act, const XMLActionScript &s )
{
    if ( !act )
	return false;

//    kdWarning() << "Runner:bind called, action=" << act->name() << " type=" << s.type
//		<< " text=" << s.text << " src=" << s.src << endl;

    connect( act, SIGNAL( activated() ), this, SLOT( action_activated() ) );
    return bind( act->name(), s );
}

bool XMLActionClient::run( const QString &name )
{
    if ( scripts.contains( name ) )
	return run( scripts[name] );
    else
	return false;
}

bool XMLActionClient::run( const XMLActionScript &s )
{
//    kdWarning() << "Client:run called, type=" << s.type << " text=" << s.text << " src=" << s.src << endl;

    if ( actrun )
	return actrun->run( this, s );
    else
	return false;
}

void XMLActionClient::action_activated()
{
    const QObject *sender = QObject::sender();
    if ( !sender )
	return;

    run( sender->name() );
}

//
// SAX Document Handler
//

XMLActionHandler::XMLActionHandler( XMLActionClient *client )
    : QXmlDefaultHandler(), actclient( client )
{
}

bool XMLActionHandler::characters( const QString &chars )
{
    cdata = cdata + chars;
    return true;
}


bool XMLActionHandler::startElement( const QString &, const QString &, const QString &qn,
				     const QXmlAttributes &attrs )
{
    cdata = QString::null;

    if ( qn == tag_script ) {
	ad.script.type = attrs.value( attr_type );
	ad.script.src = attrs.value( attr_src );
    }
    else if ( qn == tag_group ) {
	QString ex = attrs.value( attr_exclusive );
	if ( ex == QString("true") )
	    ad.exclusive = true;
    }
    else if ( qn == tag_action )
	inAction = true;

    return true;
}

bool XMLActionHandler::endElement( const QString &, const QString &, const QString &qn )
{
    if ( qn == tag_action ) {
	defineAction();
	inAction = false;
    }
    else if ( qn == tag_type ) {
	ad.type = cdata;
	cdata = QString::null;
    }
    else if ( qn == tag_label ) {
	ad.text = cdata;
	cdata = QString::null;
    }
    else if ( qn == tag_text ) {
	// Nothing
    }
    else if ( qn == tag_icons ) {
	ad.icons = cdata;
	cdata = QString::null;
    }
    else if ( qn == tag_shortcut ) {
	ad.keys = cdata;
	cdata = QString::null;
    }
    else if ( qn == tag_name ) {
	ad.name = cdata.latin1();
	cdata = QString::null;
    }
    else if ( qn == tag_group ) {
	ad.group = cdata.latin1();
	cdata = QString::null;
    }
    else if ( qn == tag_whatsthis ) {
	ad.whatsthis = cdata;
	cdata = QString::null;
    }
    else if ( qn == tag_statustext ) {
	ad.status = cdata;
	cdata = QString::null;
    }
    else if ( qn == tag_script ) {
	ad.script.text = cdata;
	cdata = QString::null;

	if ( !inAction && ad.script.isValid() )
	    actclient->run( ad.script );
    }
    else if ( qn == tag_item ) {
	ad.items += cdata;
	cdata = QString::null;
    }

    return true;
}

void XMLActionHandler::defineAction()
{
    if ( ad.name.isEmpty() ) {
	kdWarning() << "Attempt to create a KAction without setting a name" << endl;
	return;
    }

    if ( ad.text.isEmpty() )
	ad.text = ad.name;

    KAction *act = createAction( actclient->actionCollection() );
    if ( act && ad.script.isValid() )
	actclient->bind( act, ad.script );

    ad.clear();
    cdata = QString::null;
}

KAction *XMLActionHandler::createAction( KActionCollection *parent )
{
//    kdDebug(80001) << "Creating Action, type is " << type << endl;
//    kdDebug(80001) << "text=" << text << ", icons=" << icons << endl;
//    kdDebug(80001) << "keys=" << keys << ", name=" << name << endl;

    if ( !parent ) {
	kdWarning() << "Create action called but no parent set" << endl;
	return 0;
    }

    KAction *act=0;

    if ( ad.type.isEmpty() || (ad.type == "KAction") ) {
	act = new KAction( ad.text, ad.icons, ad.keys, 0, 0, parent, ad.name.latin1() );
    }
    else if ( ad.type == "KToggleAction" ) {
	act = new KToggleAction( ad.text, ad.icons, ad.keys, 0, 0, parent, ad.name.latin1() );
    }
    else if ( ad.type == "KRadioAction" ) {
	KRadioAction *ra = new KRadioAction( ad.text, ad.icons, ad.keys, 0, 0, parent, ad.name.latin1() );
	if ( ad.exclusive )
	    ra->setExclusiveGroup( ad.group );

	act = ra;
    }
    else if ( ad.type == "KStdAction" ) {
	for ( int i = KStdAction::ActionNone ; i < KStdAction::ConfigureNotifications ; i++ ) {
	    if ( KStdAction::stdName(static_cast<KStdAction::StdAction>(i)) == ad.name )
		act = KStdAction::create( (KStdAction::StdAction)i, 0, 0, parent );
	}
    }
    else if ( ad.type == "KListAction" ) {
	KListAction *la = new KListAction( ad.text, ad.icons, ad.keys, 0, 0, parent, ad.name.latin1() );
	la->setItems( ad.items );
	ad.items.clear();
	act = la;
    }
    else if ( ad.type == "KActionMenu" ) {
	KActionMenu *am = new KActionMenu( ad.text, ad.icons, parent, ad.name.latin1() );

	for ( QStringList::Iterator it = ad.items.begin() ; it != ad.items.end() ; ++it ) {
	    KAction *a = parent->action( (*it).latin1() );
	    if ( a )
		am->insert( a );
	}
	ad.items.clear();
	act = am;
    }
    else {
	kdWarning() << "Unknown ActionType " << ad.type << endl;
	return 0;
    }

    if ( !act ) {
	kdWarning() << "Unable to create action" << endl;
	return act;
    }

    if ( !ad.group.isEmpty() )
	act->setGroup( ad.group );

    act->setStatusText( ad.status );
    act->setWhatsThis( ad.whatsthis );

    QObject::connect( actclient, SIGNAL( destroyed() ), act, SLOT( deleteLater() ) );

    return act;
}

} // namespace KJSEmbed
